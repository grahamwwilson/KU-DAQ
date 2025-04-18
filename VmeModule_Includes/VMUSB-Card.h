// USB-Unit-v2.h, code to handle USB interface
// 1/2008, David J. File

#ifndef USB_UNIT
#define USB_UNIT

#include "libxxusb.h"
#include <libusb.h>
#include <cstring>

namespace VMUSB {

int CheckDevice(libusb_device_handle *udev)
{ //Check if device is open. If not, end program
  if(udev)
    {
      std::cout << "-> VM-USB device is open for communication" << std::endl;
      return 0;
    }
  else
    {
      std::cout << "-> NO VM-USB devices found!" << std::endl;
      return 1;   // won't end the program but useful to if() 
    }
}

/*
usb_dev_handle* DeviceUSB()
   {
     xxusb_device_type devices[99];
     struct usb_device *dev;
     usb_dev_handle *udev;       // Device Handle 
     xxusb_devices_find(devices);
     dev = devices[0].usbdev;
     udev = xxusb_device_open(dev);
     return udev;
   }

usb_dev_handle* DeviceUSBi(int usbindex)
  { //can only overload if part of object
    xxusb_device_type devices[99];
    struct usb_device *dev;
    usb_dev_handle *udev;       // Device Handle
    xxusb_devices_find(devices);
    // check index value
    dev = devices[usbindex].usbdev;
    udev = xxusb_device_open(dev);
    return udev;
  }
*/

libusb_device_handle *DeviceUSBm(int usbmodule)
  { //can only overload if part of object

// Available VM-USB modules on July 27th 2011
// 
// VM0036 currently running firmware 86000802 (v08.02) in RH crate with most of the modules (V965 etc ..)
// VM0064 currently running firmware 77000804 (v08.04) in LH crate with the V775 TDC
//
// Add diagnostics.    GWW Sept 9th 2011
//

    const char* USBm1 = "VM0036";     // the one we use the most
    const char* USBm2 = "VM0064";     // the one currently used mostly for TDC stuff

    libusb_device_handle *udev = NULL;       // Device Handle
    int rc;
    
// Initialize libusb
    rc = libusb_init(NULL);
    if (rc < 0) {
        printf("libusb initialization failed\n");
        return NULL;
    }else{
        printf("libusb initialized OK\n");
    }
    
    char* USBm = new char[strlen(USBm1) + 1];

    std::cout << " VM-USB::DeviceUSBm(int usbmodule) called with argument " << usbmodule << std::endl;

    // check module value. in future use a case statement?
    if (usbmodule == 2) {
        strcpy(USBm, USBm2);
    }
    else {
        strcpy(USBm, USBm1);
    }

    std::cout << " VM-USB::DeviceUSBm opening module with serial number " << USBm << std::endl;

    udev = xxusb_serial_open(USBm);
    
    delete[] USBm;
    return udev;
  }

} // end namespace
#endif //USB_UNIT
