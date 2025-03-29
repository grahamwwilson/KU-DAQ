//VHQ-Card.h
//Contains constants for the VHQ module
//
// Note the libxxusb.cpp source code on heplx4 is at /home/graham/vmusb_daq/src
// The relevant signatures are:
// short   VME_read_16(usb_dev_handle *hdev,short Address_Modifier, long VME_Address, long *Data)
// short   VME_write_16(usb_dev_handle *hdev, short Address_Modifier, long VME_Address, long Data)
// 

#ifndef VHQ_UNIT
#define VHQ_UNIT

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <usb.h>
#include "libxxusb.h" 
#include "VMUSB-Card.h"
#include "VHQ.h"               
#include "strtools.h"
#define Sleep(x) usleep((x)*1000)

// VHQ Definitions
const int VHQ_BA = 0xFFFFDD00;  // See https://groups.nscl.msu.edu/nscl_library/manuals/iseg/vhqx0x_eng.pdf
const int VHQ_AM = 0x29;

using std::ofstream;
using std::cout;
using std::endl;
using std::hex;

extern unsigned getbits(unsigned int x, int p, int n){
  //  Function to get bits  p, p-1...p-n from word x...p is the highest bit 
  unsigned int r;
  r = (x >> (p+1-n)) & ~(~0 << n);
  return (r);
}

namespace VMUSB {
class VHQ 
 {
 private:
     usb_dev_handle *udev;     // Device Handle 
     std::string messageOut;
     long Data;
     int RegAddr;
     int RegValue;
     short returnVME;
 public:
  VHQ(usb_dev_handle*);
  ~VHQ();
  // not all VME calls are the same
  // some pass data or references to VME calls

  int ReadRegister(int RegAddr);  
  void WriteRegister(int RegAddr, int RegValue);
  
};

VHQ::VHQ(usb_dev_handle* udev) {

     this->udev = udev;

     ofstream logVHQ ("VHQ_initialize.out");

}

VHQ::~VHQ(){
}

int VHQ::ReadRegister(int RegAddr){
// Read register using D16 mode
// short   VME_read_16(usb_dev_handle *hdev,short Address_Modifier, long VME_Address, long *Data)
    returnVME = VME_read_16(this->udev, VHQ_AM, VHQ_BA + RegAddr, &Data);

    if(returnVME>0) {
//        messageOut = "Register " + RegAddr + " " + Data + " ";
        messageOut = "Register " + RegAddr;        
    }
    else
    {
      messageOut = "error: for register " + RegAddr;
    }
    cout << messageOut << endl;

    return(Data);
}

void VHQ::WriteRegister(int RegAddr, int RegValue){
// Write D16 register
    returnVME = VME_write_16(this->udev, VHQ_AM, VHQ_BA + RegAddr, RegValue);

    if(returnVME>0) {
        messageOut = "Writing to Register " + RegAddr;
    }
    else{
        messageOut = "error: writing to register " + RegAddr;
    }
    cout << messageOut << endl;
}

} // end VMUSB namespace
#endif // VHQ_UNIT
