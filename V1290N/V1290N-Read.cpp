//
// Data acquisition program for V1290N 16-channel multi-hit TDC
// (code started from V965-Read.cpp)
//

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <iomanip>
#include <sys/time.h>
#include "V1290N-CAEN.h"
#include "VMUSB-Pulser.h"
#include "V1290N_configure.h"
#include <vector>

using namespace std;
/* using std::cout;
using std::cin;
using std::endl;
using std::ofstream;
using std::dec;
using std::hex;
using std::right;
using std::left;
using std::string;
using std::fixed; */

void mysleep(){

// A programmatic sleep

    cout << "Programmatic sleep " << endl;
    for (unsigned int i=0; i<2000000; i++){
       if (i%1000000==0)cout << "PS  " << dec << i << endl;
    }   
}

void specifiedsleep(unsigned int maxloops = 1300000){

// A programmatic specified sleep. maxloops = 1300000 is about 16 ms.

    cout << "Programmatic specified sleep " << endl;
    struct timeval tim;
    double tstart,tend;
    
    gettimeofday(&tim, NULL);
    tstart = tim.tv_sec + (tim.tv_usec/1000000.0);
    printf("Start PSS: %.6lf s\n",tstart);    
    
    for (unsigned int i=0; i<maxloops; i++){
       if (i%100000==0)cout << "PSS " << dec << i << endl;
    }
    
    gettimeofday(&tim, NULL);
    tend = tim.tv_sec + (tim.tv_usec/1000000.0);
    printf("End   PSS: %.6lf s\n",tend);     
      
}

