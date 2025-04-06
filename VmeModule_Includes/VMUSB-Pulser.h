// attempt to build small pulser object that can be used as an object with other vms calling objects
// 10/2007, David File

/* This version is used to set up two pulsers, one potentially 
 * as a reference clock for the scaler and the second as 
 * a prototype for gate generation.
 * This way we can avoid using the gate generator module (V462).
 * 
 *                       Graham W. Wilson      Feb 15th 2007
 */
 
// Clean this up a bit including better documentation.
// Add statusA, statusB, and version methods
//                       Graham W. Wilson      JUN-21-2024 

#ifndef USB_PULSER
#define USB_PULSER

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include <string>
//#include "libxxusb.h"
#include "VMUSB-Card.h"
#include "strtools.h"

#define DGG_A_OFFSET 0x14
#define DGG_B_OFFSET 0x18
#define DGG_EXT_OFFSET 0x38
#define DGG_EXT_TOSET 0x00010000
#define TRIGGER_PULSER 6

namespace VMUSB {

class Pulser 
{ 
 private:
  libusb_device_handle *udev;
  std::string mode;        // Frequency ("f") or Period configuration (currently non-"f")
  long ddg;
  long freqA;              // Frequency (Hz)
  long nanoA;              // Period (ns)
  long gatewidth;          // Gate width (ns)
  long delay;
  long nanoB;
  std::string pmode;
  int channel, nimbit, trigger, gate, invert, latch;
  int setf();
  int sett();
 public:
  Pulser(libusb_device_handle*, int, long, long, std::string);
  Pulser(libusb_device_handle*);
  ~Pulser();
  int reset1t(long, long);
  int reset1f(long, long);
  int reset2t(long, long);
  int reset2f(long, long);
  int disable(int);
  std::string status();
  std::string statusA();
  std::string statusB();
  std::string version();
  std::string params();
};

Pulser::Pulser(libusb_device_handle* udev){
  }

Pulser::Pulser(libusb_device_handle* udev, int nim, long param1, long param2, std::string mode){
    // cout << "Snoozing for 1s after establishing communication " << endl;
    this->udev=udev;
    this->channel=nim;
    this->nimbit=nim-1;
    this->pmode=mode;

    // GetRegisters();
    if (nim == 1)
      {
      if (mode.compare(0,1,"f") == 0) reset1f(param1, param2);            
      else reset1t(param1, param2);
      }
    else  // nim == 2 default
      {
      if (mode.compare(0,1,"f") == 0) reset2f(param1, param2);
      else reset2t(param1, param2);
      }
    //if nim > 2 error
  }

Pulser::~Pulser(){
    // Turn off the Pulser using some command
    disable(1);
    disable(2);
  }

std::string Pulser::status(){
  long dgg;
  std::string statstr; // = "status: ";
  VME_register_read(udev,DGG_A_OFFSET,&dgg);
  statstr = "status: " + LongToStr(dgg) + "  ";
  VME_register_read(udev,DGG_B_OFFSET,&dgg);
  statstr = statstr + LongToStr(dgg) + "  ";
  VME_register_read(udev,DGG_EXT_OFFSET,&dgg);
  statstr = statstr + LongToStr(dgg);
  return (statstr);
}

std::string Pulser::params(){
  std::string sparams;
  if (this->pmode == "f") {
    sparams = "FREQ mode. freq (Hz) = " + LongToStr(this->freqA) + " gate (ns) = " +  LongToStr(this->gatewidth);
    }
  else
    {
    sparams = "PERIOD mode. period (12.5 ns ticks) = " + LongToStr(this->nanoA) + " gate (ns) = " + LongToStr(this->gatewidth) ;
    }
  return(sparams);
 }
 
std::string Pulser::version(){
// Return version string
    std::string s;
    s = "VMUSB-Pulser version 3.00";
    return s;
}
 
std::string Pulser::statusA(){
//
// Read registers and decode information related to first channel
//
    long dgga;
    long dggext;
    std::stringstream ss;
    std::string s; 
    VME_register_read(udev,DGG_A_OFFSET,&dgga);
    VME_register_read(udev,DGG_EXT_OFFSET,&dggext);    
    
    unsigned short dgg_delayFine;
    unsigned short dgg_gate;
    unsigned short dgg_delayCoarse;
    
// Now coral the correct bits into these words 
    dgg_delayFine   = (dgga & 0x0000FFFF);    // the low word
//    dgg_gate        = (dgga & 0xFFFF0000);    // the high word
    dgg_gate        =  (dgga >> 16);      // the high word    
    dgg_delayCoarse = (dggext & 0x0000FFFF);  // the low word
      
    ss << "STATUS A:: Gate = " << dgg_gate << " Delay Coarse = " << dgg_delayCoarse << " " << " Delay Fine = " << dgg_delayFine;
    s = ss.str();
    
    return s;
}

std::string Pulser::statusB(){
//
// Read registers and decode information related to second channel
//
    long dggb;
    long dggext;
    std::stringstream ss;
    std::string s; 
    VME_register_read(udev,DGG_B_OFFSET,&dggb);
    VME_register_read(udev,DGG_EXT_OFFSET,&dggext);    
    
    unsigned short dgg_delayFine;
    unsigned short dgg_gate;
    unsigned short dgg_delayCoarse;
    
// Now coral the correct bits into these words 
    dgg_delayFine   = (dggb & 0x0000FFFF);    // the low word
    dgg_gate        = (dggb >> 16);    // the high word
    dgg_delayCoarse = (dggext >> 16 );  // the high word
      
    ss << "STATUS B:: Gate = " << dgg_gate << " Delay Coarse = " << dgg_delayCoarse << " " << " Delay Fine = " << dgg_delayFine;
    s = ss.str();
    
    return s;
}

int Pulser::setf() {
  //
  // Frequency based configuration
  // Input: frequency (Hz), gatewidth (ns).
  //
  // This function takes the frequency (in Hz) and the gate width in (ns) to 
  // calculate the rounded integer values for the delay and gate length (in units of 12.5 ns) 
  // The delay is encoded on 32 bits while the gate is encoded with 16 bits.
  // This allows in principle gate lengths up to 819.1875 us, and delays up to 53.686 s.
  //
  int coarse = 0;
  int fine  = 0;
  long siggate = long(round(this->gatewidth / 12.5));
  long delaytime = long((1.0/this->freqA) * 1e9 - siggate*12.5);

  coarse = int(floor(delaytime / 819200.0));
  fine = int(round(((delaytime - coarse*819200.0) / 12.5)));

  // Ok, lets now construct the long word....
  int sigdelay = 0;

  // Ok, plug the coarse into the real_delay, then shift by 16 places (make room for the lower bits)
  sigdelay = coarse;
  sigdelay = sigdelay<<16;

  // cout << " 16-bit shifted value of delay : " << real_delay << endl;
  // Now do a bit-wise OR of real_delay and fine, which should get the
  // final value correct for the input to VME_DGG command

  sigdelay = sigdelay | fine;

  // Need to check the validity of thes settings...  Dont want to burn something up!
  nimbit = this->channel - 1;
  int retcheck=VME_DGG(this->udev, nimbit, TRIGGER_PULSER, nimbit, sigdelay, siggate, 0, 0);
  return (retcheck);
  }

int Pulser::sett(){
  //
  // Period based configuration
  // Input: period (multiples of 12.5 ns), gatewidth (ns)
  //
  // This function takes the period (in multiples of 12.5 ns) and the gate width in (ns) to 
  // calculate the rounded integer values for the delay and gate length (in units of 12.5 ns) 
  // The delay is encoded on 32 bits while the gate is encoded with 16 bits.
  // This allows in principle gate lengths up to 819.1875 us, and delays up to 53.686 s.
  //

  // Rewrite with nanoA as 12.5 ns multiples

  long siggate = long(round(2*gatewidth / 25));
  long delaytime = long(this->nanoA - siggate);
  int coarse = 0;
  int fine  = 0;
  coarse = int(floor(delaytime / 65536));
  fine = int(round((delaytime - coarse*65536)));

  int sigdelay = 0;

  // Ok, plug the coarse into the real_delay, then shift by 16 places (make room for the lower bits)

  sigdelay = coarse;
  sigdelay = sigdelay<<16;

  //cout << " 16-bit shifted value of delay : " << real_delay << endl;

  // Now do a bit-wise OR of real_delay and fine, which should get the
  // final value correct for the input to VME_DGG command

  sigdelay = sigdelay | fine;

  // Need to check the validity of thes settings...  Dont want to burn something up!
  nimbit=channel-1; // no multiplexing of output and channels
  int retcheck=VME_DGG(this->udev, nimbit, TRIGGER_PULSER, nimbit, sigdelay, siggate, 0, 0);
  return(retcheck);
  }

inline int Pulser::reset1t(long nanoA, long nanoB){
  this->nanoA = nanoA;
  this->gatewidth = nanoB;
  this->channel = 1;
  sett();
  return(0);
  }

inline int Pulser::reset1f(long freqA, long nanoB){
   this->freqA = freqA;
   this->gatewidth = nanoB;
   this->channel = 1;
   setf();
   return(0);
  }

inline int Pulser::reset2t(long nanoA, long nanoB){
   this->nanoA = nanoA;
   this->gatewidth = nanoB;
   this->channel = 2;
   sett();
   return(0);
  }

inline int Pulser::reset2f(long freqA, long nanoB){
   this->freqA = freqA;
   this->gatewidth = nanoB;
   this->channel = 2;
   setf();
   return(0);
  }

inline int Pulser::disable(int channel){
  //do the disable thing
  int nimbit=channel-1;
  VME_DGG(this->udev, nimbit, 0, nimbit, 0, 0, 0, 0);
  return(0);
  }

} // end namespace VMUSB
#endif // USB_PULSER
