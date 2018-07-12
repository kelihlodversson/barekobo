CIRCLEHOME = ../../3rd_party/circle
INCLUDE	+= -I ../
OBJS	+= $(patsubst %.cpp,%.o,$(filter-out %.gen.cpp,$(wildcard *.cpp)))

libutil.a: $(OBJS)
	rm -f $@
	$(AR) cr $@ $(OBJS)

include $(CIRCLEHOME)/Rules.mk
