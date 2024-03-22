//V1290N-CAEN.h
//Contains constants for the V1290N module

#ifndef V1290N_UNIT
#define V1290N_UNIT

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <usb.h>
#include "libxxusb.h" 
#include "VMUSB-Card.h"
#include "V1190.h"  // V1190,V1290 header file from CAEN
#include "strtools.h"
#define Sleep(x) usleep((x)*1000)

// V1290N Definitions
const int V1290N_BA = 0xC1000000;
const int V1290N_DATA = 0x0000;
const int V1290N_AM = 0x9;
const int V1290N_BLT_AM = 0x0B;

using std::ofstream;
using std::endl;
using std::hex;

//unsigned short opcdData[10] = {0};
long opcdData[10] = {0};

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
  long *pDataBuffer;   // pointer to the DataBuffer
  int RegAddr;
  int RegValue;
  short returnVME;
  int nWords;
 public:
  V1290N(usb_dev_handle*);
  ~V1290N();
  // not all VME calls are the same
  // some pass data or references to VME calls
  std::string GetRevision();
  int GetStatus();
  int ReadRegister(int RegAddr);
  int ReadRegisterD32(int RegAddr);  
  void WriteRegister(int RegAddr, int RegValue);
  int WriteOpcodeSlow(int nWords, long* opcdData);
  int WriteOpcode(int nWords, long* opcdData);
  int ReadOneDatumUsingOpcode();  
  int GetDataBLT32(int bltWords, long *pDataBuffer);
};

V1290N::V1290N(usb_dev_handle* udev) {

     this->udev = udev;

  // int V1290N_initialize(usb_dev_handle *udev)
  // GWW, give up on trying to pass the ofstream from the main function
  // declare a separate file here

     ofstream log1290N ("V1290N_initialize.out");

     // read Control Register
     VME_read_16(udev, V1290N_AM, V1290N_BA + CONTROL, &Data);
     log1290N << "V1290N Control Register Read : 0x" << hex << Data << endl;

     // read Status Register
     VME_read_16(udev, V1290N_AM, V1290N_BA + STATUS, &Data);
     log1290N << "V1290N Status Register Read : 0x" << hex << Data << endl;
     
     // read Firmware Revision Register
     VME_read_16(udev, V1290N_AM, V1290N_BA + FW_REVISION, &Data);
     log1290N << "V1290N Firmware Revision Register Read : 0x" << hex << Data << endl;    

}

V1290N::~V1290N(){
}

int V1290N::GetDataBLT32(int bltWords, long* pDataBuffer){
// Perform block transfer read (BLT32) of bltWords (32-bits each) from the data buffer.
// Returns number of bytes read when successful. Otherwise a negative number.
   returnVME = VME_BLT_read_32(this->udev, V1290N_BLT_AM, bltWords, V1290N_BA + V1290N_DATA, pDataBuffer);
   return(returnVME);
}

int V1290N::ReadRegister(int RegAddr){
// Read register using D16 mode
  returnVME = VME_read_16(this->udev, V1290N_AM, V1290N_BA + RegAddr, &Data1);

/*
  if(returnVME>0) {
      messageOut = "Register " + RegAddr + " " + DecToBin(Data1) + " ";
    }
  else
    {
      messageOut = "error: for register " + RegAddr;
    }
*/
  return(Data1);
}

int V1290N::ReadRegisterD32(int RegAddr){
// Read register using D32 mode
  returnVME = VME_read_32(this->udev, V1290N_AM, V1290N_BA + RegAddr, &Data1);

/*
  if(returnVME>0) {
      messageOut = "Register " + RegAddr + " " + DecToBin(Data1) + " ";
    }
  else
    {
      messageOut = "error: for register " + RegAddr;
    }
*/
  return(Data1);
}

void V1290N::WriteRegister(int RegAddr, int RegValue){
// Write D16 register
  returnVME = VME_write_16(this->udev, V1290N_AM, V1290N_BA + RegAddr, RegValue);
/*
  if(returnVME>0) {
      messageOut = "Writing to Register " + RegAddr + " the value " + DecToBin(RegValue) + " ";
    }
  else
    {
      messageOut = "error: writing to register " + RegAddr;
    }
*/    
}

