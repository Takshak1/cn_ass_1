#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <sys/socket.h>


#define PORT 8080
#define BUF_SIZE 1024

using namespace std;

// Embedded IP pool
const string ipPool[15] = {
    "192.168.1.1","192.168.1.2","192.168.1.3","192.168.1.4","192.168.1.5",
    "192.168.1.6","192.168.1.7","192.168.1.8","192.168.1.9","192.168.1.10",
    "192.168.1.11","192.168.1.12","192.168.1.13","192.168.1.14","192.168.1.15"
};

// Time slot starts
const int morningStart = 0;     // 04:00-11:59 -> first 5 IPs
const int afternoonStart = 5;   // 12:00-19:59 -> next 5 IPs
const int nightStart = 10;      // 20:00-03:59 -> last 5 IPs
const int hashMod = 5;

// Determine time slot start based on hour
int getPoolStart(int hour) {
    if(hour >= 4 && hour <= 11) return morningStart;
    if(hour >= 12 && hour <= 19) return afternoonStart;
    return nightStart;
}

// Resolve IP based on custom header "HHMMSSID"
string resolveIP(const string &header) {
    if(header.size() != 8) return "0.0.0.0";
    int hour = stoi(header.substr(0,2));
    int id   = stoi(header.substr(6,2));
    int start = getPoolStart(hour);
    int idx = start + (id % hashMod);
    return ipPool[idx];
}

int main() {
    int sockfd;
    struct sockaddr_in servaddr, cliaddr;
    char buffer[BUF_SIZE];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if(sockfd < 0) { perror("socket creation failed"); return 1; }

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if(bind(sockfd, (const struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        perror("bind failed"); return 1;
    }

    cout << "Server listening on port " << PORT << endl;
    socklen_t len = sizeof(cliaddr);

    while(true) {
        int n = recvfrom(sockfd, buffer, BUF_SIZE, 0, (struct sockaddr*)&cliaddr, &len);
        buffer[n] = '\0';

        string msg(buffer);
        if(msg.size() < 8) continue;

        string header = msg.substr(0,8);
        string domain = msg.substr(8);

        string resolvedIP = resolveIP(header);

        sendto(sockfd, resolvedIP.c_str(), resolvedIP.size(), 0, (struct sockaddr*)&cliaddr, len);
        cout << header << " | " << domain << " -> " << resolvedIP << endl;
    }

    close(sockfd);
    return 0;
}
