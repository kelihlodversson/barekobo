
CIRCLEHOME=3rd_party/circle
OBJS	= $(patsubst %.cpp,%.o,$(wildcard *.cpp))
VENDOR_LIBS = $(CIRCLEHOME)/lib/usb/libusb.a \
              $(CIRCLEHOME)/lib/fs/libfs.a \
			  $(CIRCLEHOME)/lib/input/libinput.a \
			  $(CIRCLEHOME)/lib/libcircle.a
LIB_DIRS = $(wildcard lib/*)
OWN_LIBS = $(foreach  D,$(LIB_DIRS),$D/lib$(notdir $D).a)
LIBS	= $(OWN_LIBS) $(VENDOR_LIBS)
INCLUDE	+= -I lib -I .

RASPPI ?= 3
include $(CIRCLEHOME)/Rules.mk

$(VENDOR_LIBS):
	make -C $(dir $@) RASPPI=$(RASPPI)

$(OWN_LIBS):
	make -C $(dir $@) RASPPI=$(RASPPI)  $(notdir $@)

$(OWN_LIBS): $(wildcard lib/*/*.cpp) graphics/sprites.xpm

all: $(TARGET)
	echo "Created $(TARGET)"

libclean:
	@for dir in $(dir $(LIBS)) ;   	\
	do  							\
		make -C $$dir clean;      	\
	done

clean: libclean
