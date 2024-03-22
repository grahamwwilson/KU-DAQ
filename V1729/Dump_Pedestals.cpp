// Dump Pedestals data from file in manner compatible with easy PAW coding
// Assume 4 channels per record.

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

  const int NUM_EVENTS = 1000;
  unsigned int ch0[2563], ch1[2563], ch2[2563], ch3[2563];
  long EventNumber;
  long TriggerRecord;
  long FirstDatum;
  long SecondDatum;

  for (int i=0; i<NUM_EVENTS; i++){

      datafile >> hex >> EventNumber;
      datafile >> hex >> TriggerRecord;
      for (int j=0; j<2563; j++){
          datafile >> hex >> FirstDatum;
          datafile >> hex >> SecondDatum;
          ch3[j] = getbits(FirstDatum,31,16);
          ch2[j] = getbits(FirstDatum,15,16);
          ch1[j] = getbits(SecondDatum,31,16);
          ch0[j] = getbits(SecondDatum,15,16);
          pdatafile << dec << setw(5) << ch0[j] << endl;
          pdatafile << dec << setw(5) << ch1[j] << endl;
          pdatafile << dec << setw(5) << ch2[j] << endl;
          pdatafile << dec << setw(5) << ch3[j] << endl;
      }

  }

  pdatafile.close();
  datafile.close();

  return 0; 
}
