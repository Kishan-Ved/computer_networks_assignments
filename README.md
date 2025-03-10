# Computer Networks Assignment 2

# Report
The report is `12-22110099_22110122.pdf`

# Task 1

All relevant files are in the `./task1` directory. 

`pcap_analysis.py` contains code for finding throughput, goodput and packet loss rate for any pcap. This is used in all the sub questions of this part.

## Subtask a

Code is in `a2_p1.py`. Run using `sudo python3 a2_p1.py --ccs='bbr'`. The congestion control scheme can be set using the `-ccs=''` argument.

## Subtask b

Code is in `a2_p1_q2.py`. Run using `sudo python3 a2_p1_q2.py --ccs='bbr'`. The congestion control scheme can be set using the `-ccs=''` argument.

## Subtask c

### Subquestion 1

Code is in `a2_p1_q3.py`. Run using `sudo python3 a2_p1_q3.py --ccs='bbr'`. The congestion control scheme can be set using the `-ccs=''` argument.

### Subquestion 2

Code is in `a2_p1_q3_b3.py`. Run using `sudo python3 a2_p1_q3_b3.py --ccs='bbr'`. The congestion control scheme can be set using the `-ccs=''` argument.

## Subtask d

Code is in `a2_p1_q4_loss5.py`. Run using `sudo python3 a2_p1_q4_loss5.py --ccs='bbr' --loss=5`. The congestion control scheme can be set using the `-ccs=''` argument. The loss persent on the link between can be set using the `--loss=5` argument.

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
