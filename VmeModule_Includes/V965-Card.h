//V965-Card.h
//Contains constants for the V965 module

#ifndef V965_UNIT
#define V965_UNIT

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <usb.h>
#include "libxxusb.h" 
#include "VMUSB-Card.h"
#include "strtools.h"

// V965 Definitions
const int V965_BA = 0xB0000000;
const int V965_DATA = 0x0000;
const int V965_Bit_Set1 = 0x1006;
const int V965_Bit_Set2 = 0x1032;
const int V965_Bit_Clear2 = 0x1034;
const int V965_AM = 0x9;
const int V965_BLT_AM = 0x0B;
const int V965_Status_Register1 = 0x100E;
const int V965_Control_Register1 = 0x1010;
const int V965_Event_Count_Reset = 0x1040;
const int V965_Iped_Register = 0x1060;
const int V965_REVISION = 0x1000;

// Sets up the channel access array.  High channels are in place 2n (n=0,1,2).  Low channels are in 2n+1
const int V965_Threshold_Mem[] = {0x1080, 0x1082, 0x1084, 0x1086, 0x1088, 0x108A, 0x108C, 0x108E,
                                  0x1090, 0x1092, 0x1094, 0x1096, 0x1098, 0x109A, 0x109C, 0x109E,
                                  0x10A0, 0x10A2, 0x10A4, 0x10A6, 0x10A8, 0x10AA, 0x10AC, 0x10AE,
                                  0x10B0, 0x10B2, 0x10B4, 0x10B6, 0x10B8, 0x10BA, 0x10BC, 0x10BE};   // 4.40 p54 
// Threshold Memory definitions
const int V965_Killbit_On = 0x100;
const int V965_Killbit_Off = 0x000;

// Status Register 1 definitions
const int V965_Data_Ready = 0x0;

// Bit Set 2 and Bit Clear 2 Register
const int V965_Clear_Data = 0x4;  // 4.26 pg 47
const int V965_Over_Range_En = 0x8;

// Control Register 1
const int V965_Berr_Enable = 0x20;
const int V965_Blk_End = 0x4;

//Reading the type of the Data word
const int Header_Data = 0x2;  // 4.5 pg 34 
const int End_of_Block = 0x4;

//Interrupter capability, 4.2

//Data transfer capability, 4.3

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
class V965 
 {
 private:
  usb_dev_handle *udev;     // Device Handle 
  std::string messageOut, text1;
  char twobytes[17];
  long Data, Data1;
  long int One, Zero;
  long *pDataBuffer;
  int nChannels;
  int bltWords, mxbltBytes, mxevents;
  //
  int index;
  unsigned long int uldata0;
  short returnVME;
 public:
  V965(usb_dev_handle*, int);
  ~V965();
  // not all VME calls are the same
  // some pass data or references to VME calls
  std::string GetRevision();
  int SetEvents(int);
  int GetStatus();
  int GetData32(long *pDataBuffer);
  std::string GetInterrupts();
  std::string ClearInterrupt();
  int GetWords();
  int GetBytes();
};