// ---------------------------------------------------------------------------
// Write Opcode
// ---------------------------------------------------------------------------
int V1290N::WriteOpcode(int nw, long* opcdData)
{
	int i;
	int timeout = 0;
	unsigned short hs = 0;

	for (i = 0; i < nw; i++) {
		do {
			hs = ReadRegister(PROG_HS);
			timeout++;
			Sleep(1);
		} while ( ((hs & 0x01) == 0) && (timeout < 3000)); // wait to write 
// Continues looping if the microcontroller handshake register's WRITE OK bit is not set or time has not yet expired

		if ( timeout == 3000 ) {
		// TIMEOUT error
			return 1;
	    }
	    
	    // Now the WRITEOK bit is set and we can write this datum
		WriteRegister(OPCODE, opcdData[i]);
		// Note that after the write the WRITEOK bit is reset to zero - and we need to check it 
		// again before writing another datum.		
	}
	
// Upon successful write cycles wait a while (so that module status and micro-controller handshake register have chance to update)	
	do {
		hs = ReadRegister(PROG_HS);	
		Sleep(1);
	} while ( (hs & 0x03) == 0 ); // wait until read or write OK bit is set (caution may not work for READ operations ??)      
	    
	return 0;
}

int V1290N::ReadOneDatumUsingOpcode()
{
// Read one datum from the relevant OPCODE related register assuming a prior WriteOpcode call 
// has configured the relevant register for reading
	
	unsigned short hs = 0;

    int Datum;

	do {
		hs = ReadRegister(PROG_HS);
		Sleep(1);
	} while ( (hs & 0x02) == 0 ); // wait to read 
// Continues looping if the microcontroller handshake register's READ OK bit is not set
	    
	// Now the READOK bit is set and we can read a datum using the OPCODE register
    Datum = ReadRegister(OPCODE);
    
	// Note that after this one read the READOK bit will likely be reset to zero - and we will need to check it 
    // again before reading another datum.		
		
// Upon successful read cycle wait a while (so that module status and micro-controller handshake register have chance to update)	
	do {
		hs = ReadRegister(PROG_HS);	
		Sleep(1);
	} while ( (hs & 0x03) == 0 ); // wait until read or write OK bit is set (caution may not work for READ operations ??)      
	    
	return Datum;
}



// ---------------------------------------------------------------------------
// Write OpcodeSlow
// ---------------------------------------------------------------------------
int V1290N::WriteOpcodeSlow(int nw, long* opcdData)
{
	int i;
	int timeout = 0;
	unsigned short hs = 0;

	for (i = 0; i < nw; i++) {
		do {
			hs = ReadRegister(PROG_HS);
			timeout++;
			Sleep(1);
		} while ( ((hs & 0x01) == 0) && (timeout < 3000)); // wait to write 
// Continues looping if the microcontroller handshake register's WRITE OK bit is not set or time has not yet expired

		if ( timeout == 3000 ) {
		// TIMEOUT error
			return 1;
	    }
	    
	    // Now the WRITEOK bit is set and we can write this datum
		WriteRegister(OPCODE, opcdData[i]);
		// Note that after the write the WRITEOK bit is reset to zero - and we need to check it 
		// again before writing another datum.
		
	}
	
// Upon successful completion wait a while (so that module status and micro-controller handshake register have chance to update)	
	int sleeptime = 100;
	int sleeptot = 0;
	do {
	    Sleep(1);
	    sleeptot +=1;
	}   while( sleeptot < sleeptime );
	    
	return 0;
}

std::string V1290N::GetRevision(){
  // goal is to output the registers in character format of 1's and zero's
    returnVME = VME_read_16(this->udev, V1290N_AM, V1290N_BA + FW_REVISION, &Data1);
    if(returnVME>0) {
        messageOut = "GetRevision: " + DecToBin(Data1);
      }
    else
      {
	messageOut = "error: GetRevision";
      }
    return(messageOut);
}

int V1290N::GetStatus(){
  returnVME = VME_read_16(this->udev, V1290N_AM, V1290N_BA + STATUS, &Data1);
  if(returnVME>0) {
      messageOut = "Status_Reg: " + DecToBin(Data1) + " ";
    }
  else
    {
      messageOut = "error: status reg ";
    }
  return(Data1);
}



} // end VMUSB namespace
#endif // V1290N_UNIT
