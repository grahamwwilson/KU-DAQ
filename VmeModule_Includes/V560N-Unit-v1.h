// V560N object, code that encapsulates functions and returns string formatted comment to any output
// HEP 4/25/2008, David File
// Code freeze: 8/16/07, V560N-Unit-v0.h

// Overall goals:
//  1) OOD allows attachment of usb pointer to object
//  2) allows common command formats for all VME boards
//  3) must not slow operations
//  4) simplified logfile functions
//  5) provide data to calling routine
//  6) most operations can be defined in this class, but only as tools and independent of USB interactions

#ifndef V560N_UNIT
#define V560N_UNIT

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>
//#include <time.h>
#include <sys/time.h>
#include <math.h>
#include <string.h>
//#include <usb.h>
//#include <libxxusb.h>
#include "strtools.h"
#include "VMUSB-Card.h"

// Base address and offsets for CAEN-VME-V560N, 16-channel scaler module
static const unsigned long int V560N_BA = 0x90000000;
static const unsigned short int V560N_AM = 0x09;

static const short int V560N_MODULE_IDE = 0xFE;
static const short int V560N_MODULE_IDC = 0xFC;
static const short int V560N_MODULE_IDA = 0xFA;

static const short int STATUS_SCALES = 0x58;
static const short int INCREASE_SCALES = 0x56;
static const short int CLEAR_VETO = 0x54;
static const short int SET_VETO = 0x52;

static const short int CLEAR_SCALES = 0x50;

static const short int REQ_INTERRUPT = 0x0E;
static const short int CLEAR_INTERRUPT = 0x0c;
static const short int ENABLE_INTERRUPT  = 0x08;
static const short int DISABLE_INTERRUPT = 0x0A;
static const short int LEVEL_INTERRUPT  = 0x06;
static const short int VECTOR_INTERRUPT = 0X04;

static const short int COUNTERS = 16;
static const short int COUNTER[COUNTERS] = {0x10, 0x14, 0x18, 0x1c, 0x20, 0x24, 0x28, 0x2c,  \
					    0x30, 0x34, 0x38, 0x3c, 0x40, 0x44, 0x48, 0x4c};  // V560N channels 0-15
namespace VMUSB {

class V560N 
   {
   private:
    usb_dev_handle *udev;     // Device Handle 
    std::string messageOut, messageOut2, messageOut3, messageOut4, text1;
    char twobytes[17];
    char counterOut[20];
    long Data, Data1, Data2, Data3;
    long int One, Zero;
    //
    int index;
    unsigned long int uldata0;
    long *ldata[COUNTERS];
    short returnVME;
    struct word16 {
    bool bits[16];
    };
    struct timeval tvStart, tv0, tv1;
    struct timezone tvz;
    double gtdif, ratecount, rateerror;

