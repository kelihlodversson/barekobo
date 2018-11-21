import enum
import struct
from  render.starfield import Starfield
from pygame import Rect
class Opcode(enum.Enum) :
    SetViewOffset      = 0
    DrawBackground     = 1
    DrawSprite         = 2
    SetPlayerPositions = 3

class CommandBuffer:

    def __init__(self, screen, sprites):
        self.screen = screen
        self.sprites = sprites
        self.offset = (0,0)
        self.starfield = Starfield(screen, sprites.get_palette(), 2048, 4096)

        self.buffer = b''
        self.commands = {
            Opcode.SetViewOffset      : (self.set_view_offset, '<hh'),
            Opcode.DrawBackground     : (self.draw_background, ''),
            Opcode.DrawSprite         : (self.draw_sprite,     '<hhB'),
            Opcode.SetPlayerPositions : (self.set_positions,   '<hhhh')
        }

    def set_view_offset(self, x, y) :
        self.offset = (x,y)

    def draw_background(self) :
        self.starfield.draw(self.offset)

    def draw_sprite(self, x, y, image) :
        sprite = self.sprites[image >> 4][image & 0xF]
        self.screen.blit(sprite, ((x - self.offset[0]) & 2047, (y - self.offset[1]) & 4095))

    def set_positions(self, p0x, p0y, p1x, p1y) :
        pass

    def invalid_opcode(self, op) :
        print("Error: invalid opcode", op)
    
    def run(self):
        offset = 0
        self.screen.set_clip(Rect(0,10,512,512))
        self.screen.fill(0)
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
        self.screen.set_clip(None)
    
    def clear(self):
        self.buffer = b''

    def read(self, socket):
        new_buffer = socket.recv(4096)
        self.buffer = new_buffer
