import pygame

class ImageSheet:

    def __init__(self, path, imageWidth, imageHeight, transparent=None, groupSize=8):
        self.groups = [[]]
        
        base_image = pygame.image.load(path)
        base_image.set_colorkey(transparent)
        base_image = base_image.convert()

        base_width, base_height = base_image.get_size()

        for y in range(0, base_height, imageHeight) :
            for x in range(0, base_width, imageWidth) :
                self.groups[-1].append(base_image.subsurface((x,y,imageWidth,imageHeight)))
                if(len(self.groups[-1]) == groupSize) :
                    self.groups.append([])

    def __getitem__(self, group):
        return self.groups[group]


