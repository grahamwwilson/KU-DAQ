// V1729_constants.h

// All definitions are on page 32 of the Revision 9 manual.

// V1729 Definitions

const int V1729_BA = 0x30000000;
const int V1729_am = 0x09;
const int V1729_BLT_am = 0x0B;

// V1729 Register Sub-Addresses

const int V1729_Reset_Board = 0x0800;                        // Reset the board
const int V1729_Load_Trigger_Threshold_DAC = 0x0900;         // Loads DAC register value in the analog converter

const int V1729_Start_Acquisition = 0x1700;                  // Launches DAQ and resets the INTERRUPT register
const int V1729_Software_Trigger = 0x1C00;
const int V1729_Trigger_Threshold_DAC_VME = 0x0A00;

const int V1729_RAM_Data_VME = 0x0D00;
const int V1729_RAM_int_add_LSB = 0x0E00;                    // RAM interrupt address 
const int V1729_RAM_int_add_MSB = 0x0F00;  
const int V1729_Mat_Ctrl_Register_LSB = 0x1000;
const int V1729_Mat_Ctrl_Register_MSB = 0x1100;

const int V1729_Pretrig_LSB = 0x1800;
const int V1729_Pretrig_MSB = 0x1900;
const int V1729_Posttrig_LSB = 0x1A00;
const int V1729_Posttrig_MSB = 0x1B00;
const int V1729_Trigger_Type = 0x1D00;
const int V1729_Trigger_Channel_Source = 0x1E00;
const int V1729_Trig_Rec = 0x2000;

const int V1729_Fast_Read_Modes = 0x2100;
const int V1729_Nb_of_Cols_to_Read = 0x2200;
const int V1729_Channel_Masks = 0x2300;

const int V1729_Post_Stop_Latency = 0x3000;
const int V1729_Post_Latency_Pretrig = 0x3100;
const int V1729_Interrupt = 0x8000;
const int V1729_Fp_Frequency = 0x8100;
const int V1729_FPGA_Version = 0x8200;
const int V1729_En_VME_IRQ = 0x8300;

// Variables used to set registers, that are not the default settings
const int V1729_nChannels = 4;                          // Number of channels that are on
const int V1729_Channel_Masks_Value = 0xF;              // All channels are unmasked

const int V1729_Trigger_Type_Value = 0x05;              // Set falling edge discriminator trigger
const int V1729_Trigger_Channels_Value = 0x1;           // Trigger on channel 0
const int V1729_Trigger_Threshold_DAC_Value = 0x600;    // Threshold of around -125 mV.

const int V1729_BLT_Words = 64;                         // Sets number of words to get in BLT
const int V1729_Posttrig_LSB_Value = 64;                // Sets Posttrig
const int V1729_Array_Size = 2563*V1729_nChannels/2;    // 32-bit words in the databuffer array  (2560 cells per channel + 3 "header" cells per channel)

