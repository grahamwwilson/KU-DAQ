/* Simple VM_USB program
 * 
 * Copyright (C) 2005-2009 WIENER, Plein & Baus, Ltd (thoagland@wiener-us.com)
 *
 *     This program is free software; you can redistribute it and/or
 *     modify it under the terms of the GNU General Public License as
 *     published by the Free Software Foundation, version 2.
 *
 *
 *
 * This file will show the basic principles of using the xx_usb
 * Library.  It is offerered as a simple version idea of how to:
 *       Open the Device
 *       Set up the LEDs
 *       Set up a DGG
 *       Close the Device
 * In order for it to work be sure to change the serial number
 * To that of your VM_USB.
 *
 *
 * To compile, I have assumed that you have already put libxx_usb.so
 * and xx_usb.h in a standard location 
 * compile as:
 * gcc -shared Simple_VME Simple_VME.c -lxx_usb
 */

// Started here from example for XX-USB_CD2_0
// and updated to                xxusb_3.13 
// which has a new library etc.

// Updated here to xxusb_3.3 and had to do major revisions to get it to work with libusb-1.0.
// Graham  March 29th, 2025.
// Current compilation command that produces an executable is 
// g++ -v -O -Wall -fPIC -g -I/usr/include/libusb-1.0/ -L/usr/lib/ -o Simple_VME.exe Simple_VME.cpp -lxx_usb -lusb-1.0 -Wl,-rpath=/usr/lib/
//
// Now have simplified the Makefile, and it implements the following.
// g++ -v -O -Wall -fPIC -g -I/usr/include/libusb-1.0/ -o Simple_VME.exe Simple_VME.cpp -lxx_usb -lusb-1.0 -L/usr/lib/
// May also need to export LD_LIBRARY_PATH=/usr/lib:$LD_LIBRARY_PATH although I don't think so.

//#include <xx_usb.h>
#include <iostream>
#include <iomanip>
#include <stdio.h>
#include <libxxusb.h>
#include <unistd.h>  // For sleep()
#include <cstring>
#include "../Headers/CLI11.hpp"

int testVMUSB (int which)
{

// New C/C++ standards complain about mutable character array but this is what is 
// required by xxusb_serial_open.

// Wiener VM-USB received Summer 2006
    const char* constString = "VM0036";    // The one with the VHQ
// The second Wiener VM-USB received 2007?
    const char* constString2 = "VM0064";    // The one with the V560N scaler    

// Allocate memory for the modifiable char array
    char* serialString =  new char[strlen(constString) + 1];  // +1 for null terminator. This is the serial number of VM_USB 

// Copy the content of the const string to the modifiable one
    if (which==1) {
        strcpy(serialString, constString);
    }
    else{
        strcpy(serialString, constString2);    
    }

    libusb_device_handle *udev = NULL;       // Device Handle     

    //GWW Use code as in vmusb_flash.cpp
    long fwid;
    int rc;
    
// Initialize libusb in main program
    rc = libusb_init(NULL);
    if (rc < 0) {
        printf("libusb initialization failed\n");
        return -1;
    }else{
        printf("libusb initialized OK\n");
    }    

    udev = xxusb_serial_open(serialString);  // Open Device and get handle
   // Open the USB device with the selected serial number
    
    // Make sure VM_USB opened OK
    if(!udev) 
    {
	printf ("\nFailed to open VM_USB \n\n");
        libusb_exit(NULL);  // Clean up libusb	
	
	delete[] serialString;

	return -1;
    }
    else{
	printf ("\nEstablished communication with VM_USB \n\n");        
    }

    // Check the firmware version
    printf ("\n\nChecking the programmed firmware version\n\n");

    sleep(1);

    rc=VME_register_read(udev,0,&fwid);
    printf("Number of bytes read during firmware ID check: %d\n", rc);  // Print the byte count from firmware ID check
    if (rc <= 0) {  // If we didn't read any bytes or if there was an error
        printf("Error reading firmware ID\n");
        xxusb_device_close(udev);  // Close the device before exiting
        libusb_exit(NULL);
        return rc;  // Return the error code (number of bytes read)
    }    
    printf("\nThe Firmware ID is %lx \n",fwid);
    
    std::cout << "The firmare ID is " << std::hex << fwid << std::endl;

    // Set Red LED to light with NIM I1 input 
    printf ("\nCalling VME_LED_settings\n\n");
    VME_LED_settings(udev,1,1,0,0);   
    printf ("\nCalled VME_LED_settings\n\n");

    // Set DGG channel A (2nd arg=0) to trigger on NIM I1 input (3rd argument =1), output on NIM O2 (4th arg =1)
    //     with delay =10 x 12.5ns,
    //     and width =  20 x 12.5ns,
    //     not latching or inverting
    printf ("\nCalling VME_DGG\n\n");
    VME_DGG(udev,0,1,1,10,20,0,0);
    printf ("\nCalled VME_DGG\n\n");
    
    // Close the Device
    xxusb_device_close(udev);
    libusb_exit(NULL);
    
    return 0;
}

int main(int argc, char** argv){

    CLI::App app{"Basic test of the VM-USB operation"};  
        
    int which = 1;
    app.add_option("-w,--which", which, "Which crate? (1 for VM0036, 2 for VM0064)")
        ->check(CLI::Range(1, 2));  // Ensure it's either 1 or 2    
    
    CLI11_PARSE(app, argc, argv);
    
    std::cout << "which is set to " << which << std::endl;

    int rc = testVMUSB(which);
    printf ("\ntestVMUSB returns %d \n",rc);
       
    return 0;
    
}
