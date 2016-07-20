import socket
from time import sleep

socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socket.bind(("", 9999))
socket.listen(1)

def send_message(connection, message):
    # This function prefixes the message length
    print("Sending message: " + message)
    message = str(len(message.encode())).zfill(4) + message
    connection.send(message.encode())

while True:
    print("Waiting for connection")
    connection, address = socket.accept()
    connection.setblocking(0)
    while True:
        try:
            message = connection.recv(int(connection.recv(4).decode())).decode()
            print( "Recieved message reads: " + message)
            send_message( connection, message)
        except Exception:
            pass
