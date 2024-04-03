//  Program to intialize the V1729 and generate an output logfile  

#include <libxxusb.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include "V1729_Addresses.h"
#include "getbits.h"
using namespace std;

int V1729_initialize(int initialize_count, usb_dev_handle *udev) //sets a variable so you can tell when the initialize 
                                                                 //function was implemented 
{
  ofstream logfile;

  if(initialize_count == 1)
  {
    logfile.open("Data_Initialize_1.out");
  }

  if(initialize_count == 2)
  {
    logfile.open("Data_Initialize_2.out");
  }

  if(initialize_count == 3)
  {
    logfile.open("Data_Initialize_3.out");
  }

  long Data = 0;
 
  logfile << "Values for initialization check number " << initialize_count << endl;

  VME_read_32(udev, V1729_am, V1729_BA + V1729_Trigger_Threshold_DAC_VME, &Data);  
  logfile << endl << "TRIGGER THRESHOLD DAC: 0x" << hex << getbits(Data,11,12) << endl;

  VME_read_32(udev, V1729_am, V1729_BA + V1729_RAM_int_add_LSB, &Data);  
  logfile << endl << "RAM INT ADD LSB: 0x" << hex << getbits(Data,7,8) << endl; 
 
  VME_read_32(udev, V1729_am, V1729_BA + V1729_RAM_int_add_MSB, &Data);  
  logfile << endl << "RAM int add MSB: 0x" << hex << getbits(Data,7,8) << endl; 
 
  VME_read_32(udev, V1729_am, V1729_BA + V1729_Mat_Ctrl_Register_LSB, &Data);  
  logfile << endl << "MAT CTRL REGISTER LSB: 0x" << hex << getbits(Data,7,8) << endl; 
 
  VME_read_32(udev, V1729_am, V1729_BA + V1729_Mat_Ctrl_Register_MSB, &Data);  
  logfile << endl << "MAT CTRL REGISTER MSB: 0x" << hex << getbits(Data,7,8) << endl; 
 
  VME_read_32(udev, V1729_am, V1729_BA + V1729_Pretrig_LSB, &Data);  
  logfile << endl << "PRETRIG LSB: 0x" << hex << getbits(Data,7,8) << endl; 
 
  VME_read_32(udev, V1729_am, V1729_BA + V1729_Pretrig_MSB, &Data);  
  logfile << endl << "PRETRIG MSB: 0x" << hex << getbits(Data,7,8) << endl; 
 
  VME_read_32(udev, V1729_am, V1729_BA + V1729_Posttrig_LSB, &Data);  
  logfile << endl << "POSTTRIG LSB: 0x" << hex << getbits(Data,7,8) << endl; 
 
  VME_read_32(udev, V1729_am, V1729_BA + V1729_Posttrig_MSB, &Data);  
  logfile << endl << "POSTTRIG MSB: 0x" << hex << getbits(Data,7,8) << endl;
  
  VME_read_32(udev, V1729_am, V1729_BA + V1729_Trigger_Type, &Data);  
  logfile << endl << "TRIGGER TYPE: 0x" << hex << getbits(Data,4,5) << endl;
  
  VME_read_32(udev, V1729_am, V1729_BA + V1729_Trigger_Channel_Source, &Data);  
  logfile << endl << "TRIGGER CHANNEL SOURCE: 0x" << hex << getbits(Data,3,4) << endl;  

  VME_read_32(udev, V1729_am, V1729_BA + V1729_Fast_Read_Modes, &Data);  
  logfile << endl << "FAST READ MODES: 0x" << hex << getbits(Data,1,2) << endl; 
 
  VME_read_32(udev, V1729_am, V1729_BA + V1729_Nb_of_Cols_to_Read, &Data);  
  logfile << endl << "NB COLS TO READ: 0x" << hex << getbits(Data,7,8) << endl;
  
  VME_read_32(udev, V1729_am, V1729_BA + V1729_Channel_Masks, &Data);  
  logfile << endl << "CHANNEL MASKS: 0x" << hex << getbits(Data,3,4) << endl;  

  VME_read_32(udev, V1729_am, V1729_BA + V1729_Post_Stop_Latency, &Data);  
  logfile << endl << "POST STOP LATENCY: 0x" << hex << getbits(Data,7,8) << endl;  
  
  VME_read_32(udev, V1729_am, V1729_BA + V1729_Post_Latency_Pretrig, &Data);  
  logfile << endl << "POST LATENCY PRETTRIG: 0x" << hex << getbits(Data,7,8) << endl;

  VME_read_32(udev, V1729_am, V1729_BA + V1729_Interrupt, &Data);  
  logfile << endl << "INTERRUPT: 0x" << hex << getbits(Data,0,1) << endl;

  VME_read_32(udev, V1729_am, V1729_BA + V1729_Fp_Frequency, &Data);  
  logfile << endl << "FP FREQUENCY: 0x" << hex << getbits(Data,1,2) << endl;

  VME_read_32(udev, V1729_am, V1729_BA + V1729_FPGA_Version, &Data);  
  logfile << endl << "FPGA VERSION: 0x" << hex << getbits(Data,7,8) << endl;

  VME_read_32(udev, V1729_am, V1729_BA + V1729_En_VME_IRQ, &Data);  
  logfile << endl << "EN VME IRQ: 0x" << hex << getbits(Data,0,1) << endl;

  logfile << "End of initialization check number " << initialize_count << endl;

  logfile.close(); 
 
  return 0;
}

