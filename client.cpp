#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <cstdint>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>


#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8080
#define BUF_SIZE 1024

using namespace std;

// Structure to store DNS query and timestamp
struct DNSQuery {
    string domain;
    uint32_t ts_sec;
    uint32_t ts_usec;
};

// Build custom header HHMMSSID using current laptop time
string buildCustomHeader(size_t seqId) {
    time_t rawtime = time(nullptr);
    tm *ltm = localtime(&rawtime);
    ostringstream oss;
    oss << setw(2) << setfill('0') << ltm->tm_hour
        << setw(2) << setfill('0') << ltm->tm_min
        << setw(2) << setfill('0') << ltm->tm_sec
        << setw(2) << setfill('0') << seqId;
    return oss.str();
}

// Parse PCAP and extract DNS queries
vector<DNSQuery> parsePcapForDNS(const string &filename) {
    vector<DNSQuery> queries;
    ifstream f(filename, ios::binary);
    if(!f) { cerr << "Cannot open PCAP file\n"; return queries; }

    char globalHeader[24]; f.read(globalHeader, 24); // skip global header

    while(f) {
        char pktHeader[16];
        f.read(pktHeader, 16);
        if(f.gcount() < 16) break;

        uint32_t ts_sec = *(uint32_t*)(pktHeader);
        uint32_t ts_usec = *(uint32_t*)(pktHeader+4);
        uint32_t incl_len = *(uint32_t*)(pktHeader + 8);

        vector<unsigned char> pktData(incl_len);
        f.read((char*)pktData.data(), incl_len);
        if(pktData.size() < 42) continue;

        unsigned char protocol = pktData[23];
        uint16_t srcPort = (pktData[34] << 8) | pktData[35];
        uint16_t dstPort = (pktData[36] << 8) | pktData[37];
        if(protocol != 17 || (dstPort != 53 && srcPort != 53)) continue;

        size_t dnsOffset = 42;
        if(dnsOffset >= pktData.size()) continue;

        size_t pos = dnsOffset + 12; // skip DNS header
        string domain;
        while(pos < pktData.size() && pktData[pos] != 0) {
            int len = pktData[pos++];
            if(pos + len > pktData.size()) break;
            for(int i=0;i<len;i++) domain += pktData[pos+i];
            pos += len;
            if(pktData[pos-1] != 0) domain += ".";
        }

        if(!domain.empty()) queries.push_back({domain, ts_sec, ts_usec});
    }

    f.close();
    return queries;
}

int main() {
    string pcapFile = "3.pcap";
    vector<DNSQuery> queries = parsePcapForDNS(pcapFile);
    if(queries.empty()) { cerr << "No DNS queries found.\n"; return 1; }

    // Create UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0) { perror("socket failed"); return 1; }

    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERVER_PORT);
    inet_pton(AF_INET, SERVER_IP, &servaddr.sin_addr);

    ofstream table("result_table.txt");
    table << "CustomHeader\tDomain\tResolvedIP\n";

    char buffer[BUF_SIZE];
    socklen_t len = sizeof(servaddr);

    // Send each DNS query with custom header to the server
    for(size_t i = 0; i < queries.size(); i++) {
        string header = buildCustomHeader(i);
        string msg = header + queries[i].domain;

        // Send to server
        sendto(sockfd, msg.c_str(), msg.size(), 0, (struct sockaddr*)&servaddr, sizeof(servaddr));

        // Receive resolved IP from server
        int n = recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr*)&servaddr, &len);
        buffer[n] = '\0';
        string resolvedIP(buffer);

        // Print and save to table
        cout << header << " | " << queries[i].domain << " -> " << resolvedIP << endl;
        table << header << "\t" << queries[i].domain << "\t" << resolvedIP << "\n";
    }

    table.close();
    close(sockfd);
    return 0;
}
