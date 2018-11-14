from socket import socket, AF_INET, SOCK_DGRAM
from threading import Thread
import time

from pygame import event, USEREVENT

class Spotter :
    """ 
    Python version of the C++ class of the same name
    Locates a MultiKobo server by listening for broadcast UDP packets sent to port
    12345.
    """

    SPOTTER_HOST_ADDED   = USEREVENT + 1
    SPOTTER_HOST_REMOVED = USEREVENT + 2

    def __init__(self, port=12345):
        self.socket = socket(AF_INET, SOCK_DGRAM)
        self.socket.bind(('',port))
        self.socket.setblocking(False)
        self.hosts = {}
        self.running = True
        self.task = Thread(target=self.listener_main)
        self.task.start()

    def stop(self) :
        self.running = False

    def get_hosts(self) :
        return self.hosts.keys()

    def listener_main(self):
        while self.running:
            try:
                m = self.socket.recvfrom(4)
                available = int(m[0][0]) > 0
                address = m[1][0]
                if available :
                    if not address in self.hosts:
                        event.post(event.Event(self.SPOTTER_HOST_ADDED, host=address))
                    self.hosts[address] = time.time()
                elif address in self.hosts:
                    del self.hosts[address]
                    event.post(event.Event(self.SPOTTER_HOST_REMOVED, host=address))
            except BlockingIOError:
                pass
            time.sleep(.5)

            now = time.time()
            for k,v in list(self.hosts.items()):
                if now - v > 4:
                    del self.hosts[k]
                    event.post(event.Event(self.SPOTTER_HOST_REMOVED, host=k))

