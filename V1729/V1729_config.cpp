//  Program to configure the V1729 for the specified task  
//  This is the function that sets up the registers on the V1729

#include <libxxusb.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include "V1729_Addresses.h"
#include "V1729_Parameters.h"
#include "V1729_config.h"
#include "getbits.h"
using namespace std;

int V1729_config(usb_dev_handle *udev)
{
  // Reset the board
  VME_write_32(udev, V1729_am, V1729_BA + V1729_Reset_Board, 1);
  
  // Use channel mask to decide which channels are recorded
  VME_write_32(udev, V1729_am, V1729_BA + V1729_Channel_Masks, V1729_Channel_Masks_Value);

  // Set the trigger type
  VME_write_32(udev, V1729_am, V1729_BA + V1729_Trigger_Type, V1729_Trigger_Type_Value);

  // Set the channels contributing to the trigger
  VME_write_32(udev, V1729_am, V1729_BA + V1729_Trigger_Channel_Source, V1729_Trigger_Channels_Value);

  // Set the trigger threshold DAC
  VME_write_32(udev, V1729_am, V1729_BA + V1729_Trigger_Threshold_DAC_VME, V1729_Trigger_Threshold_DAC_Value);

  // Load the trigger DAC value
  VME_write_32(udev, V1729_am, V1729_BA + V1729_Load_Trigger_Threshold_DAC, 1);
  
  // Set the pilot frequency (50 MHz or 100 MHz) corresponding to 1 GHz or 2GHz sampling
  VME_write_32(udev, V1729_am, V1729_BA + V1729_Fp_Frequency, V1729_Fp_Frequency_Value);  

  // Setting the value of the Pre Trigger
  VME_write_32(udev, V1729_am, V1729_BA + V1729_Pretrig_LSB, V1729_Pretrig_LSB_Value);
  VME_write_32(udev, V1729_am, V1729_BA + V1729_Pretrig_MSB, V1729_Pretrig_MSB_Value);
 
  // Setting the value of the Post Trigger
  VME_write_32(udev, V1729_am, V1729_BA + V1729_Posttrig_LSB, V1729_Posttrig_LSB_Value);
  VME_write_32(udev, V1729_am, V1729_BA + V1729_Posttrig_MSB, V1729_Posttrig_MSB_Value);

  // Setting the number of columns to read
  VME_write_32(udev, V1729_am, V1729_BA + V1729_Nb_of_Cols_to_Read, V1729_NumCols_Value);
 
  return 0;
}
