import socket

socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
socket.bind(("", 8080))
socket.listen()

def send_message(connection, message):
    # This function prefixes the message length
    print("Sending message: " + message)
    message = str(len(message.encode())).zfill(4) + message
    connection.send(message.encode())

while True:
    connection, address = socket.accept()
    while True:
        size = int(connection.recv(4).decode())
        message = connection.recv(size).decode()
        print("Recieved message reads: " + message)
        send_message(connection, message)
