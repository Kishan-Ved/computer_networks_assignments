import socket

def main():
    # Create a TCP socket
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    # Bind to all interfaces on port 8080
    server.bind(('0.0.0.0', 8080))
    # Listen with a backlog of 10 as specified
    server.listen(2)
    print("Server listening on port 8080")
    
    # Accept connections indefinitely
    while True:
        conn, addr = server.accept()
        print(f"Accepted connection from {addr}")
        # Receive up to 1024 bytes of data
        data = conn.recv(1024)
        print(f"Received data: {data}")
        conn.close()

if __name__ == "__main__":
    main()