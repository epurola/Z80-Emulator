#include <cstdint>
#ifndef IODEVICE_H
#define IODEVICE_H

class IODevice {
public:
IODevice(); 
    virtual ~IODevice() = default; 

    // Check if the device handles this address
    virtual bool handlesAddress(int address) = 0;

    // Read from the device
    virtual uint8_t read(int address) = 0;

    // Write to the device
    virtual void write(int address, uint8_t value) = 0;
};

#endif 
