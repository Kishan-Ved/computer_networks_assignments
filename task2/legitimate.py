import socket
import time
import sys
import threading

def connect_and_send(server_ip):
    # Create a TCP socket
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # Connect to the server
    client.connect((server_ip, 8080))
    # Send 100 bytes of data
    client.send(b"Hello" * 20)  # 5 bytes * 20 = 100 bytes
    client.close()

def main(server_ip):
    # Run indefinitely
    while True:
        # Start a new thread for each connection
        threading.Thread(target=connect_and_send, args=(server_ip,), daemon=True).start()
        time.sleep(1)  # New connection every 1 second

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python legitimate.py <server_ip>")
        sys.exit(1)
    main(sys.argv[1])