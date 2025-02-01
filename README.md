# Computer Networks Assignment 1

# Part 1
`packet_sniffer.cpp` contains the packet sniffer.

Run the file as follows:
```
sudo g++ -o p packet_sniffer.cpp -lpcap
sudo ./p
```

To stop capturing the packets, click `Ctrl + c`

This program stores captured packets in `captured_packets.pcap`

## q1
Solution in `p1q1.cpp`

Run the file as follows:
```
sudo g++ -o p p1q1.cpp -lpcap
./p
```

Code for generating the histogram is in `hist.py`

## q2, q3
Solutions in `p1q2.cpp` and `p1q3.cpp` respectively.

For both these questiuns, run the file as follows:
```
sudo g++ -o p p1q2.cpp -lpcap
./p output.pcap
```

# Part 2
## q1
Solution in `p2q1.cpp`

Run the file as follows:
```
sudo g++ -o p p2q1.cpp -lpcap
./p
```

## q2, q3,q4
The solutions are in the corresponding cpp files. Run as instructed in Part 2 Q1 above.

# Part 3
## q1
The captured pcap file for q1 is `q3.pcap` 
