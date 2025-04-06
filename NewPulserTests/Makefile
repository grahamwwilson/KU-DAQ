# basic makefile

# Specify verbose version to understand what is going on
CXX   = g++ -v
RM		= rm -f

CXXLIBDIRS	= -L/usr/lib/
CXXLIBS		= -lxx_usb -lusb-1.0
INCLUDEDIRS 	= -I/usr/include/libusb-1.0/ -I./../VmeModule_Includes -I./../usr/include

CXXFLAGS      	= -O -Wall -fPIC -g $(INCLUDEDIRS)

OBJ = NewPulser.exe

main: 
	$(CXX) $(CXXFLAGS) \
	-o $(OBJ) NewPulser.cpp $(CXXLIBS) $(CXXLIBDIRS)
	@echo "Done"

clean:
	$(RM) $(OBJ)