   public:
    V560N(usb_dev_handle*);
    ~V560N();
    // not all VME calls are the same
    // some pass data or references to VME calls
    std::string GetHeader(int);
    std::string GetScalesStatus();
    std::string IncScales();
    std::string SetVeto();
    std::string ClearVeto();
    std::string ClearScales();
    //
    std::string ReadCounterAll();
    std::string ReadwStamp();
    std::string TotalwStamp();
    std::string RateswStamp();
    std::string AllwStamp();
    //
    int Read32Counter(int,long*);
    unsigned long scalerData[COUNTERS];
    unsigned long scalerPast[COUNTERS];
    unsigned long scalerIncr[COUNTERS];
    //
    int ReqInterrupt(long*);
    std::string GetInterrupts();
    std::string ClearInterrupt();
    std::string EnableInterrupt();
    std::string DisableInterrupt();
    std::string LevelInterrupt();
    std::string VectorInterrupt();
//    unsigned long int* MyCounters();
   };

V560N::V560N(usb_dev_handle* udevin) {
    this->udev = udevin;
    gettimeofday(&tvStart,&tvz);  // always record to private
    gettimeofday(&tv0,&tvz);  // always record to private
     for(int i=0; i<COUNTERS; i++){
       this->scalerPast[i] = 0;
       }
    this->Data = 0;
    this->Zero = 0;
    this->One = 1;
    }

V560N::~V560N() {}

std::string V560N::GetHeader(int row){
   // goal is to output the registers in character format of 1's and zero's
   switch(row){
     case 1:
       returnVME = VME_read_16(this->udev, V560N_AM, V560N_BA+V560N_MODULE_IDE, &Data1);
       if(returnVME>0) {
	 messageOut = "header 1: " + DecToBin(Data1) + " ";
         }
       else
         {
          messageOut = "error: header 1 ";
         }
     return(messageOut);
     case 2:
        returnVME = VME_read_16(this->udev, V560N_AM, V560N_BA+V560N_MODULE_IDC, &Data2);
     if(returnVME>0) {
        messageOut = "header 2: " + DecToBin(Data2) + " ";
        }
     else
        {
        messageOut = "error: header 2 ";
        }
     return(messageOut);
     case 3: 
       returnVME = VME_read_16(this->udev, V560N_AM, V560N_BA+V560N_MODULE_IDA, &Data3);
       if(returnVME>0) {
        messageOut = "header 3: " + DecToBin(Data3) + " ";
        }
       else
        {
        messageOut = "error: header 3 "; 
        }
       return(messageOut);
     default:
       return("error: header unknown ");
   }
}

std::string V560N::GetInterrupts(){
   // get the registers and interrupts, read only
       returnVME = VME_read_16(this->udev, V560N_AM, V560N_BA+REQ_INTERRUPT, &Data1);
       if(returnVME>0) {
         messageOut = "request reg: " + DecToBin(Data1) + ", ";
         }
       else
         {
          messageOut = "error: request reg/read ";
         }
       returnVME = VME_read_16(this->udev, V560N_AM, V560N_BA+LEVEL_INTERRUPT, &Data2);
       if(returnVME>0) {
         messageOut = messageOut + "level+veto reg: " + DecToBin(Data2) + ", ";
         }
       else
         {
          messageOut = messageOut + "error: get interrupts ";
         }
       returnVME = VME_read_16(this->udev, V560N_AM, V560N_BA+VECTOR_INTERRUPT, &Data3);
       if(returnVME>0) {
           messageOut = messageOut + "vector reg: " + DecToBin(Data3) + " ";
         }
       else
         {
           messageOut = "error: get interrupts ";
         }
     return(messageOut);
   }

std::string V560N::GetScalesStatus(){
   returnVME = VME_read_16(this->udev, V560N_AM, V560N_BA+STATUS_SCALES, &Data);    
   if(returnVME>0) {
       messageOut = "scale status: " + DecToBin(Data) + " ";
     }
   else
     {
       messageOut = "error: get scale status ";
     }
   // std::cout << messageOut << std::hex << Data << std::endl << std::dec;
   return(messageOut);
   }

std::string V560N::IncScales(){
   returnVME = VME_write_32(udev, V560N_AM, V560N_BA+INCREASE_SCALES, One);  // increment all 
  if(returnVME>0) {
       messageOut = "increment scales "; 
     }
  else
     {
       messageOut = "error: increment scales ";
     }
   return(messageOut);
   }

std::string V560N::SetVeto(){
   returnVME = VME_read_16(udev, V560N_AM, V560N_BA+SET_VETO, &Zero);      // Set the veto 
   if(returnVME>0) {
      messageOut = "set veto ";
      }
   else
      {
      messageOut = "error: set veto";
      }
   return(messageOut);
   }

std::string V560N::ClearVeto(){
    returnVME = VME_read_16(udev, V560N_AM, V560N_BA+CLEAR_VETO, &One);    // Clear the veto
    if(returnVME>0) {
      messageOut = "clear veto ";
      }
    else
      {
	messageOut = "error: clear veto";
      }
    return(messageOut);
    }

std::string V560N::ClearScales(){
     returnVME = VME_read_16(udev, V560N_AM, V560N_BA+CLEAR_SCALES, &One);  // Reset all the channels
     if(returnVME>0) {
        messageOut = "clear scales ";
      }
     else
      {
        messageOut = "error: clear scales ";
      }
       return(messageOut);
     }

//unsigned long int* V560N::MyCounters(){
//   return scalerData;
//}

std::string V560N::ReadCounterAll(){
  // simple and basic and quick
     messageOut = "  all: ";
     gettimeofday(&tv0,&tvz);  // always record to private
     for(int i=0; i<COUNTERS; i++){
       this->Read32Counter(i, &Data);
       messageOut = messageOut + LongToStr(Data) + ", ";
       this->scalerData[i] = Data; //added scalerData
       }
     return(messageOut);
     }

std::string V560N::ReadwStamp(){
     // time stamp in messageout
     messageOut = "  all: ";
     for(int i=0; i<COUNTERS; i++){
       this->Read32Counter(i, &Data);
       messageOut = messageOut + LongToStr(Data) + ", ";
       this->scalerData[i] = Data; //added scalerData
       }
     messageOut = messageOut + LongToStr(gettimeofday(&tv0,&tvz));
     return(messageOut);
     }

std::string V560N::RateswStamp(){
  // can't use in combination with other reads because each read resets the interval
  // uses data preexisting in buffer
     gettimeofday(&tv1,&tvz);
     gtdif = (double)(tv1.tv_sec - tv0.tv_sec);
     messageOut = " Time:" + DblToStr((double)(tv1.tv_sec - tvStart.tv_sec)) + "sec" + "\n";
     messageOut = messageOut + "  all: ";
     messageOut2 = "rates: ";
     messageOut3 = "error: ";
     unsigned long int ratecount=0;
     for(int i=0; i<COUNTERS; i++){
       this->Read32Counter(i, &Data);
       this->scalerData[i] = Data;         //added scalerData
       ratecount = (Data - this->scalerPast[i]);
       rateerror = sqrt(ratecount)/gtdif;  // Error on rate assuming Poisson statistics
       this->scalerPast[i] = Data;
       if (i == 8) {
	 messageOut = messageOut + "\n       ";
	 messageOut2 = messageOut2 + "\n       ";
	 messageOut3 = messageOut3 + "\n       ";
       }
       messageOut = messageOut + LongToStr(Data) + ", ";
       messageOut2 = messageOut2 + DblToStr(ratecount/gtdif) + ", ";
       messageOut3 = messageOut3 + DblToStr(rateerror) + ", ";
       }
     messageOut = messageOut + ";     counts ";
     messageOut2 = messageOut2 + "; " + DblToStr((double)gtdif) + " sec";
     messageOut3 = messageOut3 + ";      +- hz ";
     messageOut = messageOut + "\n" + messageOut2 + "\n" + messageOut3;

     gettimeofday(&tv0,&tvz);

     return(messageOut);
     }

std::string V560N::TotalwStamp(){
  // can't use in combination with other reads because each read resets the interval
  // uses data preexisting in buffer
     gettimeofday(&tv1,&tvz);
     gtdif = (double)(tv1.tv_sec - tvStart.tv_sec);
     messageOut = " Time:" + DblToStr(gtdif) + " sec" + "\n";
     messageOut = messageOut + "  all: ";
     messageOut2 = "rates: ";
     messageOut3 = "error: ";
     unsigned long int ratecount=0;
     for(int i=0; i<COUNTERS; i++){
       this->Read32Counter(i, &Data);
       this->scalerData[i] = Data;         //added scalerData
       ratecount = Data;
       rateerror = sqrt(ratecount)/gtdif;  // Error on rate assuming Poisson statistics
       if (i == 8) {
	 messageOut = messageOut + "\n       ";
	 messageOut2 = messageOut2 + "\n       ";
	 messageOut3 = messageOut3 + "\n       ";
       }
       messageOut = messageOut + LongToStr(Data) + ", ";
       messageOut2 = messageOut2 + DblToStr(ratecount/gtdif) + ", ";
       messageOut3 = messageOut3 + DblToStr(rateerror) + ", ";
     }
     messageOut = messageOut + "; counts ";
     messageOut2 = messageOut2 + "; hz-bar";
     messageOut3 = messageOut3 + "; +- hz ";
     messageOut = messageOut + "\n" + messageOut2 + "\n" + messageOut3;
     return(messageOut);
}


std::string V560N::AllwStamp(){
  // can't use in combination with other reads because each read resets the interval
  // uses data preexisting in buffer
  // added by GWW
     gettimeofday(&tv1,&tvz);
     gtdif = (double)(tv1.tv_sec - tvStart.tv_sec);
     messageOut = " Time:" + DblToStr(gtdif) + " sec" + "\n";
     messageOut = messageOut + "  all: ";
     messageOut2 = "rates: ";
     messageOut3 = "error: ";
     messageOut4 = "delta: ";
     unsigned long int ratecount=0;
     unsigned long int incr_count=0;
     for(int i=0; i<COUNTERS; i++){
       this->Read32Counter(i, &Data);
       this->scalerData[i] = Data;         // added scalerData
       ratecount = Data;                   // total so far for this one
       rateerror = sqrt(ratecount)/gtdif;  // Error on rate assuming Poisson statistics
       incr_count = ratecount - this->scalerPast[i];
       this->scalerIncr[i] = this->scalerData[i] - this->scalerPast[i];
       this->scalerPast[i] = Data;
       if (i == 8) {
	 messageOut = messageOut + "\n       ";
	 messageOut2 = messageOut2 + "\n       ";
	 messageOut3 = messageOut3 + "\n       ";
	 messageOut4 = messageOut4 + "\n       ";
       }
       messageOut = messageOut + LongToStr(Data) + ", ";
       messageOut2 = messageOut2 + DblToStr(ratecount/gtdif) + ", ";
       messageOut3 = messageOut3 + DblToStr(rateerror) + ", ";
       messageOut4 = messageOut4 + LongToStr(incr_count) + ", ";        
     }
     messageOut = messageOut + "; counts ";
     messageOut2 = messageOut2 + "; hz-bar";
     messageOut3 = messageOut3 + "; +- hz ";
     messageOut4 = messageOut4 + "; counts ";
     messageOut = messageOut + "\n" + messageOut2 + "\n" + messageOut3 + "\n" + messageOut4;
     return(messageOut);
}

int V560N::Read32Counter(int icount, long int* DataOut){
     this->index = icount;
     // Data stays local and is assumed to be of type as defined by function call
     returnVME = VME_read_32(udev, V560N_AM, V560N_BA+COUNTER[this->index], DataOut);
     return(returnVME==0);
     }

int V560N::ReqInterrupt(long int* Data){
     this->Data = *Data;
     returnVME = VME_read_16(udev, V560N_AM, V560N_BA+REQ_INTERRUPT, Data);
     if (returnVME < 0) return 0;
     //messageOut = "setveto            "; 
     //return messageOut;
     return(200);
     }

std::string V560N::ClearInterrupt(){
     returnVME = VME_read_16(udev, V560N_AM, V560N_BA+CLEAR_INTERRUPT, &One);
     if(returnVME>0) {
         messageOut = "Clear Interrupts ";
         }
     else
         {
          messageOut = "error: clear interrupts ";
         }
     return(messageOut);
     }

std::string V560N::EnableInterrupt(){
     returnVME = VME_read_16(udev, V560N_AM, V560N_BA+ENABLE_INTERRUPT, &One);
     if(returnVME>0) {
         messageOut = "Enable VME  Interrupts ";
         }
     else
         {
          messageOut = "error: enable vme interrupts ";
         }
     return(messageOut);
     }

std::string V560N::DisableInterrupt(){
     returnVME = VME_read_16(udev, V560N_AM, V560N_BA+DISABLE_INTERRUPT, &One);
     if(returnVME>0) {
         messageOut = "Disable VME  Interrupts ";
         }
     else
         {
          messageOut = "error: disable vme interrupts ";
         }
     return(messageOut);
     }

std::string V560N::LevelInterrupt(){
     returnVME = VME_read_16(udev, V560N_AM, V560N_BA+LEVEL_INTERRUPT, &Data);
     if(returnVME>0) {
         messageOut = "Veto and Level Regs: " + DecToBin(Data);
         }
     else
         {
          messageOut = "error: veto and level regs ";
         }
     return(messageOut);
     }

std::string V560N::VectorInterrupt(){
     returnVME = VME_read_16(udev, V560N_AM, V560N_BA+VECTOR_INTERRUPT, &Data);
       if(returnVME>0) {
         messageOut = "vector interrupt reg: " + DecToBin(Data);
         }
       else
         {
          messageOut = "error: request vector interrupt ";
         }
     return(messageOut);
     }

} // end of VMUSB namespace
#endif // V560N_UNIT
