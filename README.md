# cn_ass_1
Member: Prateek Takshak(23110256)

Requirements

GCC (GNU Compiler Collection)

libpcap development package
(on Ubuntu/Debian: sudo apt-get install libpcap-dev)

Compilation

Build both client and server using the provided Makefile:

make


This produces two executables:

client.out

server.out

Usage
1. Start the server

Run the server first. It listens on UDP port 8080 by default.

./server.out


You should see:

Server running... waiting for packets

2. Run the client

The client reads DNS packets from the provided 3.pcap file, appends the custom header, and sends them to the server.

./client.out

Example Output

When running together, the server prints the processed packet details:

Custom header: 17103600 | Domain: netflix.com  | Resolved IP: 192.168.1.6
Custom header: 17103601 | Domain: linkedin.com | Resolved IP: 192.168.1.7


Custom header → [HH][MM][SS][ID]

Domain → extracted from DNS query

Resolved IP → chosen from IP pool based on time slot and query ID

Notes

The client uses 3.pcap as the input file. Replace this file with your own DNS capture if needed.

Server resolution is simulated: IPs are selected from a static pool (192.168.1.1 → 192.168.1.15) based on predefined rules (Morning, Afternoon, Evening, Night).

For simplicity, the server currently prints results instead of sending real DNS replies.
