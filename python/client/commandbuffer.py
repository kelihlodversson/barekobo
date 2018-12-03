import enum
import struct
from  render.starfield import Starfield
from  render.background import Background
from pygame import Rect

class Opcode(enum.Enum) :
    SetViewOffset       = 0
    DrawBackground      = 1
    DrawSprite          = 2
    SetPlayerPositions  = 3
    SetBackgroundCell   = 4
    ClearBackgroundCell = 5
    SetPlayerStat       = 6
    SetMessage          = 7
    FrameStart          = 255

def decode_vectorS12(a,b,c):
    return (a | ((b & 0xf0) <<4), (c << 4) | (b &0xf))

class CommandBuffer:

    def __init__(self, screen, sprites):
        self.screen = screen
        self.sprites = sprites
        self.offset = (0,0)
        self.size = (2048, 2048)
        self.starfield = Starfield(screen, sprites.get_palette(), *self.size)
        self.background = Background(screen, sprites, *self.size)

        self.buffer = b''
        self.commands = {
            Opcode.SetViewOffset       : (self.set_view_offset,  '<3B'),
            Opcode.DrawBackground      : (self.draw_background,  ''),
            Opcode.DrawSprite          : (self.draw_sprite,      '<3BB'),
            Opcode.SetPlayerPositions  : (self.set_positions,    '<3B3B'),
            Opcode.SetBackgroundCell   : (self.set_background,   '<3B'),
            Opcode.ClearBackgroundCell : (self.clear_background, '<2B'),
            Opcode.SetPlayerStat       : (self.set_player_stat,  '<Bi'),
            Opcode.SetMessage          : (self.set_message,      '<Bhh'),
            Opcode.FrameStart          : (self.frame_start,      '<i')
        }

    def frame_start(self, frame_size) :
        pass

    def set_view_offset(self, x, mid, y) :
        self.offset = decode_vectorS12(x,mid,y)

    def draw_background(self) :
        self.starfield.draw(self.offset)
        self.background.draw(self.offset)

    def draw_sprite(self, a, b, c, image) :
        sprite = self.sprites[image >> 4][image & 0xF]
        x, y = decode_vectorS12(a, b, c)
        width, height = self.size
        screen_x = (x - self.offset[0]) % width
        screen_y = (y - self.offset[1]) % height

        if width - screen_x < 16:
            screen_x -= width
        if height - screen_y < 16:
            screen_y -= height
 
        self.screen.blit(sprite, (screen_x, screen_y))

    def set_positions(self, a0,b0,c0, a1,b1,c1) :
        pass

    def set_background(self, x,y, image) :
        self.background.set_cell(x,y,image)

    def clear_background(self, x,y) :
        self.background.clear_cell(x,y)

    def set_player_stat(self, player_and_stat, value) :
        pass
    
    def set_message(self, message, level, timeout) :
        pass

    def invalid_opcode(self, op) :
        print("Error: invalid opcode", op)
    
    def run(self):
        offset = 0
        self.screen.set_clip(Rect(0,10,512,470))
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
        new_buffer = socket.recv(40960)
        self.buffer = new_buffer
