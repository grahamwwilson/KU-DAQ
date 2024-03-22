// attempt to build small pulser object that can be used as an object with other vms calling objects
// 10/2007, David File

/* This version is used to set up two pulsers, one potentially 
 * as a reference clock for the scaler and the second as 
 * a prototype for gate generation.
 * This way we can avoid using the gate generator module (V462).
 * 
 *                       Graham W. Wilson      Feb 15th 2007
 */

#ifndef USB_PULSER
#define USB_PULSER

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <string>
#include "libxxusb.h"
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
  usb_dev_handle *udev;
  std::string type;
  long ddg;
  long freqA;
  long nanoA, gatewidth, delay;
  long nanoB;
  std::string pmode;
  int channel, nimbit, trigger, gate, invert, latch;
  int setf();
  int sett();
 public:
  Pulser(usb_dev_handle*, int, double, long, std::string);
  Pulser(usb_dev_handle*);
  ~Pulser();
  int reset1t(long, long);
  int reset1f(long, long);
  int reset2t(long, long);
  int reset2f(long, long);
  int disable(int);
  std::string status();
  std::string params();
};

Pulser::Pulser(usb_dev_handle* udev){
  }

Pulser::Pulser(usb_dev_handle* udev, int nim, double param1, long param2, std::string type){
    // cout << "Snoozing for 1s after establishing communication " << endl;
    this->udev=udev;
    this->channel=nim;
    this->nimbit=nim-1;
    this->pmode=type;

    // GetRegisters();
    if (nim == 1)
      {
      if (type.compare(0,1,"f") == 0) reset1f(long(param1), param2);            
      else reset1t(long(param1), param2);
      }
    else  // nim == 2 default
      {
      if (type.compare(0,1,"f") == 0) reset2f(long(param1), param2);
      else reset2t(long(param1), param2);
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
  int pout;
  if (this->pmode == "f") {
    sparams = "mode: freq (cyc/sec): " + LongToStr(this->freqA) + " gate: (nsec) " +  LongToStr(this->gatewidth);
    }
  else
    {
    pout = this->delay + this->gatewidth;
    sparams = "mode: length: " + LongToStr(pout) + "gate: " + LongToStr(this->gatewidth);
    }
  return(sparams);
 }

int Pulser::setf() {
  // This function takes the delay_time (in ns) and splits it into fine and coarse components
  // and then constructs the correct long word to pass to the VME_DGG function
  // reference: set_general(usb_dev_handle *udev, double gate_width, double freq, int output, int channel)
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
  // uses time based inputs

  // set_general(usb_dev_handle *udev, double gate_width, double freq, int output, int channel)
  // This function takes the delaytime (in ns) and splits it into fine and coarse components
  // Then constructs the correct long word to pass to the VME_DGG function
  // Now get the gate_time count;
  long siggate = long(round(gatewidth / 12.5));
  long delaytime = long(this->nanoA - siggate*12.5);
  int coarse = 0;
  int fine  = 0;
  coarse = int(floor(delaytime / 819200.0));
  // double delayfine = (delaytime - coarse*819200.0) / 12.5;
  // cout << "delayfine (double) = " << delayfine << endl;

  fine = int(round(((delaytime - coarse*819200.0) / 12.5)));

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
