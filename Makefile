
CIRCLEHOME=3rd_party/circle
# Default RaspberryPI model to build for
RASPPI ?= 3
# Directories containing source files for this project
DIRS = . game render input util graphics network

# Additional CIRCLE features to include (the sched subsytem is required by the net subsystem)
CIRCLE_FEATURES = usb fs input net sched

OBJS = $(patsubst %.cpp,%.o, $(patsubst ./%,%, $(foreach  D,$(DIRS),$(filter-out %.gen.cpp,$(wildcard $D/*.cpp)))))
LIBS = $(foreach  L,$(CIRCLE_FEATURES),$(CIRCLEHOME)/lib/$L/lib$L.a) \
		$(CIRCLEHOME)/lib/libcircle.a
INCLUDE	+= -I .

OBJS += graphics/sprite_data.gen.o
DEP = $(OBJS:%.o=%.d)

# Include build rules from the circle project.
# contains the correct definitions for cross compilers and compiler options.
include $(CIRCLEHOME)/Rules.mk

$(LIBS):
	@$(MAKE) -C $(dir $@) RASPPI=$(RASPPI) OPTIMIZE="$(OPTIMIZE) -DHFH3_PATCH"

CPPFLAGS += -MMD -DHFH3_PATCH
EXTRACLEAN += $(OBJS) $(DEP) graphics/sprite_data.gen.cpp

# sprite_data.gen.cpp is generated from a xpm file in the graphics directory
graphics/sprite_data.gen.cpp: graphics/sprites.xpm
	perl graphics/xpm2c.pl $< sprites > $@

# Helper target to copy the generated image to a remote tftp server.
# It assumes the images should be stored in a directory called /tftpboot/multikobo
# on a server called "pi" (either host name or alias in .ssh/config.)
deploy: $(TARGET).img config.txt
	rsync --progress -v $(TARGET).img config.txt pi:/tftpboot/multikobo

libclean:
	@for dir in $(dir $(LIBS)) ;   	\
	do  							\
		make -C $$dir clean;      	\
	done

-include $(DEP)

.PHONY: clean deploy libclean
