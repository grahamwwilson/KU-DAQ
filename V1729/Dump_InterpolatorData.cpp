// Dump Interpolator data from file in manner compatible with easy PAW coding

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

  const int NUM_EVENTS = 100000;
  unsigned int ch0, ch1, ch2, ch3;
  long Datum;
//  long DataBuffer[1284];

//  cout << internal << setfill('0');

  for (int i=0; i<NUM_EVENTS; i++){
      for (int j=0; j<2; j++){
          datafile >> hex >> Datum;
//         cout << setfill('0') << setw(8) << hex << Datum << endl;
          if (j==0){
             ch3 = getbits(Datum,31,16);
             ch2 = getbits(Datum,15,16);
          }
          else{
             ch1 = getbits(Datum,31,16);
             ch0 = getbits(Datum,15,16);
          }
      }
      pdatafile << dec << setw(5) << ch0 << endl;
      pdatafile << dec << setw(5) << ch1 << endl;
      pdatafile << dec << setw(5) << ch2 << endl;
      pdatafile << dec << setw(5) << ch3 << endl;
  }

  pdatafile.close();
  datafile.close();

  return 0; 
}
