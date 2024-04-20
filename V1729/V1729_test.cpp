//  Program to intialize the V1729 and read data 

//  Let's redo this using BLTs.
//  Good idea to increase the trigger rate a bit.

#include <libxxusb.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "V1729_Addresses.h"
#include "V1729_Parameters.h"
#include "V1729_initialize.h"
#include "V1729_config.h"
#include "getbits.h"
using namespace std;

int main ()
{
  const int NUM_EVENTS = 50000;

  struct timeval tim;
  double t1;

  gettimeofday(&tim, NULL);
  t1 = tim.tv_sec + (tim.tv_usec/1000000.0);
  printf("Start program: %.6lf s\n",t1);

  sleep(10);

  int initialize_count = 1;

  // For each event calculate the number of (32-bit) words that should be in the event record (including the event number and the TRIG_REC word)

  const int NCOLS  = 128;                             // Assume for now that we read all columns (NB OF COLS TO READ = 128)  (may need to fix later)
  const int NROWS  =  20;
  const int NHEADER = int((3*V1729_nChannels+1)/2);   // Number of 32-bit data reads needed for the First-Sample/Vernier/Reset-Baseline words for all channels 
  const int NWORDS = 2 + NHEADER + (NCOLS*NROWS*V1729_nChannels)/2 ;    // NCOLS * NROWS  = at most 2560 sub-cells per channel
  cout << "nChannels = " << V1729_nChannels << " NWORDS = " << NWORDS << endl;

  long DataBuffer[NWORDS];

// file with the raw data and some decoding
  ofstream  datafile;
  datafile.open("RawData.out");

  xxusb_device_type devices[100]; 
  struct usb_device *dev;
  usb_dev_handle *udev;       // Device Handle 

  printf ("\nRunning the V1729 reading program \n");

  //Find XX_USB devices and open the first one found
  xxusb_devices_find(devices);
  dev = devices[0].usbdev;
  udev = xxusb_device_open(dev);    

  //Check if device is open. If not, end program
  if(!udev)
  {
      printf("\nNo XX-USB devices found!  Oops ... \n");
      return 0;
  }
  else{

      printf("\nCommunication with first XX-USB device established !\n\n");

  }

  // Check the board settings
  V1729_initialize(initialize_count, udev);
  initialize_count++;

  // Configure the system
  V1729_config(udev); 
  
  // Verify the Board settings
  V1729_initialize(initialize_count, udev);
  initialize_count++;  
 
  unsigned int interrupt;
  long Interrupt_Data;
  short int returnVME;

  for(unsigned int iev=0; iev<NUM_EVENTS; iev++){
 
  //Start Acquisition
  VME_write_32(udev, V1729_am, V1729_BA+V1729_Start_Acquisition, 1);  

  gettimeofday(&tim, NULL);
  t1 = tim.tv_sec + (tim.tv_usec/1000000.0);
  printf("Acquisition started %.6lf s\n",t1);
  cout <<"Testing for interrupt " << endl;  

  // Wait for interrupt before reading RAM   (follow wait_for_interrupt) in code by Brandon Elman
  interrupt = 0;
  while(interrupt != 0x1){

     Interrupt_Data = 0;
     returnVME = VME_read_32(udev, V1729_am, V1729_BA + V1729_Interrupt, &Interrupt_Data);
     interrupt = getbits(Interrupt_Data,0,1);
//     cout << " Interrupt test " << hex << " " << Interrupt_Data << " " << dec << returnVME << endl;   
     interrupt = Interrupt_Data & 0x1;
  }

  cout << "Interrupt received " << " event number " << iev << endl;
  gettimeofday(&tim, NULL);
  t1 = tim.tv_sec + (tim.tv_usec/1000000.0);
  printf("Interrupt time %.6lf s\n",t1);

  long Trig_Rec_Data;
  returnVME = VME_read_32(udev, V1729_am, V1729_BA + V1729_Trig_Rec, &Trig_Rec_Data);
  cout << showbase << internal << setfill('0');  

  cout << "TRIG REC " << hex << setw(8) << Trig_Rec_Data << " (" << dec << returnVME << ") " << dec << getbits(Trig_Rec_Data,7,8) << endl;   

  DataBuffer[0] = iev;               // Set first element of the event record to the event number
  DataBuffer[1] = Trig_Rec_Data;     // Set second element of the event record to the TRIG_REC word
  int nread = 2;

  int ntoread = NHEADER;             // First get the header info from the RAM 
  VME_BLT_read_32(udev, V1729_BLT_am, ntoread, V1729_BA + V1729_RAM_Data_VME, &DataBuffer[nread]);
  nread += ntoread;

  ntoread = 64;                      // Now blocks of 64  (TODO - make this work for more general numbers of columns read with potential remainders - needed for calibration
  while(nread <= NWORDS - ntoread){
     VME_BLT_read_32(udev, V1729_BLT_am, ntoread, V1729_BA + V1729_RAM_Data_VME, &DataBuffer[nread]);
     nread += ntoread;     
  }

  cout << "nread = " << nread << endl;

  gettimeofday(&tim, NULL);
  t1 = tim.tv_sec + (tim.tv_usec/1000000.0);
  printf("Data read from RAM using BLTs %.6lf s\n",t1);

  // Do some formatting for hex numbers
  // (See http://www.cplusplus.com/forum/windows/51591/ )
  datafile << internal << setfill('0');  

  int k=0;
  while (k < NWORDS){
        datafile << setfill('0') << setw(8) << hex << DataBuffer[k] << endl;
        k++;
  }  

  gettimeofday(&tim, NULL);
  t1 = tim.tv_sec + (tim.tv_usec/1000000.0);
  printf("Event data written to file %.6lf s\n",t1);

  }          // End of event loop

  //  Check the board settings at the end
  V1729_initialize(initialize_count, udev);

  datafile.close();

  gettimeofday(&tim, NULL);
  t1 = tim.tv_sec + (tim.tv_usec/1000000.0);
  printf("End program: %.6lf s\n",t1);

  return 0; 

}