int main (int argc, char *argv[]){

  // initialize
    int neventmax = 1000;
    int pulser_freq = 1000000;    // 1 MHz (so every 1 us).
    int gate_freq = 10;           // 10 Hz
    if (argc > 1) neventmax = CharToInt(argv[1]);
    if (argc > 2) pulser_freq = CharToInt(argv[2]);
    if (argc > 3) gate_freq = CharToInt(argv[3]);

    struct timeval tv0,tv1,tv4;  // .sec and .usec
    struct timezone tvz;

    const unsigned int NMIN = 6;      // Software trigger requirement on the minimum number of words in the event to write the event to the data file.
    const int NSIZE = 192;            // Number of 32-bit words to transfer
    
    long int bytes_read = 0;

    long DataBuffer[NSIZE];
    long *pDataBuffer=NULL;  //set to null to avoid a compiler warning
    //Initializing the DataBuffer array
    for (int i=0; i<NSIZE; i++){
        DataBuffer[i] = 0;
    }
    pDataBuffer = &DataBuffer[0];  // Make pDataBuffer point to the DataBuffer array    

    usb_dev_handle *udev;
    std::string logfilename = "logfile.out";
    std::string outfilename = "Data_Out.out";

    udev = VMUSB::DeviceUSBm(1);    // Now use specific USB module

    std::string mode = "f";
    VMUSB::Pulser pulser1( udev, 1, pulser_freq, 25, mode); // udev,int,double,long,string
    cout << pulser1.status() << endl;

    // Hard-wire pulser 2 used for TDC stop to 10 Hz and 300 ns
    VMUSB::Pulser pulser2( udev, 2, gate_freq, 300, mode); // udev,int,double,long,string
    cout << pulser2.status() << endl;

    // logfile: don't open file until everything's been instanced
    ofstream fout ("Data_Out.out");   // open for writing   
    ofstream lout ("logfile.out");    // open for writing 

    gettimeofday(&tv0,&tvz);
    VMUSB::V1290N tdc(udev);
    gettimeofday(&tv1,&tvz);
  
    lout << logfilename << endl;      // include date and time?
    lout << "neventmax set to " << neventmax << " events " << endl;
    lout << "Pulser frequency set to " << pulser_freq << " (Hz) " << endl;
    lout << "Second Pulser frequency used for DAQ event rate " << gate_freq << " (Hz) " << endl;
    lout << "nim " << pulser1.status() << endl;
    
// TDC configuration and checking
    V1290N_configure(tdc, lout);
    
// Do software clear of TDC to start data collection
    tdc.WriteRegister(SW_CLEAR, 0x0000);
    mysleep();

    int event;
    int nevs_stored;
    int nevs_read = 0;
    std::vector<unsigned int>ev_vector; 
    std::vector<unsigned int>::iterator it;       
    
    event = tdc.ReadRegisterD32(EV_CNT);
    nevs_stored = tdc.ReadRegister(EV_STORED);
    lout << "tdc Event Counter Register : 0x" << hex << event << " " << dec << event << endl;
    lout << "tdc Events Stored          : 0x" << hex << nevs_stored << " " << dec << nevs_stored << endl; 
    
    int nbltBytes;
    int nwordsperBLTtransfer = NSIZE;    //TODO check that this is LE buffer DataBuffer array size ...

    while (nevs_read < neventmax ){
    // Check the status register
        int status = getbits( tdc.ReadRegister(STATUS), 1, 2);
        if ( status == 3 ){   // DREADY and ALMOST-FULL set
// Read requested number of words using BLT32
           nbltBytes = tdc.GetDataBLT32(nwordsperBLTtransfer, pDataBuffer);
//           lout << "nbltBytes: 0x" << hex << nbltBytes << " " << dec << nbltBytes << endl;
           bytes_read += nbltBytes;
           for (int i=0; i<nwordsperBLTtransfer; i++){
               int r = DataBuffer[i];
               ev_vector.push_back(r);
//               fout << std::setfill('0') << std::setw(8) << hex << r << endl;    // Prioritize writing the data.               
               int dtype = DATA_TYPE(r);
               if (dtype == 0x10){
              // select events to write to output with at least NMIN words
                   if( ev_vector.size() >= NMIN ){
                        // Event passes software trigger. So write out the complete event
                       for (it = ev_vector.begin(); it < ev_vector.end(); it++){
                           fout << std::setfill('0') << std::setw(8) << hex << *it << endl;    // Write the data.
                       }
                   }
                   ev_vector.clear();
                   nevs_read += 1;
                   if( nevs_read%24000==1 ){                                     // 1 Hz for DAQ rate of 24 kHz
                      lout << "byr, nr, st, bytot: " << dec << nbltBytes << " " << dec << nevs_read << " " 
                           << dec << tdc.ReadRegister(EV_STORED) << " " << dec << bytes_read << endl;  
                   }
                   if( nevs_read%16000==0 ){                                     // 1.5Hz for DAQ rate of 24 kHz
                      specifiedsleep();              // Simulate effect of substantial dead time associated with V1729 readout. Will need to adjust the parameters. 
                      lout << "post sleep buffer content: " << dec << tdc.ReadRegister(EV_STORED) << endl; 
                   }                                         
               }            
//               lout << " Type: 0x" << std::setfill('0') << std::setw(2) << hex << dtype << " Data: 0x" << std::setfill('0') << std::setw(8) << hex << r << endl;
           }   
        }
    }

    event = tdc.ReadRegisterD32(EV_CNT);
    nevs_stored = tdc.ReadRegister(EV_STORED);
    lout << "tdc Overall Event Counter Register (32-bit)" << hex << event << " " << dec << event << endl;
    lout << "tdc Events Stored          " << hex << nevs_stored << " " << dec << nevs_stored << endl;
    lout << "events read out = " << dec << nevs_read << endl;
    lout << "bytes read = " << dec << bytes_read << endl;

    gettimeofday(&tv4,&tvz);

    lout << " SP0   Data-taking started at: " << dec << tv0.tv_sec << " (s)  " << dec << tv0.tv_usec << " (micro-seconds)" << endl;    
    lout << " SP1   Data-taking started at: " << dec << tv1.tv_sec << " (s)  " << dec << tv1.tv_usec << " (micro-seconds)" << endl;
    lout << " EP4 Data-taking completed at: " << dec << tv4.tv_sec << " (s)  " << dec << tv4.tv_usec << " (micro-seconds)" << endl;

    double daqtime = double(tv4.tv_sec - tv1.tv_sec) + 1.0e-6*double(tv4.tv_usec - tv1.tv_usec);
    if( (tv4.tv_usec < tv1.tv_usec) < 0 ) daqtime += 1.0;

    lout << " DAQ time " << daqtime << " s " << endl;
    double MBrate = double(bytes_read)*1.0e-6/daqtime;
    lout << " DAQ rate " << MBrate << " MB/s " << endl;  
    double evsize = double(bytes_read)/double(nevs_read);
    lout.precision(12);
    lout << " Event size " << std::setw(12) << evsize << " bytes " << std::setw(12) << evsize/4.0 << " words " << endl;

    lout << "tdc PROG_HS: 0x" << hex << tdc.ReadRegister(PROG_HS) << endl;
    lout << "tdc STATUS: 0x" << hex << tdc.ReadRegister(STATUS) << endl;

    lout.close();
    fout.close();

    // Close the Device
    cout << " V1290N-Read: Closing VM-USB device " << endl;
    xxusb_device_close(udev);

    return 0;
}
