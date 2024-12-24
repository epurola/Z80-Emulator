#include "IODevice.hpp"
#include <iostream>

IODevice::IODevice() {
    
}

// Default behavior for checking if the device handles the address
bool IODevice::handlesAddress(int address) {
    return address == 0x01;
}

// Read from the device (example)
uint8_t IODevice::read(int address) {
    if (handlesAddress(address)) {
        std::cout << "Reading from I/O device at address 0x" << std::hex << address << std::endl;
        return 0x42;  // Return a dummy value for now
    } else {
        std::cerr << "Attempted read from unmapped I/O address 0x" << std::hex << address << std::endl;
        return 0xFF;  // Return a default value for unmapped addresses
    }
}

void IODevice::write(int address, uint8_t value) {
    if (handlesAddress(address)) {
        std::cout << "Writing 0x" << std::hex << (int)value << " to I/O device at address 0x" << std::hex << address << std::endl;
    } else {
        std::cerr << "Attempted write to unmapped I/O address 0x" << std::hex << address << std::endl;
    }
}
