import enum
import struct
from  render.starfield import Starfield

class Opcode(enum.Enum) :
    SetViewOffset  = 0
    DrawBackground = 1
    DrawSprite     = 2

from pygame import event, USEREVENT

class CommandBuffer:

    COMMAND_BUFFER_PACKET_RECEIVED = USEREVENT + 3

    def __init__(self, screen, sprites):
        self.screen = screen
        self.sprites = sprites
        self.offset = (0,0)
        self.starfield = Starfield(screen, 4096, 4096)

        self.buffer = b''
        self.commands = {
            Opcode.SetViewOffset  : (self.set_view_offset, '<hh'),
            Opcode.DrawBackground : (self.draw_background, ''),
            Opcode.DrawSprite     : (self.draw_sprite,     '<hhB')
        }

    def set_view_offset(self, x, y) :
        self.offset = (x,y)

    def draw_background(self) :
        self.starfield.draw(self.offset)

    def draw_sprite(self, x, y, image) :
        sprite = self.sprites[image >> 4][image & 0xF]
        self.screen.blit(sprite, ((x - self.offset[0]) & 4095, (y - self.offset[1]) & 4095))

    def invalid_opcode(self, op) :
        print("Error: invalid opcode", op)
    
    def run(self):
        offset = 0
        while offset < len(self.buffer):
            try:
                args = ()
                op = Opcode(self.buffer[offset])
                offset += 1
                args = struct.unpack_from(self.commands[op][1], self.buffer, offset)
                offset += struct.calcsize(self.commands[op][1])
                self.commands[op][0](*args)
            except ValueError :
                self.invalid_opcode(self.buffer[offset])
                offset += 1
            except struct.error:
                pass
    
    def clear(self):
        self.buffer = b''

    def read(self, socket):
        new_buffer = socket.recv(4096)
        self.buffer = new_buffer
        event.post(event.Event(self.COMMAND_BUFFER_PACKET_RECEIVED, buffer=self))
