#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <string>
#include <functional>
class z80; // Forward declaration of z80 class

enum class AddressingMode
{
    IMMEDIATE,
    REGISTER,
    REGISTER_INDIRECT,
    IMPLIED,
    RELATIVE,
    INDEXED,
    EXTENTED_IMMEDIATE,

};

class Instruction
{
private:
    std::string mnemonic;
    std::function<void(z80 &, uint8_t)> operation;
    int cycles;

public:
    Instruction()
        : mnemonic("NOP"), operation(nullptr), cycles(4) {}

    Instruction(const std::string &mnemonic,
                std::function<void(z80 &, uint8_t)> operation, int cycles);

    void execute(z80 &cpu, uint8_t opCode);

    std::string getMnemonic() const;
    int getCycles() const;
};

#endif
