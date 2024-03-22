//  Program to intialize the V965 and read data 

#include <libxxusb.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include "V1729_constants.h"
#include "V1729_initialize.h"
#include "V1729_config.h"
#include "getbits.h"
using namespace std;

int main ()
{
  int i;
  int k;
  long Data = 0;
  int initialize_count = 1;
  long DataBuffer[V1729_BLT_Words];
  long *pDataBuffer=NULL;
  ofstream  datafile;
  datafile.open("Data_Out.out"); 

  xxusb_device_type devices[100]; 
  struct usb_device *dev;
  usb_dev_handle *udev;       // Device Handle 

  printf ("\n\nRunning the V1729 reading program \n\n");

  //Find XX_USB devices and open the first one found
  xxusb_devices_find(devices);
  dev = devices[0].usbdev;
  udev = xxusb_device_open(dev);    

  //Check if device is open. If not, end program
  if(!udev)
  {
      printf("\n\nNo XX_USB devices found!\n\n");
      return 0;
  }
    
  // Check the board settings
  V1729_initialize(initialize_count, udev);
  initialize_count++;

  // Configure the system
  V1729_config(udev); 
  
  // Verify the Board settings
  V1729_initialize(initialize_count, udev);
  initialize_count++;  

  //Start Acquisition
  VME_write_32(udev, V1729_am, V1729_BA+V1729_Start_Acquisition, 1);  
  sleep(3);
 
  // Reading a single event from the RAM
  i=0;
  while(i<11000)
  {    
    //VME_write_32(udev, V1729_am, V1729_BA+V1729_Start_Acquisition, 1); 
    VME_read_32(udev, V1729_am, V1729_BA+V1729_RAM_Data_VME, &Data);
    datafile << dec << getbits(Data,11,12) << endl;
    i++;
    if (i%1000==0)
    {
	cout << "Recorded " << i << " events" << endl;
    }
    
  }

  //  Check the board settings at the end
  V1729_initialize(initialize_count, udev);

  datafile.close();
  return 0; 
}
