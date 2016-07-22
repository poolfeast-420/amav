import socket
from errno import *
from multiprocessing import Process, Manager, SimpleQueue

class Server(Process):
    """ Handles receiving and sending """

    def __init__(self, send_queue, recv_queue, port=9999):
        Process.__init__(self)
        self.send_queue = send_queue
        self.recv_queue = recv_queue
        self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.socket.bind(("", port))
        self.socket.listen(1)

    def run(self):
        while True:
            # Main loop for server
            print("Waiting for connection")
            connection, address = self.socket.accept()
            print("Connection established")
            connection.setblocking(0)
            previous_error = None
            while True:
                try:
                    if not self.send_queue.empty():
                        message = self.send_queue.get()
                        message = str(len(message.encode())).zfill(4) + message
                        connection.send(message.encode())
                    size_prefix = connection.recv(4).decode()
                    if size_prefix == '': # Disconnected
                        break
                    if size_prefix: # If message was sent
                        self.recv_queue.put(connection.recv(int(size_prefix)).decode())
                        print("Recieved message")
                except Exception as error:
                    # Remove error messages due to non-blocking
                    if error.errno != EWOULDBLOCK:
                        # Remove duplicate errors
                        if not error.errno == previous_error:
                            previous_error = error.errno
                            print(error)

# Setup process stuff

recv_queue = SimpleQueue()
send_queue = SimpleQueue()

server = Server(send_queue, recv_queue)
server.start()

while True:
    send_queue.put("mayonaise")
    print(recv_queue.get())
