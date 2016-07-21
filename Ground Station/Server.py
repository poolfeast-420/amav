import socket
from errno import *

socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socket.bind(("", 9994))
socket.listen(1)

# Prefix the message with its length and send it
def send_message(connection, message):
    print("Sending message: " + message)
    message = str(len(message.encode())).zfill(4) + message
    connection.send(message.encode())

# Main loop for server
while True:
    print("Waiting for connection")
    connection, address = socket.accept()
    print("Connection established")
    connection.setblocking(0)
    previous_error = None
    while True:
        try:
            # Recieve four bytes of message first, and interpret them as the size of the remaining message
            message = connection.recv(int(connection.recv(4).decode())).decode()
            print( "Recieved message reads: " + message)
            send_message( connection, message)
        except Exception as error:
            # Remove error messages due to non-blocking
            if error.errno != EWOULDBLOCK: 
                # Remove duplicate errors
                if not error.errno == previous_error: 
                    previous_error = error.errno              
                    print(error)
            # Disconnect if connection is broken
            if error.errno == WSAECONNRESET:
                print("Connection removed")
                connection.shutdown(0)
                break
