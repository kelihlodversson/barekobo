import enum
import struct
from  render.starfield import Starfield
from pygame import Rect

class Opcode(enum.Enum) :
    SetViewOffset      = 0
    DrawBackground     = 1
    DrawSprite         = 2
    SetPlayerPositions = 3
    PlotMap            = 4
    FrameStart         = 255

def decode_vectorS12(a,b,c):
    return (a | ((b & 0xf0) <<4), (c << 4) | (b &0xf))

class CommandBuffer:

    def __init__(self, screen, sprites):
        self.screen = screen
        self.sprites = sprites
        self.offset = (0,0)
        self.starfield = Starfield(screen, sprites.get_palette(), 2048, 2048)

        self.buffer = b''
        self.commands = {
            Opcode.SetViewOffset      : (self.set_view_offset, '<3B'),
            Opcode.DrawBackground     : (self.draw_background, ''),
            Opcode.DrawSprite         : (self.draw_sprite,     '<3BB'),
            Opcode.SetPlayerPositions : (self.set_positions,   '<3B3B'),
            Opcode.PlotMap            : (self.plot_map,        '<3B'),
            Opcode.FrameStart         : (self.frame_start,     '<i')
        }

    def frame_start(self, frame_size) :
        pass

    def set_view_offset(self, x, mid, y) :
        self.offset = decode_vectorS12(x,mid,y)

    def draw_background(self) :
        self.starfield.draw(self.offset)

    def draw_sprite(self, a, b, c, image) :
        sprite = self.sprites[image >> 4][image & 0xF]
        x, y = decode_vectorS12(a, b, c)
        self.screen.blit(sprite, ((x - self.offset[0]) & 2047, (y - self.offset[1]) & 2047))

    def set_positions(self, a0,b0,c0, a1,b1,c1) :
        pass

    def plot_map(self, a,b,c) :
        print("plot map (%d, %d)" % decode_vectorS12(a, b, c))

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
