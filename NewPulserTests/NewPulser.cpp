// NewPulser

#include <iostream>
#include <fstream>
#include "VMUSB-Pulser.h"

// using namespace std;
using std::cout;
using std::cin;
using std::endl;
using std::ofstream;
using std::dec;
using std::hex;
using std::right;
using std::left;

int main(int argc, char* argv[]){

    // initialize
    int cyclemax=10;
    int freq=100;
    int idelay=2;
    const char logName[9]="test.log";
    std::string msgData;

    libusb_device_handle *udev = NULL;

    if (argc > 1) cyclemax = CharToInt(argv[1]);
    if (argc > 2) freq = CharToInt(argv[2]);
    if (argc > 3) idelay = CharToInt(argv[3]);

    cout << "Hello from my program" << endl;

    // Keep track of operating parameters and make available to the GUI

    ofstream pout("test.pars");
    pout << cyclemax << endl;
    pout << freq << endl;
    pout << idelay << endl;
    pout.close();

    udev = VMUSB::DeviceUSBm(2);
    //    cout << VMUSB::CheckUSB(udev) << endl;

    ofstream fout(logName);   // open for writing
    fout << logName << endl;  // include date and time

    std::string mode = "f";
    VMUSB::Pulser pulser1(udev,1,freq,100,mode); // udev,int,double,long,string
    cout << pulser1.status() << endl;

    VMUSB::Pulser pulser2(udev,2,freq,100,mode); // udev,int,double,long,string
    cout << pulser2.status() << endl;

    msgData = pulser1.status();
    cout << msgData << endl;
    std::size_t msg2 = msgData.find("Fail");
    if(msg2 != std::string::npos) return(-1);

    return 0;
}
