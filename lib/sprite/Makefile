CIRCLEHOME = ../../3rd_party/circle
INCLUDE	+= -I ../
OBJS	= $(patsubst %.cpp,%.o,$(wildcard *.cpp))

libsprite.a: $(OBJS)
	rm -f $@
	$(AR) cr $@ $(OBJS)

include $(CIRCLEHOME)/Rules.mk
