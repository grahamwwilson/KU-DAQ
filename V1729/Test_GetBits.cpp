//  Test getbits code

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <limits.h>
#include "getbits.h"
using namespace std;

int main ()
{

// First figure out what the "limits" are on this machine 
// from Appendix B11 of Kernighan and Ritchie

  cout << CHAR_BIT << endl;
  cout << CHAR_MAX << " " << CHAR_MIN << endl;
  cout << INT_MAX  << " " << INT_MIN  << endl;
  cout << LONG_MAX << " " << LONG_MIN << endl;
  cout << SCHAR_MAX << " " << SCHAR_MIN << endl;
  cout << SHRT_MAX << " " << SHRT_MIN << endl;
  cout << UCHAR_MAX << " " << UINT_MAX << " " << ULONG_MAX << " " << USHRT_MAX << endl;

 
  unsigned long ulData = 0xf1f1f1f1;
  cout << hex << "0x" << ulData << "  0x" << getbits(ulData,11,12) << endl;

  unsigned int uiData = 0xf1f1;
  cout << hex << "0x" << uiData << "  0x" << getbits(uiData,11,12) << endl;

  return 0; 
}

