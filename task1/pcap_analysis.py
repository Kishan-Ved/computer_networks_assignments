################# Throughput Analysis

# from scapy.all import rdpcap

# def calculate_total_bytes(pcap_file, ip_address):
#     packets = rdpcap(pcap_file)  # Read the pcap file
#     total_bytes = sum(len(pkt) for pkt in packets 
#                       if pkt.haslayer("IP") and pkt.haslayer("TCP") and 
#                       (pkt["IP"].src == ip_address))
    
#     return total_bytes

# # Path to your PCAP file
# pcap_file = "a2_p1_client_as_given.pcap"
# ip_address = "10.0.0.1"

# # Compute total bytes
# total_bytes = calculate_total_bytes(pcap_file, ip_address)
# print(f"Throughput: Total bytes for TCP packets where source IP is {ip_address}: {total_bytes} bytes")

#################### Goodput Analysis

from scapy.all import rdpcap, TCP, IP

def calculate_goodput(pcap_file, src_ip):
    packets = rdpcap(pcap_file)  # Read the pcap file
    goodput_bytes = 0
    seen_sequences = set()  # Store (seq_num, payload_size) to track duplicates

    for pkt in packets:
        if pkt.haslayer(IP) and pkt.haslayer(TCP):
            if pkt[IP].src == src_ip or pkt[IP].src == "10.0.0.3" or pkt[IP].src == "10.0.0.4":
                tcp_payload_size = len(pkt[TCP].payload)  # Extract payload size
                seq_num = pkt[TCP].seq  # Sequence number

                if tcp_payload_size > 0 and (seq_num, tcp_payload_size) not in seen_sequences:
                    seen_sequences.add((seq_num, tcp_payload_size))  # Mark seen packets
                    goodput_bytes += tcp_payload_size

    return goodput_bytes

# Path to your PCAP file
pcap_file = "p1_q4_loss5_server_scalable.pcap"
src_ip = "10.0.0.1"  # Client IP

# Compute goodput
goodput_bytes = calculate_goodput(pcap_file, src_ip)
print(f"Total goodput bytes for TCP packets sent from {src_ip} (excluding retransmissions): {goodput_bytes} bytes")


################## Packet loss rate analysis

from scapy.all import rdpcap, TCP, IP

# File paths (replace with actual paths)
sender_pcap_file = "p1_q4_loss5_server_scalable.pcap"

# Define sender and receiver IPs
sender_ip = "10.0.0.1"
receiver_ip = "10.0.0.7"

# Load PCAP files
sender_packets = rdpcap(sender_pcap_file)

# Extract TCP sequence numbers for unique packets
sent_seq_numbers = set()
received_seq_numbers = set()
arr = []

# Process sender packets
for pkt in sender_packets:
    if IP in pkt and TCP in pkt and (pkt[IP].src == sender_ip or pkt[IP].src == "10.0.0.3" or pkt[IP].src == "10.0.0.4"):
        sent_seq_numbers.add(pkt[TCP].seq)
        arr.append(pkt[TCP].seq)

# Compute packet loss
packets_sent = len(arr)
packets_received = len(sent_seq_numbers)
packets_lost = packets_sent - packets_received
packet_loss_rate = (packets_lost / packets_sent) * 100 if packets_sent > 0 else 0

# Display results
print(f"Total Sent: {packets_sent}")
print(f"Distinct packets Sent: {packets_received}")
print(f"Packets Lost: {packets_lost}")
print(f"Packet Loss Rate (Packets lost/Total Packet sent): {packet_loss_rate:.2f}%")
