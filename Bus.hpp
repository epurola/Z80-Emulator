#ifndef BUS_H
#define BUS_H

#include <cstdint>
#include <fstream> 
#include <stdexcept> 
#include <vector> 
#include <string> 


class Memory;
class IODevice;

class Bus {
private:
    Memory& memory;     
    IODevice* ioDevice;  
    uint8_t ioPort; 
    
    
    

public:
bool interrupt = false;
uint8_t KeyMatrix[8];
    
    Bus(Memory& mem);

  
    void attachIODevice(IODevice* device);

   
    uint8_t read(uint16_t address);

  
    void write(uint16_t address, uint8_t value);

    void loadROM(const std::string& filePath);

    std::vector<uint8_t> readAllBytes(const std::string& filePath);

    bool isIOPort(uint16_t address);  
    uint8_t readIO(uint16_t port);     
    int readBorder(uint16_t port);  
    void writeIO(uint16_t port, uint8_t value);  
    uint8_t ioPorts[256] = {0}; 

};

#endif 
