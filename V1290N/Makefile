# basic makefile  

.LIBPATTERNS ="'lib%.so lib%.a' "

# Specify verbose version to understand what is going on
CXX		= g++ -v -g
CXXC	= g++ -c -g
RM		= rm -f
CC		= gcc-3.3
LD		= gcc-3.3
LDFLAGS		= -O
SOFLAGS		= -shared

CXXLIBDIRS	= -L./../../libs/
CXXLIBS		= -lxx_usb
INCLUDEDIRS 	= -I./../../include/ -I./../VmeModule_Includes/

CXXFLAGS      	= -O -Wall -fPIC -g $(INCLUDEDIRS)

CFLAGS		+= $(INCLUDEDIRS) $(SOFLAG)
CFLAGS		+= $(CXXLIBDIRS) $(CXXLIBS)
LDFLAGS		= $(CXXFLAGS)

OBJS = V1290N_configure.o

PROGRAM = V1290N-Read

$(PROGRAM): $(PROGRAM).cpp
	$(CXX) $(CXXFLAGS) \
	$(CXXLIBS) $(CXXLIBDIRS) -o $(PROGRAM) V1290N-Read.cpp
	@echo "Done"


