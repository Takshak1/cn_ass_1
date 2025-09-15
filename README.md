# cn_ass_1
Member: Prateek Takshak(23110256)


# Requirements

* **GCC (GNU Compiler Collection)**
* **libpcap development package**
  *(Ubuntu/Debian: `sudo apt-get install libpcap-dev`)*

---

# Compilation

Build both the client and server using the provided **Makefile**:

```bash
make
```

This generates two executables:

* `client.out`
* `server.out`

---

# Usage

### 1. Start the Server

Run the server first (listens on **UDP port 8080** by default):

```bash
./server.out
```

Expected output:

```
Server listening on the port
```

---

### 2. Run the Client

The client reads DNS packets from the provided `3.pcap` file, appends a custom header, and sends them to the server:

```bash
./client.out
```

---

# Example Output

When both are running, the server prints processed packet details:

```
17103600 | Domain: netflix.com  | Resolved IP: 192.168.1.6
17103601 | Domain: linkedin.com | Resolved IP: 192.168.1.7
```

---

# Field Explanations

* **Custom header** → `[HH][MM][SS][ID]`
* **Domain** → extracted from DNS query
* **Resolved IP** → selected from a static pool, based on time slot and query ID

---

# Notes

* input file for the client as per the formula given: **`3.pcap`**


* Server resolution is simulated:

  * IPs are chosen from a static pool (`192.168.1.1` → `192.168.1.15`)
  * Selection depends on **time slot** (Morning, Afternoon, Evening, Night).



