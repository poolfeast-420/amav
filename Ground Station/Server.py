import socket

socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socket.bind(("", 8080))
socket.listen()

while True:
    connection, address = socket.accept()
    while True:
        message = ""
        while message is None or not '\n' in message:
            message += connection.recv(1).decode() # It's probably dumb to recieve one byte at a time
        connection.send(message.encode())