V965::V965(usb_dev_handle* udev, int nChannels) {
     this->udev = udev;
     // if nChannels > MXCHANNELS ...   
     this->nChannels = nChannels;
     this->bltWords = (nChannels * 2)+ 2;
     this->mxbltBytes = bltWords * 4;
     int i;

  // int V965_initialize(usb_dev_handle *udev)
  // GWW, give up on trying to pass the ofstream from the main function
  // declare a separate file here

     ofstream log965 ("V965_initialize.out");

     // read bit set register 1
     VME_read_16(udev, V965_AM, V965_BA+V965_Bit_Set1, &Data);
     log965 << "V965 Bit Set 1 Register Read : 0x" << hex << Data << endl;

     //Reading the Bit Set 2 Register 
     VME_read_16(udev, V965_AM, V965_BA+V965_Bit_Set2, &Data) ;
     log965 << "V965 Bit Set 2 Register Read : 0x" << hex << Data << endl;    

     //Enable Overflow Range Enable, OVER RANGE ENABLE of BS2 to 1
     VME_write_16(udev, V965_AM, V965_BA+V965_Bit_Set2, V965_Over_Range_En) ;
     VME_read_16(udev, V965_AM, V965_BA+V965_Bit_Set2, &Data) ;
     log965 << "V965 Bit Set 2 Register Read : 0x" << hex << Data << endl << endl;

     // Reading the Threshold Memory for all channels
     for (i=0; i<32; i++)
       {
	 log965 << "Threshold Memory location : " << i ;
	 VME_read_16(udev, V965_AM, V965_BA+V965_Threshold_Mem[i], &Data);
	 log965 << " Value in location : 0x" << Data << endl;
       }
    
         log965 << " Setting the Killbit of the Memory Threshold " << endl << endl;

     // Setting all Killbits in the Threshold Memory to 1
     for (i=0; i<32; i++)
       {
  	 log965 << "Threshold Memory location : " << i;
	 VME_write_16(udev, V965_AM, V965_BA+V965_Threshold_Mem[i],V965_Killbit_On);
	 VME_read_16(udev, V965_AM, V965_BA+V965_Threshold_Mem[i], &Data);
	 log965 << " Value in location : 0x" << hex << Data << endl;
       }

       log965 << endl << "Setting the Threshold Memory Killbits to 0" << endl;
     // Setting all Killbits  in the Threshold Memory to 0 for the requested channels
     int j=0;
     while (j < 2*(this->nChannels)) // High and low range starting at channel 0
       {
	 VME_write_16(udev, V965_AM, V965_BA+V965_Threshold_Mem[j],V965_Killbit_Off);
	 VME_read_16(udev, V965_AM, V965_BA+V965_Threshold_Mem[j], &Data);  
	 // Logfile verification of Threshold Memory buffer
	 log965 << "Verify Threshold Memory " << endl;
	 j++;
       }
     for (i=0; i<32; i++)
       {
         log965 << "Threshold Memory location : " << i;
	 VME_read_16(udev, V965_AM, V965_BA+V965_Threshold_Mem[i], &Data);
         log965 << " Value in location : 0x" << hex << Data << endl;
       }

     // Setting Control Register 1 to BLKEND and BERR ENABLE to 1
     VME_write_16(udev, V965_AM, V965_BA+V965_Control_Register1, 0x00);
     VME_write_16(udev, V965_AM, V965_BA+V965_Control_Register1, V965_Blk_End);
     VME_read_16(udev, V965_AM, V965_BA+V965_Control_Register1, &Data);
     log965 << endl << "Control Register 1 : 0x" << hex << Data << endl;
     // Check Iped register value
     VME_read_16(udev, V965_AM, V965_BA+V965_Iped_Register, &Data);
     log965 << endl << "Iped Register read value: 0x" << hex << Data << endl;

     // Resetting and releasing the output buffer
     VME_write_16(udev, V965_AM, V965_BA+V965_Bit_Set2, V965_Clear_Data);
     VME_write_16(udev,V965_AM,  V965_BA+V965_Bit_Clear2, V965_Clear_Data);  
}

V965::~V965(){
}

int V965::SetEvents(int num){
   mxevents = num;
   return(0);
}

std::string V965::GetRevision(){
  // goal is to output the registers in character format of 1's and zero's
    returnVME = VME_read_16(this->udev, V965_AM, V965_BA+V965_REVISION, &Data1);
    if(returnVME>0) {
        messageOut = "header 1: " + DecToBin(Data1);
      }
    else
      {
	messageOut = "error: revision ";
      }
    return(messageOut);
}

int V965::GetStatus(){
  returnVME = VME_read_16(this->udev, V965_AM, V965_BA+V965_Status_Register1, &Data1);
  if(returnVME>0) {
    // messageOut = "Status_Reg1: " + DecToBin(Data1) + " ";
      messageOut = "Status_Reg1: ";
    }
  else
    {
      messageOut = "error: status reg1 ";
    }
  return(Data1);
}

int V965::GetData32(long* pDataBuffer){
   returnVME = VME_BLT_read_32(this->udev, V965_BLT_AM, this->bltWords, V965_BA+V965_DATA, pDataBuffer);
   return(returnVME);
}

int V965::GetWords(){
   return(this->bltWords);
}

int V965::GetBytes(){
   return(this->mxbltBytes);
}

} // end VMUSB namespace
#endif // V965_UNIT
