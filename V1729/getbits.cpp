//getbits.cpp

#include <iostream>
#include "getbits.h"
using namespace std;

/*  Function to get bits  p, p-1...p-n from word x
Taken from pg. 40 of Kernighan and Ritchie 1988 ANSI-C N.B.
p is the highest bit
*/

unsigned int getbits(unsigned x, int p, int n){
  unsigned int r;
  r = (x >> (p+1-n)) & ~(~0 << n);
  return (r);
  }

