import socket
from errno import *
from multiprocessing import Process, Manager, SimpleQueue

class Server(Process):
    """ Handles receiving and sending over network """

    def __init__(self, send_queue, recv_queue, port=9999):
        Process.__init__(self)
        self.send_queue = send_queue
        self.recv_queue = recv_queue
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.bind(("", port))
        self.socket.listen(1)

    def run(self):
        while True: # Main loop for server
            print("Waiting for connection")
            connection, address = self.socket.accept() # blocks until connection starts
            print("Connection established")
            connection.setblocking(0)
            while True:
                try:
                    if not self.send_queue.empty(): # If their is something to send
                        message = self.send_queue.get()
                        message = str(len(message.encode())).zfill(4) + message # Prefix four digit byte amount
                        connection.send(message.encode())
                        print("Sent message")
                    size_prefix = connection.recv(4).decode() # Recieve four bytes
                    if size_prefix == '': # If disconnected
                        break
                    if size_prefix: # If message was recieved
                        self.recv_queue.put(connection.recv(int(size_prefix)).decode())
                        print("Recieved message")
                except Exception as error:
                    if error.errno != EWOULDBLOCK: # Remove error messages due to non-blocking
                        print(error)


# Setup process stuff

recv_queue = SimpleQueue()
send_queue = SimpleQueue()

server = Server(send_queue, recv_queue)
server.start()

while True:
    send_queue.put("mayonaise")
    print(recv_queue.get())
