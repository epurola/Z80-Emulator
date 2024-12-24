#include "Memory.hpp"
#include <iostream>  

// Constructor to initialize memory with a given size
Memory::Memory(int size) : memorySize(size) {
    memory = new uint8_t[size];  // Dynamically allocate memory array
    std::fill(memory, memory + size, 0);  // Initialize all memory to zero
}

// Destructor to clean up the dynamically allocated memory array
Memory::~Memory() {
    delete[] memory;  // Free the memory
}

// Read a byte from memory
uint8_t Memory::read(int address) {
    address = address & 0xFFFF;  // Wrap address to 16-bit
    return memory[address];  // Return the value at the address
}

// Write a byte to memory
void Memory::write(int address, uint8_t value) {
    address = address & 0xFFFF;  // Wrap address to 16-bit
    memory[address] = value;  // Set the value at the address
}

