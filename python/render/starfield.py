import pygame
import random
class Starfield:

    def __init__(self, screen, palette, width, height) :
        self.screen = screen
        self.size = (width, height)
        self.density = 625
        self.palette = palette

    def draw(self, offset) :
        rstate = random.getstate()
        random.seed(999)
        mask1_x = self.size[0]//2 - 1
        mask1_y = self.size[1]//2 - 1
        mask2_x = self.size[0]//4 - 1
        mask2_y = self.size[1]//4 - 1

        offset1_x = offset[0] // 2
        offset1_y = offset[1] // 2
        offset2_x = offset[0] // 4
        offset2_y = offset[1] // 4

        for _ in range(self.density) :
            x = random.getrandbits(16)
            y = random.getrandbits(16)
            b = (y >> 20) & 5 

            p1 = ((x - offset1_x) & mask1_x, (y - offset1_y) & mask1_y)
            p2 = ((x - offset2_x) & mask2_x, (y - offset2_y) & mask2_y)
            
            self.screen.set_at(p2, self.palette[5+b])
            self.screen.set_at(p1, self.palette[22+b])

        random.setstate(rstate)