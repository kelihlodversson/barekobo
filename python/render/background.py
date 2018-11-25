import pygame
import random
class Background:

    def __init__(self, screen, sprites, width, height) :
        self.screen = screen
        self.pixel_size = (width, height)
        self.size = (width//16, height//16)
        self.sprites = sprites
        self.grid = [[None] * self.size[0] for _ in range(self.size[1])]

    def set_cell(self, x, y, image):
        self.grid[y][x] = image

    def clear_cell(self, x, y):
        self.grid[y][x] = None

    def draw(self, offset) :
        start_x = offset[0] // 16
        start_y = offset[1] // 16

        rows = 480 // 16 + 1
        cols = 512 // 16 + 1

        width, height = self.pixel_size
        for y in range(start_y, start_y+rows):
            row = self.grid[y % self.size[1]]
            for x in range(start_x, start_x + cols):
                cell = row[x % self.size[0]]
                if not cell is None:
                    sprite = self.sprites[cell >> 4][cell & 0xF]
                    screen_x = (x*16 - offset[0]) % width
                    screen_y = (y*16 - offset[1]) % height

                    if width - screen_x < 16:
                        screen_x -= width
                    if height - screen_y < 16:
                        screen_y -= height
                    
                    self.screen.blit(sprite, (screen_x, screen_y))
