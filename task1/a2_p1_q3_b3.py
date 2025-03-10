#!/usr/bin/env python3

import time
import argparse
from mininet.topo import Topo
from mininet.net import Mininet
from mininet.log import setLogLevel
from mininet.node import Controller

PCAP_FILE_SERVER = "p1_q3_b3_server_scalable.pcap"

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
        self.addLink(s1, s2, bw=100)
        self.addLink(s2, s3, bw=50)
        self.addLink(s3, s4, bw=100)


def parse_args():
    parser = argparse.ArgumentParser(
        description='Mininet topology with iperf3 experiment for TCP congestion control analysis')
    parser.add_argument('--option', type=str, default='a',
                        help='Experiment option to run (e.g., "a")')
    parser.add_argument('--ccs', type=str, default='scalable',
                        help='TCP congestion control scheme (e.g., scalable, cubic, scalable)')
    return parser.parse_args()

def runExperiment(net, ccs):
    h1, h3, h4, h7 = net.get('h1', 'h3', 'h4', 'h7')
    
    print("\n*** Starting tcpdump on h1 and h7")
    h7.cmd(f"tcpdump -i any -w {PCAP_FILE_SERVER} &")
    time.sleep(2)
    
    print("\n*** Starting iperf3 server on h7")
    h7.cmd("iperf3 -s -p 5201 &")
    h7.cmd("iperf3 -s -p 5202 &")
    h7.cmd("iperf3 -s -p 5203 &")
    time.sleep(6)  # Give the server time to start

    # Client commands
    client_cmd_h1 = f"iperf3 -c {h7.IP()} -p 5201 -b 10M -P 10 -t 15 -C {ccs} -V &"
    client_cmd_h3 = f"iperf3 -c {h7.IP()} -p 5202 -b 10M -P 10 -t 15 -C {ccs} -V &"
    client_cmd_h4 = f"iperf3 -c {h7.IP()} -p 5203 -b 10M -P 10 -t 15 -C {ccs} -V &"
    
    print("\n*** Running iperf3 client on H1")
    h1.cmd(client_cmd_h1)
    
    print("\n*** Running iperf3 client on H3")
    h3.cmd(client_cmd_h3)
    
    print("\n*** Running iperf3 client on H4")
    h4.cmd(client_cmd_h4)

    time.sleep(20)
    
    print("\n*** Stopping tcpdump on h1 and h7")

    h7.cmd("pkill tcpdump")
    
    print("\n*** Stopping iperf3 server")
    h7.cmd("killall -9 iperf3")

def runTopology(option, ccs):
    topo = MyTopo()
    net = Mininet(topo=topo, controller=Controller)
    net.start()
    print("Network Started...")
    
    if option.lower() == 'a':
        print("\n*** Running Experiment Option A: iperf3 test for TCP congestion control analysis")
        runExperiment(net, ccs)
    
    net.stop()

if __name__ == '__main__':
    setLogLevel('info')
    args = parse_args()
    runTopology(args.option, args.ccs)
