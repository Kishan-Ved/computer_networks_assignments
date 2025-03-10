# Computer Networks Assignment 2

# Report
The report is `12-22110099_22110122.pdf`

# Task 1

All relevant files are in the `./task1` directory. 

# Task 2

All relevant files are in the `./task2` directory. 
To emulate syn-flood attack on Mininet, run the following command:
```
sudo python3 main.py <filename> <mitigate_bool>
```
where `filename` is the name of the output pcap file, and `mitigate_bool` is a boolean value (True or False) to indicate whether to mitigate the attack or not. When set to True, mitigation is applied. When set to False, attack is performed.

The captured traffic is stored as `capture_mitigated.pcap` and `capture_syn_flood.pcap` for mitigation attempt and syn-food attack, respectively. Analysis is provided in `analysis.ipnb`

# Task 3

All relevant files are in the `./task3` directory. `configs.txt` contains the information about the different configurations of Nagle's algorithm and Delayed ACK.

To test a configuration, run the following command:
```
sudo python3 main.py <config_no>
``` 

where `config_no` is mentioned against the configuration in `configs.txt`.

The captured traffic for different configurations is stored as `config_<config_no>.pcap` Analysis is provided in `analysis.ipnb`
