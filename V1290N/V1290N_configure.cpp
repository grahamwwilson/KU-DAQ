#include <stdio.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include "V1290N-CAEN.h"
using namespace std;

void V1290N_configure(VMUSB::V1290N tdc, std::ofstream& lout){

// Start TDC configuration and checking
    lout << "tdc " << tdc.GetRevision() << endl;
    lout << "tdc FW revision: 0x" << hex << tdc.ReadRegister(FW_REVISION) << endl;

// Now test writing and reading
    lout << "tdc BLT ENR: 0x" << hex << tdc.ReadRegister(BLT_EVNUM) << endl;
    short int value = 19;
    tdc.WriteRegister(BLT_EVNUM, value);
    lout << "tdc BLT ENR: 0x" << hex << tdc.ReadRegister(BLT_EVNUM) << endl;
    tdc.WriteRegister(BLT_EVNUM, 0);   // disables event aligned BLT - we are OK with getting some partial events and don't want to add fillers
    lout << "tdc BLT ENR: 0x" << hex << tdc.ReadRegister(BLT_EVNUM) << endl;    
    
// Now test use of opcodes etc.
    lout << "tdc STATUS: 0x" << hex << tdc.ReadRegister(STATUS) << endl;
    lout << "tdc PROG_HS: 0x" << hex << tdc.ReadRegister(PROG_HS) << endl;

    opcdData[0] = 0x0000;
    int rc = tdc.WriteOpcode(1, opcdData);  // Set Trigger Matching Mode 
    lout << "tdc WriteOpcode RC " << rc << endl;    
    lout << "tdc PROG_HS: 0x" << hex << tdc.ReadRegister(PROG_HS) << endl;
    lout << "tdc STATUS: 0x" << hex << tdc.ReadRegister(STATUS) << endl;
    lout << "tdc PROG_HS: 0x" << hex << tdc.ReadRegister(PROG_HS) << endl;    

    opcdData[0] = 0x0100;
    rc = tdc.WriteOpcode(1, opcdData);  // Set Continuous Storage Mode 
    lout << "tdc WriteOpcode RC " << rc << endl;
    lout << "tdc PROG_HS: 0x" << hex << tdc.ReadRegister(PROG_HS) << endl;
    lout << "tdc STATUS: 0x" << hex << tdc.ReadRegister(STATUS) << endl;
    lout << "tdc PROG_HS: 0x" << hex << tdc.ReadRegister(PROG_HS) << endl;

    opcdData[0] = 0x0200;
    rc = tdc.WriteOpcode(1, opcdData);  // Read acquisition mode 
    lout << "tdc WriteOpcode RC " << rc << endl;    
    lout << "tdc PROG_HS: 0x" << hex << tdc.ReadRegister(PROG_HS) << endl;
    lout << "tdc STATUS: 0x" << hex << tdc.ReadRegister(STATUS) << endl;
    lout << "tdc PROG_HS: 0x" << hex << tdc.ReadRegister(PROG_HS) << endl;
    int Datum = tdc.ReadOneDatumUsingOpcode();
    lout << "tdc Read Microcontroller Datum: 0x" << hex << Datum << endl;  
    
    opcdData[0] = 0x0000;
    rc = tdc.WriteOpcode(1, opcdData);  // Set Trigger Matching Mode 
    lout << "tdc WriteOpcode RC " << rc << endl;    
    lout << "tdc PROG_HS: 0x" << hex << tdc.ReadRegister(PROG_HS) << endl;
    lout << "tdc STATUS: 0x" << hex << tdc.ReadRegister(STATUS) << endl;
    lout << "tdc PROG_HS: 0x" << hex << tdc.ReadRegister(PROG_HS) << endl;
    
    opcdData[0] = 0x0200;
    rc = tdc.WriteOpcode(1, opcdData);  // Read acquisition mode 
    lout << "tdc WriteOpcode RC " << rc << endl;    
    lout << "tdc PROG_HS: 0x" << hex << tdc.ReadRegister(PROG_HS) << endl;
    lout << "tdc STATUS: 0x" << hex << tdc.ReadRegister(STATUS) << endl;
    lout << "tdc PROG_HS: 0x" << hex << tdc.ReadRegister(PROG_HS) << endl;
    Datum = tdc.ReadOneDatumUsingOpcode();
    lout << "tdc Read Microcontroller Datum: 0x" << hex << Datum << endl; 
    
// Now set Trigger Matching Mode operating conditions
    opcdData[0] = 0x1000;
    opcdData[1] = 0x0708;               // 45.0 us    
    rc = tdc.WriteOpcode(2, opcdData);  // Set Window Width
    opcdData[0] = 0x1100;
    opcdData[1] = 0xf90c;               //-44.5 us    
    rc = tdc.WriteOpcode(2, opcdData);  // Set Window Offset
    opcdData[0] = 0x1200;
    opcdData[1] = 0x14;                 // 500 ns    
    rc = tdc.WriteOpcode(2, opcdData);  // Set Extra Search Margin    
    opcdData[0] = 0x1400;
    rc = tdc.WriteOpcode(1, opcdData);  // Enable subtraction of trigger time 

    opcdData[0] = 0x3100;
    rc = tdc.WriteOpcode(1, opcdData);  // Disable TDC header and trailer       
    
    opcdData[0] = 0x1600;
    rc = tdc.WriteOpcode(1, opcdData);  // Read trigger configuration 
    lout << "tdc WriteOpcode RC " << rc << endl;    
    lout << "tdc PROG_HS: 0x" << hex << tdc.ReadRegister(PROG_HS) << endl;
    lout << "tdc STATUS: 0x" << hex << tdc.ReadRegister(STATUS) << endl;
    lout << "tdc PROG_HS: 0x" << hex << tdc.ReadRegister(PROG_HS) << endl;
    int TrigDatum1_MWW  = tdc.ReadOneDatumUsingOpcode();
    int TrigDatum2_WO   = tdc.ReadOneDatumUsingOpcode();
    int TrigDatum3_ESM  = tdc.ReadOneDatumUsingOpcode();
    int TrigDatum4_RM   = tdc.ReadOneDatumUsingOpcode();
    int TrigDatum5_SUB  = tdc.ReadOneDatumUsingOpcode();      
    lout <<  " " << endl;
    lout << "Trigger configuration " << endl;
    lout << " " << endl;          
    lout << "tdc Read Microcontroller Match-Window-Width      : 0x" << hex << TrigDatum1_MWW << endl;
    lout << "tdc Read Microcontroller Window-Offset           : 0x" << hex << TrigDatum2_WO << endl;
    lout << "tdc Read Microcontroller Extra-Search-Margin     : 0x" << hex << TrigDatum3_ESM << endl;
    lout << "tdc Read Microcontroller Reject-Margin           : 0x" << hex << TrigDatum4_RM << endl;
    lout << "tdc Read Microcontroller Trigger-Time-Subtraction: 0x" << hex << TrigDatum5_SUB << endl; 
    
    // Enable all channels
    opcdData[0] = 0x4200;
    rc = tdc.WriteOpcode(1, opcdData);  // Enable all channels
        
    // Double-check control register
    int cr = tdc.ReadRegister(CONTROL);
    lout << "tdc CONTROL: 0x" << hex << cr << endl;
    
    // Now set the TRIGGER TIME TAG ENABLE bit in the CONTROL register (COSTS an extra word per event)
    int newval = cr | 0x0200;
    tdc.WriteRegister(CONTROL, newval);
    cr = tdc.ReadRegister(CONTROL);
    lout << "tdc CONTROL: 0x" << hex << cr << endl; 
    
    lout << "tdc Almost Full Level Register: 0x" << hex << tdc.ReadRegister(AF_LEV) << endl;
    tdc.WriteRegister(AF_LEV, 0xC0);   // 192 words
    lout << "tdc Almost Full Level Register: 0x" << hex << tdc.ReadRegister(AF_LEV) << " " << dec << tdc.ReadRegister(AF_LEV) << endl;    
        
// Do software clear
//    tdc.WriteRegister(SW_CLEAR, 0x0000);
  
//    mysleep();
// End of TDC configuration    

}
