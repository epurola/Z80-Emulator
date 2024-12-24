#include "Instruction.hpp"
#include "main.hpp"

Instruction::Instruction(const std::string &mnemonic,
                         std::function<void(z80 &, uint8_t)> operation, int cycles)
    : mnemonic(mnemonic), operation(operation), cycles(cycles) {}

void Instruction::execute(z80 &cpu, uint8_t opCode)
{
    operation(cpu, opCode);
}

std::string Instruction::getMnemonic() const
{
    return mnemonic;
}

int Instruction::getCycles() const
{
    return cycles;
}
