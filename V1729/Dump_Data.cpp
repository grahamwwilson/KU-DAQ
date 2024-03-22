// Dump Ch0 data from file in manner compatible with previous PAW code.

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <limits.h>
#include "getbits.h"
using namespace std;

int main ()
{

  ifstream  datafile;
  datafile.open("RawData.out");

// decoded data only
  ofstream pdatafile;
  pdatafile.open("Data.out");
  pdatafile << showbase << internal << setfill(' ');

  const int NUM_EVENTS = 50000;
  unsigned int ch0[2563];
  long Datum;
  long DataBuffer[1284];
  long Trig_Rec_Data;

// cout << internal << setfill('0');

  for (int i=0; i<NUM_EVENTS; i++){
      for (int j=0; j<1284; j++){
         datafile >> hex >> Datum;
//         cout << setfill('0') << setw(8) << hex << Datum << endl;
         DataBuffer[j] = Datum;
         if(j>=2){
            if(j<1283){   
               ch0[2*j-4] = getbits(Datum,31,16);
               ch0[2*j-3] = getbits(Datum,15,16);
            }
            else{
               ch0[2*j-4] = getbits(Datum,31,16);     //  Pick up the last word from the MSB part of the last word 
            }
         }
      }
      Trig_Rec_Data= DataBuffer[1];
      pdatafile << dec << setw(3) << getbits(Trig_Rec_Data,7,8) << endl;
      for(int k=0;k<2563;k++){
         pdatafile << dec << setw(5) << ch0[k] << endl;
      } 
  }

  pdatafile.close();
  datafile.close();

  return 0; 
}

