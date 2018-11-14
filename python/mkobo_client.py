import render
from  client import Client
from  client.spotter import Spotter
from  client.commandbuffer import CommandBuffer

import pygame
 
from os import path

class MultiKobo:
    def __init__(self):
        self.size = 640, 400
        self.screen = pygame.display.set_mode(self.size, pygame.HWSURFACE | pygame.DOUBLEBUF)
        self.running = self.screen != None
        self.clock = pygame.time.Clock()
        self.target_fps = 60
        self.elapsed = 0.0 
        sprite_path = path.join('..','graphics','sprites.xpm')
        self.sprites = render.imagesheet.ImageSheet(sprite_path, 16, 16, 255, 8)
        self.client = None
        self.subimage = 0
        self.buffer = None
 
    def handle_events(self):
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                self.running = False
            elif event.type == Spotter.SPOTTER_HOST_ADDED:
                print("HOST ADDED   " + event.host)
                if self.client is None:
                    self.client = Client(self.screen, self.sprites, event.host)
            elif event.type == Spotter.SPOTTER_HOST_REMOVED:
                print("HOST REMOVED " + event.host)
            elif event.type == CommandBuffer.COMMAND_BUFFER_PACKET_RECEIVED:
                self.buffer = event.buffer
            elif event.type in (pygame.KEYDOWN, pygame.KEYUP) and self.client:
                self.client.key_event(event.key, event.type == pygame.KEYDOWN)


    def update(self):
        time_step = self.clock.tick(self.target_fps)
        self.elapsed += time_step
        pygame.display.set_caption("FPS: {0:.0f}".format(self.clock.get_fps()))

    def render(self):
        self.screen.fill(0)
        if self.client is None:
            self.screen.blit(self.sprites[6][self.subimage],(10,10))
            self.subimage = (self.subimage + 1) % len(self.sprites[6])
        elif not self.buffer is None:
            self.buffer.run()

        pygame.display.flip() 
 
    def run(self):
        s = Spotter()
        while( self.running ):
            self.handle_events()
            self.update()
            self.render()
        s.stop()
        if not self.client is None:
            self.client.stop()
        pygame.quit()
 
if __name__ == "__main__" :
    mkobo = MultiKobo()
    mkobo.run()
