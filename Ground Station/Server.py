import socket
from errno import *
from multiprocessing import Process, Manager

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
        # Main loop for server
        while True:
            print("Waiting for connection")
            connection, address = self.socket.accept()
            print("Connection established")
            connection.setblocking(0)
            previous_error = None
            while True:
                try:
                    # Recieve four bytes of message first, and interpret them as the size of the remaining message
                    recv_queue.append(connection.recv(int(connection.recv(4).decode())).decode())
                    print("Recieved message")
                    message = send.queue.pop(0)
                    message = str(len(message.encode())).zfill(4) + message
                    connection.send(message.encode())
                except Exception as error:
                    # Remove error messages due to non-blocking
                    if error.errno != EWOULDBLOCK: 
                        # Remove duplicate errors
                        if not error.errno == previous_error: 
                            previous_error = error.errno              
                            print(error)
                    # Disconnect if connection is broken
                    if error.errno == ECONNRESET or error.errno == ECONNABORTED or error.errno == WSAECONNRESET:
                        print("Connection removed")
                        connection.shutdown(0)
                        break

# init server process
manager = Manager()
send_queue = manager.list()
recv_queue = manager.list()
server = Server(send_queue, recv_queue)

server.start()

send_queue.append("mayonaise")
