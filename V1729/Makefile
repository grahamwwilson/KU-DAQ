# This Makefile started out life as the Makefile in the xx_usb distribution - and was a bit of a mess.
# So have tidied it up.

# Specify verbose version to understand what is going on

CXX   = g++ -v -g
CXXC  = g++ -c -g                      # -c = compile only    -g is a debugging flag compatible with GDB
RM    = rm -f

CXXLIBDIRS	= -L./../../libs/
CXXLIBS		= -lxx_usb
INCLUDEDIRS 	= -I./../../include/
CXXFLAGS      	= -O -Wall -g -fPIC $(INCLUDEDIRS)                # fPIC is position-independent-code suitable for dynamic linking

PROGRAM = V1729_test
TEST = Test_GetBits
DUMP = Dump_Data
OBJS = getbits.o V1729_initialize.o V1729_config.o

$(PROGRAM):   $(PROGRAM).cpp $(OBJS)
	$(CXX) $(CXXFLAGS) \
	$(CXXLIBS) $(CXXLIBDIRS) -o $(PROGRAM) V1729_test.cpp $(OBJS)
	@echo "Done"

test:   $(TEST).cpp getbits.o
	$(CXX) -o $(TEST) $(TEST).cpp getbits.o

dump:   $(DUMP).cpp getbits.o
	$(CXX) -o $(DUMP) $(DUMP).cpp getbits.o

objects: $(OBJS)
	$(CXXC) -o $@ $(OBJS)

clean:
	$(RM) $(OBJS)
	$(RM) $(PROGRAM)
