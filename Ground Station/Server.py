import socket

socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socket.bind(("", 8081))
socket.listen()

while True:
    connection, address = socket.accept()
    while True:
        size = connection.recv(4).decode()
        print(size)
        message = connection.recv(size).decode()
        send_message(connection, message)
        print(message)

def send_message(connection, message):
    message = str(len(message.encode())).zfill(4) + message
    connection.sendall(message)
