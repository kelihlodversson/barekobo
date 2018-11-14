import pygame
import random

class Starfield:

    def __init__(self, screen, width, height) :
        self.screen = screen
        self.size = (width, height)
        self.density = 2500
        

    def draw(self, offset) :
        rstate = random.getstate()
        random.seed(900)
        mask1_x = self.size[0]//2 - 1
        mask1_y = self.size[1]//2 - 1
        mask2_x = self.size[0]//4 - 1
        mask2_y = self.size[1]//4 - 1

        offset1_x = offset[0] // 2
        offset1_y = offset[1] // 2
        offset2_x = offset[0] // 4
        offset2_y = offset[1] // 4

        for _ in range(self.density) :
            bits = random.getrandbits(32)
            x = bits >> 16
            y = bits & 0xffff
            b = (y >> 20 & 5) * 10

            p1 = ((x - offset1_x) & mask1_x, (y - offset1_y) & mask1_y )
            p2 = ((x - offset2_x) & mask2_x, (y - offset2_y) & mask2_y )
            pygame.draw.line(self.screen, (100+b,100+b,100+b), p1, p1, 1)
            pygame.draw.line(self.screen, (50+b,50+b,50+b), p2, p2, 1)


        random.setstate(rstate)