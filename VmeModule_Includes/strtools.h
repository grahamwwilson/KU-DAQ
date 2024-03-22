// power tools for stings and numbers
// David File, 2007, summer, KU-grad stu

#ifndef STR_TOOLS
#define STR_TOOLS

#include <stdio.h>
#include <math.h>
#include <iostream>
#include <sstream>
#include <string>

using std::ostringstream;
using std::istringstream;

std::string DecToBin(float num)
   {
     std::string str = "";
     double i, top, expmax;
     const double lg2 = log(2.0);

     if(num == 0) return("0");
     expmax = floor(log(num)/lg2);
     for(i=expmax; i>=0; i--){
       top = num - pow(2.0,i);    // subtract the binary basis
       //    std::cout << num << " " << top << std::endl;
       if (top < 0){
         str = str + "0";
       }
       else {
         str = str + "1";
         num = top;
       }
     }
     return(str);
   }

long int BinToDec(std::string str)
   {
     long int i, expmax;
     double var=0;

     expmax = str.length() - 1;
     if(str == "") return(0);
     for(i=0; i<=expmax; i++){
       if (str.compare(i,1,"1",0,1) == 0){
         var = var + pow(2.0,(expmax-i));
       }
     }
     return((long) var);
   }

std::string LongToStr(long lx)
   {
     ostringstream ostr;
     ostr.precision(6);
     ostr.width(10);
     ostr << lx;
     return (ostr.str());
   }

std::string DblToStr(double dx)
   {
     ostringstream ostr;
     ostr.precision(6);
     ostr.width(10);
     ostr << dx;
     return (ostr.str());
   }

int CharToInt(char* cx)
   {
   istringstream strchar;
   strchar.str(cx);
   int i;
   strchar >> i;
   return (i);
   }

#endif // STR_TOOLS
