from threading import Thread
from client.commandbuffer import CommandBuffer

import socket 
import struct
import time
import pygame

KEYS_UP    = set([pygame.K_w, pygame.K_UP,    pygame.K_KP7, pygame.K_KP8, pygame.K_KP9])
KEYS_DOWN  = set([pygame.K_s, pygame.K_DOWN,  pygame.K_KP1, pygame.K_KP2, pygame.K_KP3])
KEYS_LEFT  = set([pygame.K_a, pygame.K_LEFT,  pygame.K_KP1, pygame.K_KP4, pygame.K_KP7])
KEYS_RIGHT = set([pygame.K_d, pygame.K_RIGHT, pygame.K_KP3, pygame.K_KP6, pygame.K_KP9])
KEYS_FIRE  = set([pygame.K_LCTRL, pygame.K_RCTRL, pygame.K_SPACE, pygame.K_LSHIFT, pygame.K_RSHIFT, pygame.K_LALT, pygame.K_RALT])

class Client:
    def __init__(self, screen, sprites, host, port=12345):
        self.command_buffer = CommandBuffer(screen, sprites)
        self.socket = socket.create_connection((host, port))
        self.socket.setblocking(False)
        self.direction = 0
        self.fire  = False
        self.up    = False
        self.down  = False
        self.left  = False
        self.right = False
        
        # Send greeting to server
        self.socket.send(b'Hi!')

        self.socket.setblocking(True)
        # Wait for greeting from server
        greeting = self.socket.recv(3)
        self.socket.setblocking(False)
        
        print("Connected to", host, ":", greeting)

        self.running = True
        self.task = Thread(target=self.reader_main)
        self.task.start()

    def stop(self) :
        self.running = False

    def reader_main(self):
        while self.running:
            try:
                self.command_buffer.read(self.socket)
            except BlockingIOError:
                pass
            time.sleep(1.0/60.0)

    def key_event(self, event):
        key = event.key
        pressed = event.type == pygame.KEYDOWN
        if key in KEYS_UP:
            self.up = pressed
            if pressed and self.down:
                self.down = False
        if key in KEYS_DOWN:
            self.down = pressed
            if pressed and self.up:
                self.up = False
        if key in KEYS_LEFT:
            self.left = pressed
            if pressed and self.right:
                self.right = False
        if key in KEYS_RIGHT:
            self.right = pressed
            if pressed and self.left:
                self.left = False
        if key in KEYS_FIRE:
            self.fire = pressed


        self.send_input_state()
    
    def controller_event(self, event):
        # TODO: This code is written with assumptions that may not match all controllers:
        #   It assumes that axis 0 is the x axis and axis 1 is the y axis and that they
        #   go from a negative to a positive value.
        #   It also assumes that the fire button is button 1 and ignores all other buttons
        if event.type == pygame.JOYAXISMOTION:
            negative = True if event.value < -0.1 else False
            positive = True if event.value >  0.1 else False
            if event.axis == 0:
                self.left, self.right = negative, positive
            else:
                self.up,   self.down  = negative, positive
        elif event.type in (pygame.JOYBUTTONUP, pygame.JOYBUTTONDOWN) and event.button == 1:
            self.fire = event.type == pygame.JOYBUTTONDOWN

        self.send_input_state()

    def render(self):
        self.command_buffer.run()

    def get_direction(self):
        if self.up:
            if self.right:
                return 1
            elif self.left:
                return 7
            else:
                return 0
        elif self.down:
            if self.right:
                return 3
            elif self.left:
                return 5
            else:
                return 4
        elif self.right:
            return 2
        elif self.left:
            return 6
        else:
            return 8

    def send_input_state(self):
        state = self.get_direction() << 4
        state |= 1 if self.fire else 0
        buffer = struct.pack('B', state)
        self.socket.send(buffer)
