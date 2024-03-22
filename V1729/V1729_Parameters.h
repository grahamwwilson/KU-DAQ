// V1729_Parameters.h

// Define configurable parameters for V1729. 
// In practice for now there may be different versions of this.

// For now it is the user repsonsibility to check that these are all mutually consistent

const int V1729_nChannels = 1;                          // Number of channels that are on - better to use the channel mask for this.

const int V1729_Channel_Masks_Value = 0x1;              // Only Channel 0
const int V1729_Trigger_Type_Value = 0x05;              // Set falling edge discriminator trigger
const int V1729_Trigger_Channels_Value = 0x1;           // Trigger on Channel 0
const int V1729_Trigger_Threshold_DAC_Value = 0x780;    // Threshold of around -31.25 mV.

const int V1729_Pretrig_LSB_Value  = 0x00;              // Pretrig LSB
const int V1729_Pretrig_MSB_Value  = 0x28;              // Pretrig MSB
const int V1729_Posttrig_LSB_Value = 0x40;              // Posttrig LSB
const int V1729_Posttrig_MSB_Value = 0x00;              // Posttrig MSB

const int V1729_NumCols_Value      = 0x080;             // Number of columns to read

