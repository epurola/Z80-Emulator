#include "bus.hpp"
#include "memory.hpp"
#include "IODevice.hpp"
#include <iostream>  // for std::cout


// Constructor to initialize memory
Bus::Bus(Memory& mem) : memory(mem), ioDevice(nullptr) {}

// Attach an I/O device
void Bus::attachIODevice(IODevice* device) {
    ioDevice = device;
}

// Read from the bus (either memory or I/O)
int Bus::read(uint16_t address) {
    address = address & 0xFFFF;  // Ensure address is 16-bit
    if (ioDevice != nullptr && ioDevice->handlesAddress(address)) {
        return ioDevice->read(address);  // Read from I/O device
    }
    return memory.read(address);  // Default to reading from memory
}

// Write to the bus (either memory or I/O)
void Bus::write(uint16_t address, uint8_t value) {
    address = address & 0xFFFF;  // Ensure address is 16-bit
    if (ioDevice != nullptr && ioDevice->handlesAddress(address)) {
        ioDevice->write(address, value);  // Write to I/O device
    } else {
        memory.write(address, value);  // Default to writing to memory
    }

}

// Read from an I/O port
uint8_t Bus::readIO(uint8_t port) {
    if (ioDevice != nullptr && port == ioPort) {
        return ioDevice->read(port);  // Read from the device at the given port
    }
    std::cerr << "Warning: Unmapped I/O read at port 0x" << std::hex << (int)port << std::endl;
    return 0xFF;  // Default return for unmapped ports
}


// Write to an I/O port
void Bus::writeIO(uint8_t port, uint8_t value) {
    if (ioDevice != nullptr && port == ioPort) {
        ioDevice->write(port, value);  // Write to the device at the given port
    } else {
        std::cerr << "Warning: Unmapped I/O write at port 0x" << std::hex << (int)port 
                  << " with value 0x" << (int)value << std::endl;
    }
}


// Helper function to determine if an address is an I/O port
bool Bus::isIOPort(uint16_t address) {
    return (address & 0xFF00) == 0;  // I/O ports are mapped to the lower byte (0x00–0xFF)
}

std::vector<uint8_t> Bus::readAllBytes(const std::string& filePath) {
    // Open the file in binary mode
    std::ifstream file(filePath, std::ios::binary | std::ios::ate);  // 'ate' moves the pointer to the end of the file
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open ROM file: " + filePath);
    }

    // Get the size of the file
    std::streamsize fileSize = file.tellg();  // Get the size of the file
    file.seekg(0, std::ios::beg);  // Rewind to the beginning of the file

    // Read the file into a vector
    std::vector<uint8_t> buffer(fileSize);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), fileSize)) {
        throw std::runtime_error("Error reading ROM file: " + filePath);
    }

    file.close();  // Close the file
    return buffer;  // Return the vector containing the file data
}

void Bus::loadROM(const std::string& filePath) {
    // Read the ROM file into a buffer
    std::vector<uint8_t> romData = readAllBytes(filePath);

    // Now load the ROM data into the memory, starting from 0x0000
    for (size_t i = 0; i < romData.size(); ++i) {
        memory.write(static_cast<uint16_t>(i), romData[i]);
    }
}

