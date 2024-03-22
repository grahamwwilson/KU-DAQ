// V1729_Parameters_TemporalInterpolator.h

// Define configurable parameters for V1729. 
// In practice for now there may be different versions of this.

// For now it is the user responsibility to check that these are all mutually consistent

const int V1729_nChannels = 4;                          // Number of channels that are on - better to use the channel mask for this.

const int V1729_Channel_Masks_Value = 0xF;              // All 4 channels
const int V1729_Trigger_Type_Value = 0x08;              // Use internal trigger for pedestals
const int V1729_Trigger_Channels_Value = 0x0;           // No discriminator trigger
const int V1729_Trigger_Threshold_DAC_Value = 0x600;    // Threshold of around -125 mV.  (irrelevant)

const int V1729_Posttrig_LSB_Value = 0x01;              // Posttrig LSB
const int V1729_Posttrig_MSB_Value = 0x00;              // Posttrig MSB
const int V1729_Pretrig_LSB_Value = 0x01;               // Pretrig LSB
const int V1729_Pretrig_MSB_Value = 0x00;               // Pretrig MSB
const int V1729_NumCols_Value = 0x01;                         // Number of columns to read
