# BareKobo
Bare-metal multiplayer reimplementation of XKobo for Rasberry PI (2 and 3)

![game screen shot](https://raw.githubusercontent.com/kelihlodversson/barekobo-dissertation/master/5_dissertation/screenshots/network_game_over_both.png)

This is a bare-metal project based on a subset of XKobo, a 2D space shooter
where players need to avoid enemy ships and destroy maze-looking space stations,
extended with a cooperative multiplayer mode for a pair of Raspberry PI
devices running on the same network.

The core game is implemented in C++ on top of the Circle bare metal framework
for Raspberry PI. A functional implementation of the client part is also available 
in a desktop client written in Python using the Pygame framework.

The development is done using a GNU C cross compiler environment compiled
for Ubuntu. It reuses and extends the GNU Makefiles shipped with Circle to build
a kernel image to be loaded onto an SD card along with the Raspberry Pi firmware.
The sources can be built on an Ubuntu Linux distribution as follows:

```bash
# Install cross build tools
sudo apt install gcc-arm-none-eabi
# Build the application
make
# Mount the SD card
sudo mount /dev/<sd card device> /mnt/<path to SD card>/ [other mount options]
# Copy resulting kernel image and config file to an SD Card
cp kernel8-32.img config.txt /mnt/<path to SD card>/
# Eject the SD card
sudo umount /mnt/<path to SD card>/
# .. repeat for a second SD card..
# Insert the SD card into a Raspberry PI and boot it up.
``` 

This project is part of an individual master's degree project. Feel free to read
the [dissertation](https://github.com/kelihlodversson/barekobo-dissertation/blob/master/5_dissertation/hfh3_dissertation.pdf) and take a look at the [viva presentation slides](https://github.com/kelihlodversson/barekobo-dissertation/blob/master/6_presentation/hfh3_presentation.pdf)
