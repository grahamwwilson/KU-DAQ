//V1290N-Card.h
//Contains constants for the V1290N module

#ifndef V1290N_UNIT
#define V1290N_UNIT

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <usb.h>
#include "libxxusb.h" 
#include "VMUSB-Card.h"
#include "strtools.h"

// V1290N Definitions
const int V1290N_BA = 0xC1000000;
const int V1290N_DATA = 0x0000;
const int V1290N_AM = 0x9;
const int V1290N_BLT_AM = 0x0B;

// Register addresses
const int V1290N_Control_Register = 0x1000;            //6.3 p73
const int V1290N_Status_Register = 0x1002;             //6.4 p75
const int V1290N_FirmwareRevision_Register = 0x1026;   //6.21 p81

using std::ofstream;
using std::endl;
using std::hex;

extern unsigned getbits(unsigned int x, int p, int n){
  //  Function to get bits  p, p-1...p-n from word x...p is the highest bit 
  unsigned int r;
  r = (x >> (p+1-n)) & ~(~0 << n);
  return (r);
}

namespace VMUSB {
class V1290N 
 {
 private:
  usb_dev_handle *udev;     // Device Handle 
  std::string messageOut;
  long Data, Data1;
  short returnVME;
 public:
  V1290N(usb_dev_handle*);
  ~V1290N();
  // not all VME calls are the same
  // some pass data or references to VME calls
  std::string GetRevision();
  int GetStatus();
};

V1290N::V1290N(usb_dev_handle* udev) {

     this->udev = udev;

  // int V1290N_initialize(usb_dev_handle *udev)
  // GWW, give up on trying to pass the ofstream from the main function
  // declare a separate file here

     ofstream log1290N ("V1290N_initialize.out");

     // read Control Register
     VME_read_16(udev, V1290N_AM, V1290N_BA + V1290N_Control_Register, &Data);
     log1290N << "V1290N Control Register Read : 0x" << hex << Data << endl;

     // read Status Register
     VME_read_16(udev, V1290N_AM, V1290N_BA + V1290N_Status_Register, &Data);
     log1290N << "V1290N Status Register Read : 0x" << hex << Data << endl;
     
     // read Firmware Revision Register
     VME_read_16(udev, V1290N_AM, V1290N_BA + V1290N_FirmwareRevision_Register, &Data);
     log1290N << "V1290N Firmware Revision Register Read : 0x" << hex << Data << endl;     

}

V1290N::~V1290N(){
}


std::string V1290N::GetRevision(){
  // goal is to output the registers in character format of 1's and zero's
    returnVME = VME_read_16(this->udev, V1290N_AM, V1290N_BA + V1290N_FirmwareRevision_Register, &Data1);
    if(returnVME>0) {
        messageOut = "header 1: " + DecToBin(Data1);
      }
    else
      {
	messageOut = "error: revision ";
      }
    return(messageOut);
}

int V1290N::GetStatus(){
  returnVME = VME_read_16(this->udev, V1290N_AM, V1290N_BA+V1290N_Status_Register, &Data1);
  if(returnVME>0) {
    // messageOut = "Status_Reg1: " + DecToBin(Data1) + " ";
      messageOut = "Status_Reg: ";
    }
  else
    {
      messageOut = "error: status reg1 ";
    }
  return(Data1);
}

} // end VMUSB namespace
#endif // V1290N_UNIT
