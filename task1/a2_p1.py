#!/usr/bin/env python3

import time
import argparse
from mininet.topo import Topo
from mininet.net import Mininet
from mininet.log import setLogLevel
from mininet.node import Controller

PCAP_FILE_CLIENT = "a2_p1_client_scalable.pcap"
PCAP_FILE_SERVER = "a2_p1_server_scalable.pcap"

class MyTopo(Topo):
    def build(self):
        # Add switches
        s1 = self.addSwitch('s1')
        s2 = self.addSwitch('s2')
        s3 = self.addSwitch('s3')
        s4 = self.addSwitch('s4')
        
        # Add hosts (h7 will act as the iperf3 server)
        h1 = self.addHost('h1')
        h2 = self.addHost('h2')
        h3 = self.addHost('h3')
        h4 = self.addHost('h4')
        h5 = self.addHost('h5')
        h6 = self.addHost('h6')
        h7 = self.addHost('h7')
        
        # Connect hosts to switches
        self.addLink(s1, h1)
        self.addLink(s1, h2)
        self.addLink(s2, h3)
        self.addLink(s3, h4)
        self.addLink(s3, h5)
        self.addLink(s4, h6)
        self.addLink(s4, h7)
        
        # Interconnect switches to form the topology
        self.addLink(s1, s2)
        self.addLink(s2, s3)
        self.addLink(s3, s4)
        

def parse_args():
    parser = argparse.ArgumentParser(
        description='Mininet topology with iperf3 experiment for TCP congestion control analysis')
    parser.add_argument('--option', type=str, default='a',
                        help='Experiment option to run (e.g., "a")')
    parser.add_argument('--ccs', type=str, default='scalable',
                        help='TCP congestion control scheme (e.g., bbr, cubic, bbr)')
    return parser.parse_args()

def runExperiment(net, ccs):
    # Retrieve the iperf3 client and server hosts
    h1 = net.get('h1')
    h7 = net.get('h7')
    h1.cmd(f"tcpdump -i h1-eth0 -w {PCAP_FILE_CLIENT} &")
    h7.cmd(f"tcpdump -i h7-eth0 -w {PCAP_FILE_SERVER} &")
    time.sleep(2)
    
    print("\n*** Starting iperf3 server on h7")
    # Start iperf3 server on h7 in the background.
    h7.cmd("iperf3 -s &")
    time.sleep(2)  # Give the server time to start

    # CLI(net) # here
    # Open a new terminal with tcpdump at h1-eth0 interface


    # Construct and run the iperf3 client command on h1.
    client_cmd = f"iperf3 -c {h7.IP()} -p 5201 -b 10M -P 10 -t 10 -C {ccs} -V"
    print("\n*** Running iperf3 client on h1 with command:")
    print(client_cmd)
    result = h1.cmd(client_cmd)
    time.sleep(5)

    # Stop capturing packets on h2 and h7
    h1.cmd("pkill tcpdump")
    h7.cmd("pkill tcpdump")
    
    
    print("\n*** iperf3 Test Output:\n")
    print(result)
    
    # Kill the iperf3 server process on h7
    h7.cmd("killall -9 iperf3")

def runTopology(option, ccs):
    topo = MyTopo()
    net = Mininet(topo=topo, controller=Controller)
    net.start()
    print("Network Started...")
    # Run experiment for option 'a'
    if option.lower() == 'a':
        print("\n*** Running Experiment Option A: iperf3 test for TCP congestion control analysis")
        runExperiment(net, ccs)
    
    net.stop()

if __name__ == '__main__':
    setLogLevel('info')
    args = parse_args()
    runTopology(args.option, args.ccs)
