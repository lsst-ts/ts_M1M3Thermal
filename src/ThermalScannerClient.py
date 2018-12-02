import socket
import time

class ThermalScannerClient(object):
    def __init__(self, ip, port):
        self.ip = ip
        self.port = port
        self.client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.client.bind((ip, port))

    def prepareShutdown(self):
        self.client.sendto("0".encode("utf-8"), ("127.0.0.1", self.port))

    def close(self):
        self.client.close()

    def receive(self):
        data = self.client.recv(1028)
        message = data.decode("utf-8")
        tokens = message.split(",")
        # tokens[0] = Scanner Number, Ignoring
        data = tokens[1:]
        return data