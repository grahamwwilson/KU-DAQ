//  Program to intialize the V1729 and read data 

#include <libxxusb.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "V1729_constants.h"
#include "V1729_initialize.h"
#include "V1729_config.h"
#include "getbits.h"
using namespace std;

int main ()
{
  const int NUM_EVENTS = 10;

  struct timeval tim;
  double t1;

  gettimeofday(&tim, NULL);
  t1 = tim.tv_sec + (tim.tv_usec/1000000.0);
  printf("Start program: %.6lf s\n",t1);

  //  sleep(10);
  
  //  long Data = 0;
  int initialize_count = 1;
  long DataBuffer[V1729_Array_Size];
  long *pDataBuffer=NULL;
// file with the raw data and some decoding
  ofstream  datafile;
  datafile.open("RawData.out");

// decoded data only
  ofstream pdatafile;
  pdatafile.open("Data.out"); 

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
  
  cout << "32-bit array size = " << V1729_Array_Size << endl;

  //Initializing the DataBuffer  
    for (int i=0; i<V1729_Array_Size; i++){
      DataBuffer[i] = 0;
    }  
    pDataBuffer = &DataBuffer[0];  // Make pDataBuffer point to the DataBuffer array


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

  for( int iev=0; iev<NUM_EVENTS; iev++){
 
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

/*
  while(j<repeat)
  {
    //Block Transfer
    VME_BLT_read_32(udev, V1729_BLT_am, V1729_Array_Size-1, V1729_BA+V1729_RAM_Data_VME, pDataBuffer);
    k=0;
    while (k < V1729_Array_Size) 
      {
        datafile << hex <<  DataBuffer [k]  << endl;
        k++;
      }  
    j++;
    cout << "Block Read " << dec << j << endl;
  }
*/

  // Do some formatting for hex numbers
  // (See http://www.cplusplus.com/forum/windows/51591/ )
  datafile << showbase << internal << setfill('0');  

  // Include the trigger record in the data file as the first entry
  datafile << "TRIG REC " << hex << setw(8) << Trig_Rec_Data << " (" << dec << returnVME << ") " << dec << getbits(Trig_Rec_Data,7,8) << endl;   

  // Reading a single event from the RAM
  unsigned int ch0[2563];
  unsigned int ch1[2563];
  unsigned int ch2[2563];
  unsigned int ch3[2563];
  long Data;
  int i=0;
  int j=0;
  while(i<V1729_Array_Size)
  {    
    //VME_write_32(udev, V1729_am, V1729_BA+V1729_Start_Acquisition, 1); 
    VME_read_32(udev, V1729_am, V1729_BA+V1729_RAM_Data_VME, &Data);
    datafile << hex << setw(10) << Data << "  " << dec << setw(5) << getbits(Data,31,16) << " " << setw(5) << getbits(Data,15,16) << endl;
    if(i%2==0){
       ch3[j] = getbits(Data,31,16);
       ch2[j] = getbits(Data,15,16);
    }
    else{
       ch1[j] = getbits(Data,31,16);
       ch0[j] = getbits(Data,15,16);
       j++;                               // Only increment j after we have found the last channel for this cell
    } 
    if (i%2000==0) 
    {
	cout << "Read " << j << " cells" << endl;
    }
    i++;    
  }

  gettimeofday(&tim, NULL);
  t1 = tim.tv_sec + (tim.tv_usec/1000000.0);
  printf("Data read from RAM %.6lf s\n",t1);

  // Write decoded data to the datafile.
  // Should have some kind of header eventually including an event number.
  // Dump the decoded decimal TRIG_REC to the file
  // Dump ch0 to the output file

  pdatafile << showbase << internal << setfill(' ');      // Over-ride previous leading 0 request
  pdatafile << dec << setw(3) << getbits(Trig_Rec_Data,7,8) << endl;
  for(int k=0;k<2563;k++){
     pdatafile << dec << setw(5) << ch0[k] << endl;
  } 

  gettimeofday(&tim, NULL);
  t1 = tim.tv_sec + (tim.tv_usec/1000000.0);
  printf("Data written to file %.6lf s\n",t1);

  }          // End of event loop

  //  Check the board settings at the end
  V1729_initialize(initialize_count, udev);

  datafile.close();
  pdatafile.close();
  return 0; 
}

