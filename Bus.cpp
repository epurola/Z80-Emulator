#include "bus.hpp"
#include "memory.hpp"
#include "IODevice.hpp"
#include <iostream>  
#include <algorithm> 
#include <cstdlib> 


// Constructor to initialize memory
Bus::Bus(Memory& mem) : memory(mem), ioDevice(nullptr) {
    uint8_t ioPorts[256] = {0}; 
    
}

void Bus::attachIODevice(IODevice* device) {
    ioDevice = device;
}


uint8_t Bus::read(uint16_t address) {
    return memory.read(address);  
}


void Bus::write(uint16_t address, uint8_t value) {
    memory.write(address, value); 
}


uint8_t Bus::readIO(uint16_t port) {
    if ((port & 1) == 0) {
        uint8_t result = 0xFF; 
    uint8_t keyRow = (port & 0xFF00) >> 8;  
        if (keyRow == 0x7F) {
            result &= KeyMatrix[7];
        } else if (keyRow == 0xBF) {
            result &= KeyMatrix[6];
        } else if (keyRow == 0xDF) {
            result &= KeyMatrix[5];
        } else if (keyRow == 0xEF) {
            result &= KeyMatrix[4];
        } else if (keyRow == 0xF7) {
            result &= KeyMatrix[3];
        } else if (keyRow == 0xFB) {
            result &= KeyMatrix[2];
        } else if (keyRow == 0xFD) {
            result &= KeyMatrix[1];
        } else if (keyRow == 0xFE) {
            result &= KeyMatrix[0];
        }

        result &= 0x1F;  // Mask bits 0 to 4 (to clear higher bits)
        result |= 0b11100000;  // Set bits 5 to 7

        return result;  
    }
    return 0;
}

int Bus::readBorder(uint16_t port) {
    uint8_t lowBytePort = port & 0xFF;  

    if (lowBytePort == 0xFE) {
        return ioPorts[lowBytePort];  
    }
    return 0;
}



void Bus::writeIO(uint16_t port, uint8_t value) {
    uint8_t lowBytePort = port & 0xFF;  
    ioPorts[lowBytePort] = value; 
}



bool Bus::isIOPort(uint16_t address) {
    return (address & 0xFF00) == 0; 
}

std::vector<uint8_t> Bus::readAllBytes(const std::string& filePath) {
    // Open the file in binary mode
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);  
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open ROM file: " + filePath);
    }

    std::streamsize fileSize = file.tellg();  // Get the size of the file
    file.seekg(0, std::ios::beg);  // Rewind to the beginning of the file

    // Read the file into a vector
    std::vector<uint8_t> buffer(fileSize);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
        throw std::runtime_error("Error reading ROM file: " + filePath);
    }

    file.close();  
    return buffer;  
}

void Bus::loadROM(const std::string& filePath) {
    std::vector<uint8_t> romData = readAllBytes(filePath);
    for (size_t i = 0; i < romData.size(); ++i) {
        memory.write(static_cast<uint16_t>(i), romData[i]);
    }
}

