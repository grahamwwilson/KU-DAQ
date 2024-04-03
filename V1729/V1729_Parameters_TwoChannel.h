// V1729_Parameters.h

// Define configurable parameters for V1729. 
// In practice there will need to be different versions of this.

// Current configuration is for channel 0 and channel 1 set up with external trigger.

// It is the user's repsonsibility to check that all configured parameters are mutually consistent

const int V1729_nChannels = 2;                          // Number of channels that are on (defined by the channel mask). Needed for BLT calculation.

const int V1729_Channel_Masks_Value = 0x3;              // Channels 0 and 1
const int V1729_Trigger_Type_Value = 0x12;              // Use external trigger (Set bits 4 and 1 of the 5-bit word) - see page 33.
const int V1729_Trigger_Channels_Value = 0x0;           // Set to 0 to not trigger on particular channels
const int V1729_Trigger_Threshold_DAC_Value = 0x000;    // Threshold (irrelevant given above choices)

const int V1729_Fp_Frequency_Value = 0x2;               // Pilot frequency Fp choice (1=100MHz/2GHz, 2=50MHz/1GHz)
const int V1729_Pretrig_LSB_Value  = 0x4C;              // Pretrig LSB   |
const int V1729_Pretrig_MSB_Value  = 0x1D;              // Pretrig MSB   | PRETRIG = 7500  (Gives 150 us for 50MHz operation)
const int V1729_Posttrig_LSB_Value = 0x40;              // Posttrig LSB  | 
const int V1729_Posttrig_MSB_Value = 0x00;              // Posttrig MSB  | POSTTRIG = 64   (Gives 1.28 us after trigger for 50 MHz operation)

const int V1729_NumCols_Value      = 0x080;             // Number of columns to read (128)

