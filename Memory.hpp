#ifndef MEMORY_H
#define MEMORY_H

#include <cstdint>  

class Memory {
private:
    uint8_t* memory;  // The memory array
    int memorySize;   // Total size of memory

public:
    // Constructor to initialize memory with a given size
    Memory(int size);

    // Destructor to clean up the memory array
    ~Memory();

    // Read a byte from memory
    uint8_t read(int address);

    // Write a byte to memory
    void write(int address, uint8_t value);
    uint8_t* getMemoryPointer();
};

#endif 
