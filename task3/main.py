import os
import sys
import time
from mininet.cli import CLI
from mininet.net import Mininet
from mininet.log import setLogLevel
from mininet.topo import SingleSwitchTopo

def main(config_no=0):
    print(config_no)
    # Set Mininet logging level
    setLogLevel('info')
    # Create a topology with 2 hosts and 1 switch
    topo = SingleSwitchTopo(k=2)
    net = Mininet(topo)
    net.start()
    
    # Get host objects
    h1, h2 = net.get('h1', 'h2')

    # Start server on h2
    tcdump_proc = h2.popen(f'tcpdump -i h2-eth0 -w config{config_no}.pcap')

    time.sleep(5)

    # Server on h2
    if config_no == 0:
        server_proc = h2.popen('./server 0 0')
    elif config_no == 1:
        server_proc = h2.popen('./server 0 1')
    elif config_no == 2:
        server_proc = h2.popen('./server 1 0')
    else:
        server_proc = h2.popen('./server 1 1')

    # Client on h1
    if config_no == 0:
        client_proc = h1.popen('./client 0 0')
    elif config_no == 1:
        client_proc = h1.popen('./client 0 1')
    elif config_no == 2:
        client_proc = h1.popen('./client 1 0')
    else:
        client_proc = h1.popen('./client 1 1')

    time.sleep(5)

    # Stop Client
    client_proc.terminate()

    # Stop Server
    server_proc.terminate()

    time.sleep(5)

    net.stop()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 main.py <config_no>")
        sys.exit(1)
    print(sys.argv)
    main(int(sys.argv[1]))