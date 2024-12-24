#include <iostream>
#include "main.hpp"
#include "Instruction.hpp"

z80::z80()
{
    reset(nullptr); 
}

// Reset registers to default values
void z80::reset(Bus *bus)
{
    A = A1 = 0xFF;
    F = F1= 0xFF;
    B = C = D = E = H = L = 0x00;
    B1 = C1 = D1 = E1 = H1 = L1 = 0x00;
    I = R = 0x00;
    IX = IY = 0x00;
    PC = 0x0000;
    SP= 0xFFFF;

    this->bus = bus;

    initializeInstructionTable();
}

void z80::IncrementRefreshRegister(int steps)
{
    uint8_t refresh = (R & 0b01111111); // Only use bits 0-6
    uint8_t msbSet = (R & 0b10000000) > 0;
    refresh = (refresh + steps);
    R = refresh;

    if (msbSet)
    {
        R |= 0b10000000;
    }
    else
    {
        R &= 0b01111111;
    }
}

// Read from a specific register
uint8_t z80::readFromRegister(uint8_t regIndex)
{
    switch (regIndex)
    {
    case 0:
        return B;
    case 1:
        return C;
    case 2:
        return D;
    case 3:
        return E;
    case 4:
        return H;
    case 5:
        return L;
    case 7:
        return A;
    default:
        return 0x00;
    }
}

// Write to a specific register
void z80::writeToRegister(uint8_t regIndex, uint8_t value)
{
    switch (regIndex)
    {
    case 0:
        B = value;
        break;
    case 1:
        C = value;
        break;
    case 2:
        D = value;
        break;
    case 3:
        E = value;
        break;
    case 4:
        H = value;
        break;
    case 5:
        L = value;
        break;
    case 7:
        A = value;
        break;
    default:
        throw std::invalid_argument("Invalid register index");
    }
}

// Set a flag in F
void z80::setFlag(uint8_t flagMask)
{
    F |= flagMask; // Set the specified flag(s)
}

// Clear (reset) a flag in F
void z80::clearFlag(uint8_t flagMask)
{
    F &= ~flagMask; // Clear the specified flag(s)
}

// Toggle a flag in F
void z80::toggleFlag(uint8_t flagMask)
{
    F ^= flagMask; // Toggle the specified flag(s)
}

bool z80::isFlagSet(uint8_t flagMask) const
{
    return (F & flagMask) != 0; // Check if the flag is set
}

// Initialize the instruction table
void z80::initializeInstructionTable()
{
    // Populate instructionTable for main opcodes
    instructionTable[0x40] = Instruction("LD B, B", &z80::LD_R_R, 4);
    instructionTable[0x41] = Instruction("LD B, C", &z80::LD_R_R, 4);
    instructionTable[0x42] = Instruction("LD B, D", &z80::LD_R_R, 4);
    instructionTable[0x43] = Instruction("LD B, E", &z80::LD_R_R, 4);
    instructionTable[0x44] = Instruction("LD B, H", &z80::LD_R_R, 4);
    instructionTable[0x45] = Instruction("LD B, L", &z80::LD_R_R, 4);
    instructionTable[0x47] = Instruction("LD B, A", &z80::LD_R_R, 4);

    instructionTable[0x48] = Instruction("LD C, B", &z80::LD_R_R, 4);
    instructionTable[0x49] = Instruction("LD C, C", &z80::LD_R_R, 4);
    instructionTable[0x4A] = Instruction("LD C, D", &z80::LD_R_R, 4);
    instructionTable[0x4B] = Instruction("LD C, E", &z80::LD_R_R, 4);
    instructionTable[0x4C] = Instruction("LD C, H", &z80::LD_R_R, 4);
    instructionTable[0x4D] = Instruction("LD C, L", &z80::LD_R_R, 4);
    instructionTable[0x4F] = Instruction("LD C, A", &z80::LD_R_R, 4);

    instructionTable[0x78] = Instruction("LD A, B", &z80::LD_R_R, 4);
    instructionTable[0x79] = Instruction("LD A, C", &z80::LD_R_R, 4);
    instructionTable[0x7A] = Instruction("LD A, D", &z80::LD_R_R, 4);
    instructionTable[0x7B] = Instruction("LD A, E", &z80::LD_R_R, 4);
    instructionTable[0x7C] = Instruction("LD A, H", &z80::LD_R_R, 4);
    instructionTable[0x7D] = Instruction("LD A, L", &z80::LD_R_R, 4);
    instructionTable[0x7F] = Instruction("LD A, A", &z80::LD_R_R, 4);

    instructionTable[0x50] = Instruction("LD D, B", &z80::LD_R_R, 4);
    instructionTable[0x51] = Instruction("LD D, C", &z80::LD_R_R, 4);
    instructionTable[0x52] = Instruction("LD D, D", &z80::LD_R_R, 4);
    instructionTable[0x53] = Instruction("LD D, E", &z80::LD_R_R, 4);
    instructionTable[0x54] = Instruction("LD D, H", &z80::LD_R_R, 4);
    instructionTable[0x55] = Instruction("LD D, L", &z80::LD_R_R, 4);
    instructionTable[0x57] = Instruction("LD D, A", &z80::LD_R_R, 4);

    instructionTable[0x58] = Instruction("LD E, B", &z80::LD_R_R, 4);
    instructionTable[0x59] = Instruction("LD E, C", &z80::LD_R_R, 4);
    instructionTable[0x5A] = Instruction("LD E, D", &z80::LD_R_R, 4);
    instructionTable[0x5B] = Instruction("LD E, E", &z80::LD_R_R, 4);
    instructionTable[0x5C] = Instruction("LD E, H", &z80::LD_R_R, 4);
    instructionTable[0x5D] = Instruction("LD E, L", &z80::LD_R_R, 4);
    instructionTable[0x5F] = Instruction("LD E, A", &z80::LD_R_R, 4);

    instructionTable[0x60] = Instruction("LD H, B", &z80::LD_R_R, 4);
    instructionTable[0x61] = Instruction("LD H, C", &z80::LD_R_R, 4);
    instructionTable[0x62] = Instruction("LD H, D", &z80::LD_R_R, 4);
    instructionTable[0x63] = Instruction("LD H, E", &z80::LD_R_R, 4);
    instructionTable[0x64] = Instruction("LD H, H", &z80::LD_R_R, 4);
    instructionTable[0x65] = Instruction("LD H, L", &z80::LD_R_R, 4);
    instructionTable[0x67] = Instruction("LD H, A", &z80::LD_R_R, 4);

    instructionTable[0x68] = Instruction("LD L, B", &z80::LD_R_R, 4);
    instructionTable[0x69] = Instruction("LD L, C", &z80::LD_R_R, 4);
    instructionTable[0x6A] = Instruction("LD L, D", &z80::LD_R_R, 4);
    instructionTable[0x6B] = Instruction("LD L, E", &z80::LD_R_R, 4);
    instructionTable[0x6C] = Instruction("LD L, H", &z80::LD_R_R, 4);
    instructionTable[0x6D] = Instruction("LD L, L", &z80::LD_R_R, 4);
    instructionTable[0x6F] = Instruction("LD L, A", &z80::LD_R_R, 4);
    // Load immediate values into registers
    instructionTable[0x06] = Instruction("LD B, n", &z80::LD_R_N, 7);
    instructionTable[0x0E] = Instruction("LD C, n", &z80::LD_R_N, 7);
    instructionTable[0x16] = Instruction("LD D, n", &z80::LD_R_N, 7);
    instructionTable[0x1E] = Instruction("LD E, n", &z80::LD_R_N, 7);
    instructionTable[0x26] = Instruction("LD H, n", &z80::LD_R_N, 7);
    instructionTable[0x2E] = Instruction("LD L, n", &z80::LD_R_N, 7);
    instructionTable[0x3E] = Instruction("LD A, n", &z80::LD_R_N, 7);

    instructionTable[0x0A] = Instruction("LD A, BC", &z80::LD_A_BC, 7);
    instructionTable[0x1A] = Instruction("LD A, DE", &z80::LD_A_DE, 7);
    instructionTable[0x12] = Instruction("LD (DE), A", &z80::LD_DE_A, 7);
    instructionTable[0x02] = Instruction("LD (BC), A", &z80::LD_BC_A, 4);
    instructionTable[0x3A] = Instruction("LD A, (nn)", &z80::LD_A_NN, 7);
    instructionTable[0x32] = Instruction("LD (nn), A", &z80::LD_NN_A, 7);

    instructionTable[0x70] = Instruction("LD (HL), B", &z80::LD_HL_R, 4);
    instructionTable[0x71] = Instruction("LD (HL), C", &z80::LD_HL_R, 4);
    instructionTable[0x72] = Instruction("LD (HL), D", &z80::LD_HL_R, 4);
    instructionTable[0x73] = Instruction("LD (HL), E", &z80::LD_HL_R, 4);
    instructionTable[0x74] = Instruction("LD (HL), H", &z80::LD_HL_R, 4);
    instructionTable[0x75] = Instruction("LD (HL), L", &z80::LD_HL_R, 4);
    instructionTable[0x77] = Instruction("LD (HL), A", &z80::LD_HL_R, 4);

    instructionTable[0x21] = Instruction("LD HL, nn", &z80::LD_DD_NN, 10);
    instructionTable[0x2A] = Instruction("LD HL, (nn)", &z80::LD_HL_NN, 10);
    instructionTable[0x22] = Instruction("LD (nn), HL", &z80::LD_NN_HL, 10);

    instructionTable[0x01] = Instruction("LD BC, nn", &z80::LD_DD_NN, 10);
    instructionTable[0x11] = Instruction("LD DE, nn", &z80::LD_DD_NN, 10);
    instructionTable[0x31] = Instruction("LD SP, nn", &z80::LD_DD_NN, 10);

    instructionTable[0x7E] = Instruction("LD A, (HL)", &z80::LD_R_HL, 7);
    instructionTable[0x6E] = Instruction("LD L, (HL)", &z80::LD_R_HL, 7);
    instructionTable[0x66] = Instruction("LD H, (HL)", &z80::LD_R_HL, 7);
    instructionTable[0x5E] = Instruction("LD E, (HL)", &z80::LD_R_HL, 7);
    instructionTable[0x56] = Instruction("LD D, (HL)", &z80::LD_R_HL, 7);
    instructionTable[0x4E] = Instruction("LD C, (HL)", &z80::LD_R_HL, 7);
    instructionTable[0x46] = Instruction("LD B, (HL)", &z80::LD_R_HL, 7);

    instructionTable[0x36] = Instruction("LD (HL), n", &z80::LD_HL_N, 10);

    instructionTable[0xF9] = Instruction("LD SP, HL", &z80::LD_SP_HL, 10);

    instructionTable[0xC5] = Instruction("PUSH BC", &z80::PUSH_QQ, 10);
    instructionTable[0xD5] = Instruction("PUSH DE", &z80::PUSH_QQ, 10);
    instructionTable[0xE5] = Instruction("PUSH HL", &z80::PUSH_QQ, 10);
    instructionTable[0xF5] = Instruction("PUSH AF", &z80::PUSH_QQ, 10);

    instructionTable[0xC1] = Instruction("POP BC", &z80::POP_QQ, 10);
    instructionTable[0xD1] = Instruction("POP DE", &z80::POP_QQ, 10);
    instructionTable[0xE1] = Instruction("POP HL", &z80::POP_QQ, 10);
    instructionTable[0xF1] = Instruction("POP AF", &z80::POP_QQ, 10);
    instructionTable[0xEB] = Instruction("EX DE ,HL", &z80::EX_DE_HL, 10);
    instructionTable[0x08] = Instruction("EX AF ,AF' ", &z80::EX_AF_AF1, 10);
    instructionTable[0xD9] = Instruction("EXX ", &z80::EXX, 10);
    instructionTable[0xE3] = Instruction("EX_(SP), HL ", &z80::EX_SP_HL, 10);

    instructionTable[0x80] = Instruction("ADD A ,B ", &z80::ADD_A_R, 10);
    instructionTable[0x81] = Instruction("ADD A, C ", &z80::ADD_A_R, 10);
    instructionTable[0x82] = Instruction("ADD A, D ", &z80::ADD_A_R, 10);
    instructionTable[0x83] = Instruction("ADD A, E ", &z80::ADD_A_R, 10);
    instructionTable[0x84] = Instruction("ADD A, H ", &z80::ADD_A_R, 10);
    instructionTable[0x85] = Instruction("ADD A, L ", &z80::ADD_A_R, 10);
    instructionTable[0x87] = Instruction("ADD A, A ", &z80::ADD_A_R, 10);

    instructionTable[0xC6] = Instruction("ADD A, n ", &z80::ADD_A_N, 10);

    instructionTable[0x86] = Instruction("ADD A, (HL) ", &z80::ADD_A_HL, 10);

    instructionTable[0x88] = Instruction("ADC A ,B ", &z80::ADC_A_s, 10);
    instructionTable[0x89] = Instruction("ADC A, C ", &z80::ADC_A_s, 10);
    instructionTable[0x8A] = Instruction("ADC A, D ", &z80::ADC_A_s, 10);
    instructionTable[0x8B] = Instruction("ADC A, E ", &z80::ADC_A_s, 10);
    instructionTable[0x8C] = Instruction("ADC A, H ", &z80::ADC_A_s, 10);
    instructionTable[0x8D] = Instruction("ADC A, L ", &z80::ADC_A_s, 10);
    instructionTable[0x8F] = Instruction("ADC A, A ", &z80::ADC_A_s, 10);

    instructionTable[0xCE] = Instruction("ADC A, n ", &z80::ADC_A_N, 10);
    instructionTable[0x8E] = Instruction("ADC A, (HL) ", &z80::ADC_A_HL, 10);

    instructionTable[0x90] = Instruction("SUB A ,B ", &z80::SUB_A_R, 10);
    instructionTable[0x91] = Instruction("SUB A, C ", &z80::SUB_A_R, 10);
    instructionTable[0x92] = Instruction("SUB A, D ", &z80::SUB_A_R, 10);
    instructionTable[0x93] = Instruction("SUB A, E ", &z80::SUB_A_R, 10);
    instructionTable[0x94] = Instruction("SUB A, H ", &z80::SUB_A_R, 10);
    instructionTable[0x95] = Instruction("SUB A, L ", &z80::SUB_A_R, 10);
    instructionTable[0x97] = Instruction("SUB A, A ", &z80::SUB_A_R, 10);

    instructionTable[0x98] = Instruction("SBC A ,B ", &z80::SBC_A_S, 10);
    instructionTable[0x99] = Instruction("SBC A, C ", &z80::SBC_A_S, 10);
    instructionTable[0x9A] = Instruction("SBC A, D ", &z80::SBC_A_S, 10);
    instructionTable[0x9B] = Instruction("SBC A, E ", &z80::SBC_A_S, 10);
    instructionTable[0x9C] = Instruction("SBC A, H ", &z80::SBC_A_S, 10);
    instructionTable[0x9D] = Instruction("SBC A, L ", &z80::SBC_A_S, 10);
    instructionTable[0x9F] = Instruction("SBC A, A ", &z80::SBC_A_S, 10);

    instructionTable[0xA0] = Instruction("AND A ,B ", &z80::AND_A_R, 10);
    instructionTable[0xA1] = Instruction("AND A, C ", &z80::AND_A_R, 10);
    instructionTable[0xA2] = Instruction("AND A, D ", &z80::AND_A_R, 10);
    instructionTable[0xA3] = Instruction("AND A, E ", &z80::AND_A_R, 10);
    instructionTable[0xA4] = Instruction("AND A, H ", &z80::AND_A_R, 10);
    instructionTable[0xA5] = Instruction("AND A, L ", &z80::AND_A_R, 10);
    instructionTable[0xA7] = Instruction("AND A, A ", &z80::AND_A_R, 10);

    instructionTable[0xB0] = Instruction("OR A ,B ", &z80::OR_A_R, 10);
    instructionTable[0xB1] = Instruction("OR A, C ", &z80::OR_A_R, 10);
    instructionTable[0xB2] = Instruction("OR A, D ", &z80::OR_A_R, 10);
    instructionTable[0xB3] = Instruction("OR A, E ", &z80::OR_A_R, 10);
    instructionTable[0xB4] = Instruction("OR A, H ", &z80::OR_A_R, 10);
    instructionTable[0xB5] = Instruction("OR A, L ", &z80::OR_A_R, 10);
    instructionTable[0xB7] = Instruction("OR A, A ", &z80::OR_A_R, 10);

    instructionTable[0xA8] = Instruction("XOR A ,B ", &z80::XOR_A_R, 10);
    instructionTable[0xA9] = Instruction("XOR A, C ", &z80::XOR_A_R, 10);
    instructionTable[0xAA] = Instruction("XOR A, D ", &z80::XOR_A_R, 10);
    instructionTable[0xAB] = Instruction("XOR A, E ", &z80::XOR_A_R, 10);
    instructionTable[0xAC] = Instruction("XOR A, H ", &z80::XOR_A_R, 10);
    instructionTable[0xAD] = Instruction("XOR A, L ", &z80::XOR_A_R, 10);
    instructionTable[0xAF] = Instruction("XOR A, A ", &z80::XOR_A_R, 10);

    instructionTable[0xB8] = Instruction("CP ,B ", &z80::CP_R, 10);
    instructionTable[0xB9] = Instruction("CP, C ", &z80::CP_R, 10);
    instructionTable[0xBA] = Instruction("CP, D ", &z80::CP_R, 10);
    instructionTable[0xBB] = Instruction("CP, E ", &z80::CP_R, 10);
    instructionTable[0xBC] = Instruction("CP, H ", &z80::CP_R, 10);
    instructionTable[0xBD] = Instruction("CP, L ", &z80::CP_R, 10);
    instructionTable[0xBF] = Instruction("CP, A ", &z80::CP_R, 10);

    instructionTable[0x04] = Instruction("INC ,B ", &z80::INC_R, 10);
    instructionTable[0x0C] = Instruction("INC, C ", &z80::INC_R, 10);
    instructionTable[0x14] = Instruction("INC, D ", &z80::INC_R, 10);
    instructionTable[0x1C] = Instruction("INC, E ", &z80::INC_R, 10);
    instructionTable[0x24] = Instruction("INC, H ", &z80::INC_R, 10);
    instructionTable[0x2C] = Instruction("INC, L ", &z80::INC_R, 10);
    instructionTable[0x3C] = Instruction("INC, A ", &z80::INC_R, 10);

    instructionTable[0x05] = Instruction("DEC ,B ", &z80::DEC_R, 10);
    instructionTable[0x0D] = Instruction("DEC, C ", &z80::DEC_R, 10);
    instructionTable[0x15] = Instruction("DEC, D ", &z80::DEC_R, 10);
    instructionTable[0x1D] = Instruction("DEC, E ", &z80::DEC_R, 10);
    instructionTable[0x25] = Instruction("DEC, H ", &z80::DEC_R, 10);
    instructionTable[0x2D] = Instruction("DEC, L ", &z80::DEC_R, 10);
    instructionTable[0x3D] = Instruction("DEC, A ", &z80::DEC_R, 10);

    instructionTable[0xD6] = Instruction("SUB A, n ", &z80::SUB_A_N, 10);
    instructionTable[0xDE] = Instruction("SBC A, n ", &z80::SBC_A_N, 10);
    instructionTable[0xE6] = Instruction("AND A, n ", &z80::AND_A_N, 10);
    instructionTable[0xF6] = Instruction("OR A, n ", &z80::OR_A_N, 10);
    instructionTable[0xEE] = Instruction("XOR A, n ", &z80::XOR_A_N, 10);
    instructionTable[0xFE] = Instruction("CP, n ", &z80::CP_N, 10);

    instructionTable[0x96] = Instruction("SUB A, (HL) ", &z80::SUB_A_HL, 10);
    instructionTable[0x9E] = Instruction("SBC A, (HL) ", &z80::SBC_A_HL, 10);
    instructionTable[0xA6] = Instruction("AND A, (HL) ", &z80::AND_A_HL, 10);
    instructionTable[0xB6] = Instruction("OR A, (HL) ", &z80::OR_A_HL, 10);
    instructionTable[0xAE] = Instruction("XOR A, (HL) ", &z80::XOR_A_HL, 10);
    instructionTable[0xBE] = Instruction("CP, (HL) ", &z80::CP_HL, 10);

    instructionTable[0x34] = Instruction("INC, (HL) ", &z80::INC_HL, 10);
    instructionTable[0x35] = Instruction("DEC, (HL) ", &z80::DEC_HL, 10);
    instructionTable[0x27] = Instruction("DAA ", &z80::DAA, 10);
    instructionTable[0x2F] = Instruction("CPL ", &z80::CPL, 10);
    instructionTable[0x3F] = Instruction("CCF", &z80::CCF, 10);
    instructionTable[0x37] = Instruction("SCF", &z80::SCF, 10);
    instructionTable[0x00] = Instruction("NOP", &z80::NOP, 10);

    instructionTable[0x76] = Instruction("HALT", &z80::HALT, 10);
    instructionTable[0xF3] = Instruction("DI", &z80::DI, 10);
    instructionTable[0xFB] = Instruction("EI", &z80::EI, 10);

    instructionTable[0x09] = Instruction("ADD HL, BC", &z80::ADD_HL_SS, 10);
    instructionTable[0x19] = Instruction("ADD HL, DE", &z80::ADD_HL_SS, 10);
    instructionTable[0x29] = Instruction("ADD HL, HL", &z80::ADD_HL_SS, 10);
    instructionTable[0x39] = Instruction("ADD HL, SP", &z80::ADD_HL_SS, 10);

    instructionTable[0x03] = Instruction("INC BC", &z80::INC_SS, 10);
    instructionTable[0x13] = Instruction("INC DE", &z80::INC_SS, 10);
    instructionTable[0x23] = Instruction("INC HL", &z80::INC_SS, 10);
    instructionTable[0x33] = Instruction("INC SP", &z80::INC_SS, 10);

    instructionTable[0x0B] = Instruction("DEC BC", &z80::DEC_SS, 10);
    instructionTable[0x1B] = Instruction("DEC DE", &z80::DEC_SS, 10);
    instructionTable[0x2B] = Instruction("DEC HL", &z80::DEC_SS, 10);
    instructionTable[0x3B] = Instruction("DEC SP", &z80::DEC_SS, 10);

    instructionTable[0x07] = Instruction("RLCA", &z80::RLCA, 10);
    instructionTable[0x17] = Instruction("RLA", &z80::RLA, 10);
    instructionTable[0x0F] = Instruction("RRCA", &z80::RRCA, 10);
    instructionTable[0x1F] = Instruction("RRA", &z80::RRA, 10);

    instructionTable[0xC3] = Instruction("JP, nn", &z80::JP_NN, 10);

    instructionTable[0xC2] = Instruction("JP NZ, nn", &z80::JP_CC_NN, 10);
    instructionTable[0xCA] = Instruction("JP Z, nn", &z80::JP_CC_NN, 10);
    instructionTable[0xD2] = Instruction("JP NC, nn", &z80::JP_CC_NN, 10);
    instructionTable[0xDA] = Instruction("JP C, nn", &z80::JP_CC_NN, 10);
    instructionTable[0xE2] = Instruction("JP PO, nn", &z80::JP_CC_NN, 10);
    instructionTable[0xEA] = Instruction("JP PE, nn", &z80::JP_CC_NN, 10);
    instructionTable[0xF2] = Instruction("JP P, nn", &z80::JP_CC_NN, 10);
    instructionTable[0xFA] = Instruction("JP M, nn", &z80::JP_CC_NN, 10);

    instructionTable[0x18] = Instruction("JR, e", &z80::JR_E, 10);
    instructionTable[0x38] = Instruction("JR C, e", &z80::JR_C_E, 10);
    instructionTable[0x30] = Instruction("JR NC, e", &z80::JR_NC_E, 10);
    instructionTable[0x28] = Instruction("JR Z, e", &z80::JR_Z_E, 10);
    instructionTable[0x20] = Instruction("JR NZ, e", &z80::JR_NZ_E, 10);

    instructionTable[0xE9] = Instruction("JP (HL)", &z80::JP_HL, 10);

    instructionTable[0x10] = Instruction("DJNZ, e", &z80::DJNZ_E, 10);

    instructionTable[0xCD] = Instruction("CALL, nn", &z80::CALL_NN, 10);

    instructionTable[0xC4] = Instruction("CALL NZ, nn", &z80::CALL_CC_NN, 10);
    instructionTable[0xCC] = Instruction("CALL Z, nn", &z80::CALL_CC_NN, 10);
    instructionTable[0xD4] = Instruction("CALL NC, nn", &z80::CALL_CC_NN, 10);
    instructionTable[0xDC] = Instruction("CALL C, nn", &z80::CALL_CC_NN, 10);
    instructionTable[0xE4] = Instruction("CALL PO, nn", &z80::CALL_CC_NN, 10);
    instructionTable[0xEC] = Instruction("CALL PE, nn", &z80::CALL_CC_NN, 10);
    instructionTable[0xF4] = Instruction("CALL P, nn", &z80::CALL_CC_NN, 10);
    instructionTable[0xFC] = Instruction("CALL M, nn", &z80::CALL_CC_NN, 10);

    instructionTable[0xC9] = Instruction("RET", &z80::RET, 10);
    instructionTable[0xC0] = Instruction("RET, NZ", &z80::RET_CC, 10);
    instructionTable[0xC8] = Instruction("RET, Z", &z80::RET_CC, 10);
    instructionTable[0xD0] = Instruction("RET, NC", &z80::RET_CC, 10);
    instructionTable[0xD8] = Instruction("RET, Z", &z80::RET_CC, 10);
    instructionTable[0xE0] = Instruction("RET, PO", &z80::RET_CC, 10);
    instructionTable[0xE8] = Instruction("RET, PE", &z80::RET_CC, 10);
    instructionTable[0xF0] = Instruction("RET, P", &z80::RET_CC, 10);
    instructionTable[0xF8] = Instruction("RET, M", &z80::RET_CC, 10);

    instructionTable[0xC7] = Instruction("RST &00", &z80::RST_P, 10);
    instructionTable[0xCF] = Instruction("RST &08", &z80::RST_P, 10);
    instructionTable[0xD7] = Instruction("RST &10", &z80::RST_P, 10);
    instructionTable[0xDF] = Instruction("RST &18", &z80::RST_P, 10);
    instructionTable[0xE7] = Instruction("RST &20", &z80::RST_P, 10);
    instructionTable[0xEF] = Instruction("RST &28", &z80::RST_P, 10);
    instructionTable[0xF7] = Instruction("RST &30", &z80::RST_P, 10);
    instructionTable[0xFF] = Instruction("RST &38", &z80::RST_P, 10);

    instructionTable[0xDB] = Instruction("IN A, (n)", &z80::IN_A_N, 10);
    instructionTable[0xD3] = Instruction("OUT (n), A", &z80::OUT_N_A, 10);
    

    // DD-prefixed instructions
   
    instructionTableDD[0xE9] = Instruction("JP (IX)", &z80::JP_IX, 10);
    instructionTableDD[0x46] = Instruction("LD B, (IX+d)", &z80::LD_R_IX_D, 19);
    instructionTableDD[0x4E] = Instruction("LD C, (IX+d)", &z80::LD_R_IX_D, 19);
    instructionTableDD[0x56] = Instruction("LD D, (IX+d)", &z80::LD_R_IX_D, 19);
    instructionTableDD[0x5E] = Instruction("LD E, (IX+d)", &z80::LD_R_IX_D, 19);
    instructionTableDD[0x66] = Instruction("LD H, (IX+d)", &z80::LD_R_IX_D, 19);
    instructionTableDD[0x6E] = Instruction("LD L, (IX+d)", &z80::LD_R_IX_D, 19);
    instructionTableDD[0x7E] = Instruction("LD A, (IX+d)", &z80::LD_R_IX_D, 19);

    instructionTableDD[0x70] = Instruction("LD (IX+d), B", &z80::LD_IX_D_R, 19);
    instructionTableDD[0x71] = Instruction("LD (IX+d), C", &z80::LD_IX_D_R, 19);
    instructionTableDD[0x72] = Instruction("LD (IX+d), D", &z80::LD_IX_D_R, 19);
    instructionTableDD[0x73] = Instruction("LD (IX+d), E", &z80::LD_IX_D_R, 19);
    instructionTableDD[0x74] = Instruction("LD (IX+d), H", &z80::LD_IX_D_R, 19);
    instructionTableDD[0x75] = Instruction("LD (IX+d), L", &z80::LD_IX_D_R, 19);
    instructionTableDD[0x77] = Instruction("LD (IX+d), A", &z80::LD_IX_D_R, 19);

    instructionTableDD[0x36] = Instruction("LD (IX+d), N", &z80::LD_IX_D_N, 19);
    instructionTableDD[0x21] = Instruction("LD IX, nn", &z80::LD_IX_NN, 10);
    instructionTableDD[0x2A] = Instruction("LD IX, (nn)", &z80::LD_IX_NN2, 10);
    instructionTableDD[0x22] = Instruction("LD (nn), IX", &z80::LD_NN_IX, 10);
    instructionTableDD[0x22] = Instruction("LD (nn), IY", &z80::LD_NN_IX, 10);

    instructionTableDD[0x26] = Instruction("LD IXh, n", &z80::LD_IXH_N, 10);
    instructionTableDD[0x2E] = Instruction("LD IXl, n", &z80::LD_IXl_N, 10);

    instructionTableDD[0x44] = Instruction("LD B IXh", &z80::LD_R_IXH, 10);
    instructionTableDD[0x4C] = Instruction("LD C IXh", &z80::LD_R_IXH, 10);
    instructionTableDD[0x54] = Instruction("LD D IXh", &z80::LD_R_IXH, 10);
    instructionTableDD[0x5C] = Instruction("LD E IXh", &z80::LD_R_IXH, 10);

    instructionTableDD[0x60] = Instruction("LD IXh B", &z80::LD_IXH_R, 10);
    instructionTableDD[0x61] = Instruction("LD IXh C", &z80::LD_IXH_R, 10);
    instructionTableDD[0x62] = Instruction("LD IXh D", &z80::LD_IXH_R, 10);
    instructionTableDD[0x63] = Instruction("LD IXh E", &z80::LD_IXH_R, 10);
    instructionTableDD[0x67] = Instruction("LD IXh A", &z80::LD_IXH_R, 10);

    instructionTableDD[0x68] = Instruction("LD IXl B", &z80::LD_IXl_R, 10);
    instructionTableDD[0x69] = Instruction("LD IXl C", &z80::LD_IXl_R, 10);
    instructionTableDD[0x6A] = Instruction("LD IXl D", &z80::LD_IXl_R, 10);
    instructionTableDD[0x6B] = Instruction("LD IXl E", &z80::LD_IXl_R, 10);
    instructionTableDD[0x6F] = Instruction("LD IXl A", &z80::LD_IXl_R, 10);

    instructionTableDD[0x65] = Instruction("LD IXh IXl", &z80::LD_IXH_IXL, 10);
    instructionTableDD[0x6C] = Instruction("LD IXl IXh", &z80::LD_IXL_IXH, 10);

    instructionTableDD[0x45] = Instruction("LD B IXl", &z80::LD_R_IXl, 10);
    instructionTableDD[0x4D] = Instruction("LD C IXl", &z80::LD_R_IXl, 10);
    instructionTableDD[0x55] = Instruction("LD D IXl", &z80::LD_R_IXl, 10);
    instructionTableDD[0x5D] = Instruction("LD E IXl", &z80::LD_R_IXl, 10);
    instructionTableDD[0x7D] = Instruction("LD A IXl", &z80::LD_R_IXl, 10);

    instructionTableDD[0x7C] = Instruction("LD A IXh", &z80::LD_R_IXH, 10);

    instructionTableDD[0x84] = Instruction("ADD A IXh", &z80::ADD_A_IX_H, 10);
    instructionTableDD[0x85] = Instruction("ADD A IXl", &z80::ADD_A_IX_L, 10);

    instructionTableDD[0x94] = Instruction("SUB A IXh", &z80::SUB_A_IX_H, 10);
    instructionTableDD[0x95] = Instruction("SUB A IXl", &z80::SUB_A_IX_L, 10);

    instructionTableDD[0x9C] = Instruction("SBC A IXh", &z80::SBC_A_IX_H, 10);
    instructionTableDD[0x9D] = Instruction("SBC A IXl", &z80::SBC_A_IX_L, 10);

    instructionTableDD[0x8C] = Instruction("ADC A IXh", &z80::ADC_A_IX_H, 10);
    instructionTableDD[0x8D] = Instruction("ADC A IXl", &z80::ADC_A_IX_L, 10);

    instructionTableDD[0xA4] = Instruction("AND A IXh", &z80::AND_A_IX_H, 10);
    instructionTableDD[0xA5] = Instruction("AND A IXl", &z80::AND_A_IX_L, 10);

    instructionTableDD[0xAC] = Instruction("XOR A IXh", &z80::XOR_A_IX_H, 10);
    instructionTableDD[0xAD] = Instruction("XOR A IXl", &z80::XOR_A_IX_L, 10);

    instructionTableDD[0xB4] = Instruction("OR A IXh", &z80::OR_A_IX_H, 10);
    instructionTableDD[0xB5] = Instruction("OR A IXl", &z80::OR_A_IX_L, 10);

    instructionTableDD[0xBC] = Instruction("CP A IXh", &z80::CP_IX_H, 10);
    instructionTableDD[0xBD] = Instruction("CP A IXl", &z80::CP_IX_L, 10);

    instructionTableDD[0xF9] = Instruction("LD SP, IX", &z80::LD_SP_IX, 10);
    instructionTableDD[0xE5] = Instruction(" PUSH IX", &z80::PUSH_IX, 10);
    instructionTableDD[0xE1] = Instruction(" POP IX", &z80::POP_IX, 10);
    instructionTableDD[0xE3] = Instruction(" EX (SP), IX", &z80::EX_SP_IX, 10);
    instructionTableDD[0x86] = Instruction("ADD A,(IX+d)", &z80::ADD_A_IX_D, 10);

    instructionTableDD[0x8E] = Instruction("ADC A,(IX+d)", &z80::ADC_A_IX_D, 10);
    instructionTableDD[0x96] = Instruction("SUB A,(IX+d)", &z80::SUB_A_IX_D, 10);
    instructionTableDD[0x9E] = Instruction("SBC A,(IX+d)", &z80::SBC_A_IX_D, 10);
    instructionTableDD[0xA6] = Instruction("AND A,(IX+d)", &z80::AND_A_IX_D, 10);
    instructionTableDD[0xB6] = Instruction("OR A,(IX+d)", &z80::OR_A_IX_D, 10);
    instructionTableDD[0xAE] = Instruction("XOR A,(IX+d)", &z80::XOR_A_IX_D, 10);
    instructionTableDD[0xBE] = Instruction("CP,(IX+d)", &z80::CP_IX_D, 10);
    instructionTableDD[0x34] = Instruction("INC,(IX+d)", &z80::INC_IX_D, 10);
    instructionTableDD[0x35] = Instruction("DEC,(IX+d)", &z80::DEC_IX_D, 10);

    instructionTableDD[0x09] = Instruction("ADD IX, BC", &z80::ADD_IX_PP, 10);
    instructionTableDD[0x19] = Instruction("ADD IX, DE", &z80::ADD_IX_PP, 10);
    instructionTableDD[0x29] = Instruction("ADD IX, IX", &z80::ADD_IX_PP, 10);
    instructionTableDD[0x39] = Instruction("ADD IX, SP", &z80::ADD_IX_PP, 10);

    instructionTableDD[0x23] = Instruction("INC IX", &z80::INC_IX, 10);
    instructionTableDD[0x24] = Instruction("INC IXh", &z80::INC_IXH, 10);
    instructionTableDD[0x2C] = Instruction("INC IXl", &z80::INC_IXl, 10);
    instructionTableDD[0x2B] = Instruction("DEC IX", &z80::DEC_IX, 10);
    instructionTableDD[0x25] = Instruction("DEC IXH", &z80::DEC_IXH, 10);
    instructionTableDD[0x2D] = Instruction("DEC IXH", &z80::DEC_IXl, 10);
    instructionTableDD[0xFD] = Instruction("NOP", &z80::NOP, 10);

    // FD-prefixed instructions
    
    instructionTableFD[0x46] = Instruction("LD B, (IY+d)", &z80::LD_R_IY_D, 19);
    instructionTableFD[0x4E] = Instruction("LD C, (IY+d)", &z80::LD_R_IY_D, 19);
    instructionTableFD[0x56] = Instruction("LD D, (IY+d)", &z80::LD_R_IY_D, 19);
    instructionTableFD[0x5E] = Instruction("LD E, (IY+d)", &z80::LD_R_IY_D, 19);
    instructionTableFD[0x66] = Instruction("LD H, (IY+d)", &z80::LD_R_IY_D, 19);
    instructionTableFD[0x6E] = Instruction("LD L, (IY+d)", &z80::LD_R_IY_D, 19);
    instructionTableFD[0x7E] = Instruction("LD A, (IY+d)", &z80::LD_R_IY_D, 19);

    instructionTableFD[0xE9] = Instruction("JP (IY)", &z80::JP_IY, 10);

    instructionTableFD[0x70] = Instruction("LD (IY+d), B", &z80::LD_IY_D_R, 19);
    instructionTableFD[0x71] = Instruction("LD (IY+d), C", &z80::LD_IY_D_R, 19);
    instructionTableFD[0x72] = Instruction("LD (IY+d), D", &z80::LD_IY_D_R, 19);
    instructionTableFD[0x73] = Instruction("LD (IY+d), E", &z80::LD_IY_D_R, 19);
    instructionTableFD[0x74] = Instruction("LD (IY+d), H", &z80::LD_IY_D_R, 19);
    instructionTableFD[0x75] = Instruction("LD (IY+d), L", &z80::LD_IY_D_R, 19);
    instructionTableFD[0x77] = Instruction("LD (IY+d), A", &z80::LD_IY_D_R, 19);

    instructionTableFD[0x36] = Instruction("LD (IY+d), N", &z80::LD_IY_D_N, 19);

    instructionTableFD[0x21] = Instruction("LD IY, nn", &z80::LD_IY_NN, 10);
    instructionTableFD[0x2A] = Instruction("LD IY, (nn)", &z80::LD_IY_NN2, 10);
    instructionTableFD[0x22] = Instruction("LD (nn), IY", &z80::LD_NN_IY, 10);
    instructionTableFD[0xF9] = Instruction("LD SP, IY", &z80::LD_SP_IY, 10);

    instructionTableFD[0xE5] = Instruction(" PUSH IY", &z80::PUSH_IY, 10);
    instructionTableFD[0xE1] = Instruction(" POP IY", &z80::POP_IY, 10);
    instructionTableFD[0xE3] = Instruction(" EX (SP), IX", &z80::EX_SP_IY, 10);
    instructionTableFD[0x86] = Instruction("ADD A,(IY+d)", &z80::ADD_A_IY_D, 10);

    instructionTableFD[0x8E] = Instruction("ADC A,(IY+d)", &z80::ADC_A_IY_D, 10);
    instructionTableFD[0x96] = Instruction("SUB A,(IY+d)", &z80::SUB_A_IY_D, 10);
    instructionTableFD[0x9E] = Instruction("SBC A,(IY+d)", &z80::SBC_A_IY_D, 10);

    instructionTableFD[0xA6] = Instruction("AND A,(IY+d)", &z80::AND_A_IY_D, 10);
    instructionTableFD[0xB6] = Instruction("OR A,(IY+d)", &z80::OR_A_IY_D, 10);
    instructionTableFD[0xAE] = Instruction("OR A,(IY+d)", &z80::XOR_A_IY_D, 10);
    instructionTableFD[0xBE] = Instruction("CP,(IY+d)", &z80::CP_IY_D, 10);

    instructionTableFD[0x34] = Instruction("INC,(IY+d)", &z80::INC_IY_D, 10);
    instructionTableFD[0x35] = Instruction("DEC,(IY+d)", &z80::DEC_IY_D, 10);

    instructionTableFD[0x09] = Instruction("ADD IY, BC", &z80::ADD_IY_RR, 10);
    instructionTableFD[0x19] = Instruction("ADD IY, DE", &z80::ADD_IY_RR, 10);
    instructionTableFD[0x29] = Instruction("ADD IY, IX", &z80::ADD_IY_RR, 10);
    instructionTableFD[0x39] = Instruction("ADD IY, SP", &z80::ADD_IY_RR, 10);

    instructionTableFD[0x23] = Instruction("INC IY", &z80::INC_IY, 10);
    instructionTableFD[0x2B] = Instruction("DEC IY", &z80::DEC_IY, 10);

    instructionTableFD[0x24] = Instruction("INC IY H", &z80::INC_IY_H, 10);
    instructionTableFD[0x2C] = Instruction("INC IY L", &z80::INC_IY_L, 10);

    instructionTableFD[0x25] = Instruction("DEC IY H", &z80::DEC_IY_H, 10);
    instructionTableFD[0x2D] = Instruction("DEC IY L", &z80::DEC_IY_L, 10);

    instructionTableFD[0x26] = Instruction("LD IY H, n", &z80::LD_IY_H_N, 10);
    instructionTableFD[0x2E] = Instruction("LD IY L, n", &z80::LD_IY_L_N, 10);

    instructionTableFD[0x44] = Instruction("LD B IYh", &z80::LD_R_IY_H, 10);
    instructionTableFD[0x4C] = Instruction("LD C IYh", &z80::LD_R_IY_H, 10);
    instructionTableFD[0x54] = Instruction("LD D IYh", &z80::LD_R_IY_H, 10);
    instructionTableFD[0x5C] = Instruction("LD E IYh", &z80::LD_R_IY_H, 10);

    instructionTableFD[0x45] = Instruction("LD B IYl", &z80::LD_R_IY_L, 10);
    instructionTableFD[0x4D] = Instruction("LD C IYl", &z80::LD_R_IY_L, 10);
    instructionTableFD[0x55] = Instruction("LD D IYl", &z80::LD_R_IY_L, 10);
    instructionTableFD[0x5D] = Instruction("LD E IYl", &z80::LD_R_IY_L, 10);

    instructionTableFD[0x60] = Instruction("LD IYh B", &z80::LD_IY_H_R, 10);
    instructionTableFD[0x61] = Instruction("LD IYh C", &z80::LD_IY_H_R, 10);
    instructionTableFD[0x62] = Instruction("LD IYh D", &z80::LD_IY_H_R, 10);
    instructionTableFD[0x63] = Instruction("LD IYh E", &z80::LD_IY_H_R, 10);
    instructionTableFD[0x67] = Instruction("LD IYh A", &z80::LD_IY_H_R, 10);

    instructionTableFD[0x68] = Instruction("LD IYl B", &z80::LD_IY_L_R, 10);
    instructionTableFD[0x69] = Instruction("LD IYl C", &z80::LD_IY_L_R, 10);
    instructionTableFD[0x6A] = Instruction("LD IYl D", &z80::LD_IY_L_R, 10);
    instructionTableFD[0x6B] = Instruction("LD IYl E", &z80::LD_IY_L_R, 10);
    instructionTableFD[0x6F] = Instruction("LD IYl A", &z80::LD_IY_L_R, 10);

    instructionTableFD[0x65] = Instruction("LD IYH IYL", &z80::LD_IYH_IYL, 10);
    instructionTableFD[0x6C] = Instruction("LD IYL IYH", &z80::LD_IYL_IYH, 10);

    instructionTableFD[0x7c] = Instruction("LD A IYH", &z80::LD_R_IY_H, 10);
    instructionTableFD[0x7D] = Instruction("LD A IYL ", &z80::LD_R_IY_L, 10);

    instructionTableFD[0x84] = Instruction("ADD A IYH", &z80::ADD_A_IY_H, 10);
    instructionTableFD[0x85] = Instruction("ADD A IYL ", &z80::ADD_A_IY_L, 10);

    instructionTableFD[0x8C] = Instruction("ADC A IYH", &z80::ADC_A_IY_H, 10);
    instructionTableFD[0x8D] = Instruction("ADC A IYL ", &z80::ADC_A_IY_L, 10);

    instructionTableFD[0x9C] = Instruction("SBC A IYH", &z80::SBC_A_IY_H, 10);
    instructionTableFD[0x9D] = Instruction("SBC A IYL ", &z80::SBC_A_IY_L, 10);

    instructionTableFD[0x94] = Instruction("SUB A IYH", &z80::SUB_A_IY_H, 10);
    instructionTableFD[0x95] = Instruction("SUB A IYL ", &z80::SUB_A_IY_L, 10);

    instructionTableFD[0xA4] = Instruction("AND A IYH", &z80::AND_A_IY_H, 10);
    instructionTableFD[0xA5] = Instruction("AND A IYL ", &z80::AND_A_IY_L, 10);

    instructionTableFD[0xAC] = Instruction("XOR A IYH", &z80::XOR_A_IY_H, 10);
    instructionTableFD[0xAD] = Instruction("XOR A IYL ", &z80::XOR_A_IY_L, 10);

    instructionTableFD[0xB4] = Instruction("OR A IYH", &z80::OR_A_IY_H, 10);
    instructionTableFD[0xB5] = Instruction("OR A IYL ", &z80::OR_A_IY_L, 10);

    instructionTableFD[0xBC] = Instruction("CP A IYH", &z80::CP_A_IY_H, 10);
    instructionTableFD[0xBD] = Instruction("CP A IYL ", &z80::CP_A_IY_L, 10);

    // ED Instructions
    instructionTableED[0x57] = Instruction("LD A, I", &z80::LD_A_I, 9);
    instructionTableED[0x5F] = Instruction("LD A, R", &z80::LD_A_R, 9);

    instructionTableED[0x47] = Instruction("LD I, A", &z80::LD_I_A, 9);
    instructionTableED[0x4F] = Instruction("LD R, A", &z80::LD_R_A, 9);

    instructionTableED[0x6B] = Instruction("LD HL, (nn)", &z80::LD_DD_nn, 20);
    instructionTableED[0x4B] = Instruction("LD BC, (nn)", &z80::LD_DD_nn, 20);
    instructionTableED[0x5B] = Instruction("LD DE, (nn)", &z80::LD_DD_nn, 20);
    instructionTableED[0x7B] = Instruction("LD SP, (nn)", &z80::LD_DD_nn, 20);

    instructionTableED[0x73] = Instruction("LD (nn), SP", &z80::LD_NN_DD, 20);
    instructionTableED[0x43] = Instruction("LD (nn), BC", &z80::LD_NN_DD, 20);
    instructionTableED[0x53] = Instruction("LD (nn), DE", &z80::LD_NN_DD, 20);
    instructionTableED[0x63] = Instruction("LD (nn), HL", &z80::LD_NN_DD, 20);

    instructionTableED[0xA0] = Instruction("LDI", &z80::LDI, 20);
    instructionTableED[0xB0] = Instruction("LDIR", &z80::LDIR, 20);
    instructionTableED[0xA8] = Instruction("LDD", &z80::LDD, 20);
    instructionTableED[0xB8] = Instruction("LDDR", &z80::LDDR, 20);
    instructionTableED[0xA1] = Instruction("CPI", &z80::CPI, 20);
    instructionTableED[0xB1] = Instruction("CPIR", &z80::CPIR, 20);
    instructionTableED[0xA9] = Instruction("CPD", &z80::CPD, 20);
    instructionTableED[0xB9] = Instruction("CPDR", &z80::CPDR, 20);
    instructionTableED[0x44] = Instruction("NEG ", &z80::NEG, 10);

    instructionTableED[0x4C] = Instruction("NEG ", &z80::NEG, 10);

    instructionTableED[0x54] = Instruction("NEG ", &z80::NEG, 10);
    instructionTableED[0x5C] = Instruction("NEG ", &z80::NEG, 10);
    instructionTableED[0x64] = Instruction("NEG ", &z80::NEG, 10);
    instructionTableED[0x6C] = Instruction("NEG ", &z80::NEG, 10);
    instructionTableED[0x74] = Instruction("NEG ", &z80::NEG, 10);
    instructionTableED[0x7C] = Instruction("NEG ", &z80::NEG, 10);

    instructionTableED[0x4E] = Instruction("IM0 ", &z80::IM0, 10);
    instructionTableED[0x46] = Instruction("IM0 ", &z80::IM0, 10);
    instructionTableED[0x66] = Instruction("IM0 ", &z80::IM0, 10);
    instructionTableED[0x6E] = Instruction("IM0 ", &z80::IM0, 10);
    instructionTableED[0x56] = Instruction("IM1 ", &z80::IM1, 10);
    instructionTableED[0x5E] = Instruction("IM2 ", &z80::IM2, 10);
    instructionTableED[0x7E] = Instruction("IM2 ", &z80::IM2, 10);

    instructionTableED[0x4A] = Instruction("ADC HL, BC", &z80::ADC_HL_SS, 10);
    instructionTableED[0x5A] = Instruction("ADC HL, DE", &z80::ADC_HL_SS, 10);
    instructionTableED[0x6A] = Instruction("ADC HL, HL", &z80::ADC_HL_SS, 10);
    instructionTableED[0x7A] = Instruction("ADC HL, SP", &z80::ADC_HL_SS, 10);

    instructionTableED[0x42] = Instruction("SBC HL, BC", &z80::SBC_HL_SS, 10);
    instructionTableED[0x52] = Instruction("SBC HL, DE", &z80::SBC_HL_SS, 10);
    instructionTableED[0x62] = Instruction("SBC HL, HL", &z80::SBC_HL_SS, 10);
    instructionTableED[0x72] = Instruction("SBC HL, SP", &z80::SBC_HL_SS, 10);

    instructionTableED[0x6F] = Instruction("RLD", &z80::RLD, 10);
    instructionTableED[0x67] = Instruction("RRD", &z80::RRD, 10);

    instructionTableED[0x4D] = Instruction("RETI", &z80::RETI, 10);
    instructionTableED[0x45] = Instruction("RETN", &z80::RETN, 10);
    instructionTableED[0x55] = Instruction("RETN", &z80::RETN, 10);
    instructionTableED[0x5D] = Instruction("RETN", &z80::RETN, 10);
    instructionTableED[0x65] = Instruction("RETN", &z80::RETN, 10);
    instructionTableED[0x6D] = Instruction("RETN", &z80::RETN, 10);
    instructionTableED[0x75] = Instruction("RETN", &z80::RETN, 10);
    instructionTableED[0x7D] = Instruction("RETN", &z80::RETN, 10);

    instructionTableED[0x40] = Instruction("IN B, (C)", &z80::IN_R_C, 10);
    instructionTableED[0x48] = Instruction("IN C, (C)", &z80::IN_R_C, 10);
    instructionTableED[0x50] = Instruction("IN D, (C)", &z80::IN_R_C, 10);
    instructionTableED[0x58] = Instruction("IN E, (C)", &z80::IN_R_C, 10);
    instructionTableED[0x60] = Instruction("IN H, (C)", &z80::IN_R_C, 10);
    instructionTableED[0x68] = Instruction("IN L, (C)", &z80::IN_R_C, 10);
    instructionTableED[0x70] = Instruction("IN F, (C)", &z80::IN_R_C, 10);
    instructionTableED[0x78] = Instruction("IN A, (C)", &z80::IN_R_C, 10);

    instructionTableED[0x41] = Instruction("OUT (C), B", &z80::OUT_C_R, 10);
    instructionTableED[0x49] = Instruction("OUT (C), C", &z80::OUT_C_R, 10);
    instructionTableED[0x51] = Instruction("OUT (C), D", &z80::OUT_C_R, 10);
    instructionTableED[0x59] = Instruction("OUT (C), E", &z80::OUT_C_R, 10);
    instructionTableED[0x61] = Instruction("OUT (C), H", &z80::OUT_C_R, 10);
    instructionTableED[0x69] = Instruction("OUT (C), L", &z80::OUT_C_R, 10);
    instructionTableED[0x71] = Instruction("OUT (C), F", &z80::OUT_C_R, 10);
    instructionTableED[0x79] = Instruction("OUT (C), A", &z80::OUT_C_R, 10);

    instructionTableED[0xA2] = Instruction("INI", &z80::INI, 10);
    instructionTableED[0xB2] = Instruction("INIR", &z80::INIR, 10);
    instructionTableED[0xAA] = Instruction("IND", &z80::IND, 10);
    instructionTableED[0xBA] = Instruction("INDR", &z80::INDR, 10);

    instructionTableED[0xA3] = Instruction("OUTI", &z80::OUTI, 10);
    instructionTableED[0xB3] = Instruction("OTIR", &z80::OTIR, 10);
    instructionTableED[0xAB] = Instruction("OUTD", &z80::OUTD, 10);
    instructionTableED[0xBB] = Instruction("OTDR", &z80::OTDR, 10);
    instructionTableED[0x76] = Instruction("IM1", &z80::IM1, 10);

    instructionTableCB[0x00] = Instruction("RLC, B", &z80::RLC_R, 10);
    instructionTableCB[0x01] = Instruction("RLC, C", &z80::RLC_R, 10);
    instructionTableCB[0x02] = Instruction("RLC, D", &z80::RLC_R, 10);
    instructionTableCB[0x03] = Instruction("RLC, E", &z80::RLC_R, 10);
    instructionTableCB[0x04] = Instruction("RLC, H", &z80::RLC_R, 10);
    instructionTableCB[0x05] = Instruction("RLC, L", &z80::RLC_R, 10);
    instructionTableCB[0x07] = Instruction("RLC, A", &z80::RLC_R, 10);
    instructionTableCB[0x06] = Instruction("RLC, HL", &z80::RLC_HL, 10);

    instructionTableCB[0x10] = Instruction("RL, B", &z80::RL_R, 10);
    instructionTableCB[0x11] = Instruction("RL, C", &z80::RL_R, 10);
    instructionTableCB[0x12] = Instruction("RL, D", &z80::RL_R, 10);
    instructionTableCB[0x13] = Instruction("RL, E", &z80::RL_R, 10);
    instructionTableCB[0x14] = Instruction("RL, H", &z80::RL_R, 10);
    instructionTableCB[0x15] = Instruction("RL, L", &z80::RL_R, 10);
    instructionTableCB[0x17] = Instruction("RL, A", &z80::RL_R, 10);
    instructionTableCB[0x16] = Instruction("RL, (HL)", &z80::RL_HL, 10);

    instructionTableCB[0x18] = Instruction("RR, B", &z80::RR_R, 10);
    instructionTableCB[0x19] = Instruction("RR, C", &z80::RR_R, 10);
    instructionTableCB[0x1A] = Instruction("RR, D", &z80::RR_R, 10);
    instructionTableCB[0x1B] = Instruction("RR, E", &z80::RR_R, 10);
    instructionTableCB[0x1C] = Instruction("RR, H", &z80::RR_R, 10);
    instructionTableCB[0x1D] = Instruction("RR, L", &z80::RR_R, 10);
    instructionTableCB[0x1F] = Instruction("RR, A", &z80::RR_R, 10);
    instructionTableCB[0x1E] = Instruction("RR, (HL)", &z80::RR_HL, 10);

    instructionTableCB[0x08] = Instruction("RRC, B", &z80::RRC_R, 10);
    instructionTableCB[0x09] = Instruction("RRC, C", &z80::RRC_R, 10);
    instructionTableCB[0x0A] = Instruction("RRC, D", &z80::RRC_R, 10);
    instructionTableCB[0x0B] = Instruction("RRC, E", &z80::RRC_R, 10);
    instructionTableCB[0x0C] = Instruction("RRC, H", &z80::RRC_R, 10);
    instructionTableCB[0x0D] = Instruction("RRC, L", &z80::RRC_R, 10);
    instructionTableCB[0x0F] = Instruction("RRC, A", &z80::RRC_R, 10);
    instructionTableCB[0x0E] = Instruction("RRC, (HL)", &z80::RRC_HL, 10);

    instructionTableCB[0x20] = Instruction("SLA, B", &z80::SLA_R, 10);
    instructionTableCB[0x21] = Instruction("SLA, C", &z80::SLA_R, 10);
    instructionTableCB[0x22] = Instruction("SLA, D", &z80::SLA_R, 10);
    instructionTableCB[0x23] = Instruction("SLA, E", &z80::SLA_R, 10);
    instructionTableCB[0x24] = Instruction("SLA, H", &z80::SLA_R, 10);
    instructionTableCB[0x25] = Instruction("SLA, L", &z80::SLA_R, 10);
    instructionTableCB[0x27] = Instruction("SLA, A", &z80::SLA_R, 10);
    instructionTableCB[0x26] = Instruction("SLA, (HL)", &z80::SLA_HL, 10);

    instructionTableCB[0x30] = Instruction("SLS, B", &z80::SLS_R, 10);
    instructionTableCB[0x31] = Instruction("SLS, C", &z80::SLS_R, 10);
    instructionTableCB[0x32] = Instruction("SLS, D", &z80::SLS_R, 10);
    instructionTableCB[0x33] = Instruction("SLS, E", &z80::SLS_R, 10);
    instructionTableCB[0x34] = Instruction("SLS, H", &z80::SLS_R, 10);
    instructionTableCB[0x35] = Instruction("SLS, L", &z80::SLS_R, 10);
    instructionTableCB[0x37] = Instruction("SLS, A", &z80::SLS_R, 10);
    instructionTableCB[0x36] = Instruction("SLS, (HL)", &z80::SLS_HL, 10);

    instructionTableCB[0x28] = Instruction("SRA, B", &z80::SRA_R, 10);
    instructionTableCB[0x29] = Instruction("SRA, C", &z80::SRA_R, 10);
    instructionTableCB[0x2A] = Instruction("SRA, D", &z80::SRA_R, 10);
    instructionTableCB[0x2B] = Instruction("SRA, E", &z80::SRA_R, 10);
    instructionTableCB[0x2C] = Instruction("SRA, H", &z80::SRA_R, 10);
    instructionTableCB[0x2D] = Instruction("SRA, L", &z80::SRA_R, 10);
    instructionTableCB[0x2F] = Instruction("SRA, A", &z80::SRA_R, 10);
    instructionTableCB[0x2E] = Instruction("SRA, (HL)", &z80::SRA_HL, 10);

    instructionTableCB[0x38] = Instruction("SRL, B", &z80::SRL_R, 10);
    instructionTableCB[0x39] = Instruction("SRL, C", &z80::SRL_R, 10);
    instructionTableCB[0x3A] = Instruction("SRL, D", &z80::SRL_R, 10);
    instructionTableCB[0x3B] = Instruction("SRL, E", &z80::SRL_R, 10);
    instructionTableCB[0x3C] = Instruction("SRL, H", &z80::SRL_R, 10);
    instructionTableCB[0x3D] = Instruction("SRL, L", &z80::SRL_R, 10);
    instructionTableCB[0x3F] = Instruction("SRL, A", &z80::SRL_R, 10);
    instructionTableCB[0x3E] = Instruction("SRL, (HL)", &z80::SRL_HL, 10);

    instructionTableCB[0x40] = Instruction("BIT 0, B", &z80::BIT_B_R, 10);
    instructionTableCB[0x41] = Instruction("BIT 0, C", &z80::BIT_B_R, 10);
    instructionTableCB[0x42] = Instruction("BIT 0, D", &z80::BIT_B_R, 10);
    instructionTableCB[0x43] = Instruction("BIT 0, E", &z80::BIT_B_R, 10);
    instructionTableCB[0x44] = Instruction("BIT 0, H", &z80::BIT_B_R, 10);
    instructionTableCB[0x45] = Instruction("BIT 0, L", &z80::BIT_B_R, 10);
    instructionTableCB[0x47] = Instruction("BIT 0, A", &z80::BIT_B_R, 10);
    instructionTableCB[0x46] = Instruction("BIT 0, (HL)", &z80::BIT_B_HL, 10);

    instructionTableCB[0xC0] = Instruction("SET 0, B", &z80::SET_B_R, 10);
    instructionTableCB[0xC1] = Instruction("SET 0, C", &z80::SET_B_R, 10);
    instructionTableCB[0xC2] = Instruction("SET 0, D", &z80::SET_B_R, 10);
    instructionTableCB[0xC3] = Instruction("SET 0, E", &z80::SET_B_R, 10);
    instructionTableCB[0xC4] = Instruction("SET 0, H", &z80::SET_B_R, 10);
    instructionTableCB[0xC5] = Instruction("SET 0, L", &z80::SET_B_R, 10);
    instructionTableCB[0xC7] = Instruction("SET 0, A", &z80::SET_B_R, 10);
    instructionTableCB[0xC6] = Instruction("SET 0, (HL)", &z80::SET_B_HL, 10);

    instructionTableCB[0x80] = Instruction("RES 0, B", &z80::RES_B_R, 10);
    instructionTableCB[0x81] = Instruction("RES 0, C", &z80::RES_B_R, 10);
    instructionTableCB[0x82] = Instruction("RES 0, D", &z80::RES_B_R, 10);
    instructionTableCB[0x83] = Instruction("RES 0, E", &z80::RES_B_R, 10);
    instructionTableCB[0x84] = Instruction("RES 0, H", &z80::RES_B_R, 10);
    instructionTableCB[0x85] = Instruction("RES 0, L", &z80::RES_B_R, 10);
    instructionTableCB[0x87] = Instruction("RES 0, A", &z80::RES_B_R, 10);
    instructionTableCB[0x86] = Instruction("RES 0, (HL)", &z80::RES_B_HL, 10);

    instructionTableCB[0x48] = Instruction("BIT 1, B", &z80::BIT_B_R, 10);
    instructionTableCB[0x49] = Instruction("BIT 1, C", &z80::BIT_B_R, 10);
    instructionTableCB[0x4A] = Instruction("BIT 1, D", &z80::BIT_B_R, 10);
    instructionTableCB[0x4B] = Instruction("BIT 1, E", &z80::BIT_B_R, 10);
    instructionTableCB[0x4C] = Instruction("BIT 1, H", &z80::BIT_B_R, 10);
    instructionTableCB[0x4D] = Instruction("BIT 1, L", &z80::BIT_B_R, 10);
    instructionTableCB[0x4F] = Instruction("BIT 1, A", &z80::BIT_B_R, 10);
    instructionTableCB[0x4E] = Instruction("BIT 1, (HL)", &z80::BIT_B_HL, 10);

    instructionTableCB[0xC8] = Instruction("SET 1, B", &z80::SET_B_R, 10);
    instructionTableCB[0xC9] = Instruction("SET 1, C", &z80::SET_B_R, 10);
    instructionTableCB[0xCA] = Instruction("SET 1, D", &z80::SET_B_R, 10);
    instructionTableCB[0xCB] = Instruction("SET 1, E", &z80::SET_B_R, 10);
    instructionTableCB[0xCC] = Instruction("SET 1, H", &z80::SET_B_R, 10);
    instructionTableCB[0xCD] = Instruction("SET 1, L", &z80::SET_B_R, 10);
    instructionTableCB[0xCF] = Instruction("SET 1, A", &z80::SET_B_R, 10);
    instructionTableCB[0xCE] = Instruction("SET 1, (HL)", &z80::SET_B_HL, 10);

    instructionTableCB[0x88] = Instruction("RES 1, B", &z80::RES_B_R, 10);
    instructionTableCB[0x89] = Instruction("RES 1, C", &z80::RES_B_R, 10);
    instructionTableCB[0x8A] = Instruction("RES 1, D", &z80::RES_B_R, 10);
    instructionTableCB[0x8B] = Instruction("RES 1, E", &z80::RES_B_R, 10);
    instructionTableCB[0x8C] = Instruction("RES 1, H", &z80::RES_B_R, 10);
    instructionTableCB[0x8D] = Instruction("RES 1, L", &z80::RES_B_R, 10);
    instructionTableCB[0x8F] = Instruction("RES 1, A", &z80::RES_B_R, 10);
    instructionTableCB[0x8E] = Instruction("RES 1, (HL)", &z80::RES_B_HL, 10);

    instructionTableCB[0x50] = Instruction("BIT 2, B", &z80::BIT_B_R, 10);
    instructionTableCB[0x51] = Instruction("BIT 2, C", &z80::BIT_B_R, 10);
    instructionTableCB[0x52] = Instruction("BIT 2, D", &z80::BIT_B_R, 10);
    instructionTableCB[0x53] = Instruction("BIT 2, E", &z80::BIT_B_R, 10);
    instructionTableCB[0x54] = Instruction("BIT 2, H", &z80::BIT_B_R, 10);
    instructionTableCB[0x55] = Instruction("BIT 2, L", &z80::BIT_B_R, 10);
    instructionTableCB[0x57] = Instruction("BIT 2, A", &z80::BIT_B_R, 10);
    instructionTableCB[0x56] = Instruction("BIT 2, (HL)", &z80::BIT_B_HL, 10);

    instructionTableCB[0xD0] = Instruction("SET 2, B", &z80::SET_B_R, 10);
    instructionTableCB[0xD1] = Instruction("SET 2, C", &z80::SET_B_R, 10);
    instructionTableCB[0xD2] = Instruction("SET 2, D", &z80::SET_B_R, 10);
    instructionTableCB[0xD3] = Instruction("SET 2, E", &z80::SET_B_R, 10);
    instructionTableCB[0xD4] = Instruction("SET 2, H", &z80::SET_B_R, 10);
    instructionTableCB[0xD5] = Instruction("SET 2, L", &z80::SET_B_R, 10);
    instructionTableCB[0xD7] = Instruction("SET 2, A", &z80::SET_B_R, 10);
    instructionTableCB[0xD6] = Instruction("SET 2, (HL)", &z80::SET_B_HL, 10);

    instructionTableCB[0x90] = Instruction("RES 2, B", &z80::RES_B_R, 10);
    instructionTableCB[0x91] = Instruction("RES 2, C", &z80::RES_B_R, 10);
    instructionTableCB[0x92] = Instruction("RES 2, D", &z80::RES_B_R, 10);
    instructionTableCB[0x93] = Instruction("RES 2, E", &z80::RES_B_R, 10);
    instructionTableCB[0x94] = Instruction("RES 2, H", &z80::RES_B_R, 10);
    instructionTableCB[0x95] = Instruction("RES 2, L", &z80::RES_B_R, 10);
    instructionTableCB[0x97] = Instruction("RES 2, A", &z80::RES_B_R, 10);
    instructionTableCB[0x96] = Instruction("RES 2, (HL)", &z80::RES_B_HL, 10);

    instructionTableCB[0x58] = Instruction("BIT 3, B", &z80::BIT_B_R, 10);
    instructionTableCB[0x59] = Instruction("BIT 3, C", &z80::BIT_B_R, 10);
    instructionTableCB[0x5A] = Instruction("BIT 3, D", &z80::BIT_B_R, 10);
    instructionTableCB[0x5B] = Instruction("BIT 3, E", &z80::BIT_B_R, 10);
    instructionTableCB[0x5C] = Instruction("BIT 3, H", &z80::BIT_B_R, 10);
    instructionTableCB[0x5D] = Instruction("BIT 3, L", &z80::BIT_B_R, 10);
    instructionTableCB[0x5F] = Instruction("BIT 3, A", &z80::BIT_B_R, 10);
    instructionTableCB[0x5E] = Instruction("BIT 3, (HL)", &z80::BIT_B_HL, 10);

    instructionTableCB[0xD8] = Instruction("SET 3, B", &z80::SET_B_R, 10);
    instructionTableCB[0xD9] = Instruction("SET 3, C", &z80::SET_B_R, 10);
    instructionTableCB[0xDA] = Instruction("SET 3, D", &z80::SET_B_R, 10);
    instructionTableCB[0xDB] = Instruction("SET 3, E", &z80::SET_B_R, 10);
    instructionTableCB[0xDC] = Instruction("SET 3, H", &z80::SET_B_R, 10);
    instructionTableCB[0xDD] = Instruction("SET 3, L", &z80::SET_B_R, 10);
    instructionTableCB[0xDF] = Instruction("SET 3, A", &z80::SET_B_R, 10);
    instructionTableCB[0xDE] = Instruction("SET 3, (HL)", &z80::SET_B_HL, 10);

    instructionTableCB[0x98] = Instruction("RES 3, B", &z80::RES_B_R, 10);
    instructionTableCB[0x99] = Instruction("RES 3, C", &z80::RES_B_R, 10);
    instructionTableCB[0x9A] = Instruction("RES 3, D", &z80::RES_B_R, 10);
    instructionTableCB[0x9B] = Instruction("RES 3, E", &z80::RES_B_R, 10);
    instructionTableCB[0x9C] = Instruction("RES 3, H", &z80::RES_B_R, 10);
    instructionTableCB[0x9D] = Instruction("RES 3, L", &z80::RES_B_R, 10);
    instructionTableCB[0x9F] = Instruction("RES 3, A", &z80::RES_B_R, 10);
    instructionTableCB[0x9E] = Instruction("RES 3, (HL)", &z80::RES_B_HL, 10);

    instructionTableCB[0x60] = Instruction("BIT 4, B", &z80::BIT_B_R, 10);
    instructionTableCB[0x61] = Instruction("BIT 4, C", &z80::BIT_B_R, 10);
    instructionTableCB[0x62] = Instruction("BIT 4, D", &z80::BIT_B_R, 10);
    instructionTableCB[0x63] = Instruction("BIT 4, E", &z80::BIT_B_R, 10);
    instructionTableCB[0x64] = Instruction("BIT 4, H", &z80::BIT_B_R, 10);
    instructionTableCB[0x65] = Instruction("BIT 4, L", &z80::BIT_B_R, 10);
    instructionTableCB[0x67] = Instruction("BIT 4, A", &z80::BIT_B_R, 10);
    instructionTableCB[0x66] = Instruction("BIT 4, (HL)", &z80::BIT_B_HL, 10);

    instructionTableCB[0xE0] = Instruction("SET 4, B", &z80::SET_B_R, 10);
    instructionTableCB[0xE1] = Instruction("SET 4, C", &z80::SET_B_R, 10);
    instructionTableCB[0xE2] = Instruction("SET 4, D", &z80::SET_B_R, 10);
    instructionTableCB[0xE3] = Instruction("SET 4, E", &z80::SET_B_R, 10);
    instructionTableCB[0xE4] = Instruction("SET 4, H", &z80::SET_B_R, 10);
    instructionTableCB[0xE5] = Instruction("SET 4, L", &z80::SET_B_R, 10);
    instructionTableCB[0xE7] = Instruction("SET 4, A", &z80::SET_B_R, 10);
    instructionTableCB[0xE6] = Instruction("SET 4, (HL)", &z80::SET_B_HL, 10);

    instructionTableCB[0xA0] = Instruction("RES 4, B", &z80::RES_B_R, 10);
    instructionTableCB[0xA1] = Instruction("RES 4, C", &z80::RES_B_R, 10);
    instructionTableCB[0xA2] = Instruction("RES 4, D", &z80::RES_B_R, 10);
    instructionTableCB[0xA3] = Instruction("RES 4, E", &z80::RES_B_R, 10);
    instructionTableCB[0xA4] = Instruction("RES 4, H", &z80::RES_B_R, 10);
    instructionTableCB[0xA5] = Instruction("RES 4, L", &z80::RES_B_R, 10);
    instructionTableCB[0xA7] = Instruction("RES 4, A", &z80::RES_B_R, 10);
    instructionTableCB[0xA6] = Instruction("RES 4, (HL)", &z80::RES_B_HL, 10);

    instructionTableCB[0x68] = Instruction("BIT 5, B", &z80::BIT_B_R, 10);
    instructionTableCB[0x69] = Instruction("BIT 5, C", &z80::BIT_B_R, 10);
    instructionTableCB[0x6A] = Instruction("BIT 5, D", &z80::BIT_B_R, 10);
    instructionTableCB[0x6B] = Instruction("BIT 5, E", &z80::BIT_B_R, 10);
    instructionTableCB[0x6C] = Instruction("BIT 5, H", &z80::BIT_B_R, 10);
    instructionTableCB[0x6D] = Instruction("BIT 5, L", &z80::BIT_B_R, 10);
    instructionTableCB[0x6F] = Instruction("BIT 5, A", &z80::BIT_B_R, 10);
    instructionTableCB[0x6E] = Instruction("BIT 5, (HL)", &z80::BIT_B_HL, 10);

    instructionTableCB[0xE8] = Instruction("SET 5, B", &z80::SET_B_R, 10);
    instructionTableCB[0xE9] = Instruction("SET 5, C", &z80::SET_B_R, 10);
    instructionTableCB[0xEA] = Instruction("SET 5, D", &z80::SET_B_R, 10);
    instructionTableCB[0xEB] = Instruction("SET 5, E", &z80::SET_B_R, 10);
    instructionTableCB[0xEC] = Instruction("SET 5, H", &z80::SET_B_R, 10);
    instructionTableCB[0xED] = Instruction("SET 5, L", &z80::SET_B_R, 10);
    instructionTableCB[0xEF] = Instruction("SET 5, A", &z80::SET_B_R, 10);
    instructionTableCB[0xEE] = Instruction("SET 5, (HL)", &z80::SET_B_HL, 10);

    instructionTableCB[0xA8] = Instruction("RES 5, B", &z80::RES_B_R, 10);
    instructionTableCB[0xA9] = Instruction("RES 5, C", &z80::RES_B_R, 10);
    instructionTableCB[0xAA] = Instruction("RES 5, D", &z80::RES_B_R, 10);
    instructionTableCB[0xAB] = Instruction("RES 5, E", &z80::RES_B_R, 10);
    instructionTableCB[0xAC] = Instruction("RES 5, H", &z80::RES_B_R, 10);
    instructionTableCB[0xAD] = Instruction("RES 5, L", &z80::RES_B_R, 10);
    instructionTableCB[0xAF] = Instruction("RES 5, A", &z80::RES_B_R, 10);
    instructionTableCB[0xAE] = Instruction("RES 5, (HL)", &z80::RES_B_HL, 10);

    instructionTableCB[0x70] = Instruction("BIT 6, B", &z80::BIT_B_R, 10);
    instructionTableCB[0x71] = Instruction("BIT 6, C", &z80::BIT_B_R, 10);
    instructionTableCB[0x72] = Instruction("BIT 6, D", &z80::BIT_B_R, 10);
    instructionTableCB[0x73] = Instruction("BIT 6, E", &z80::BIT_B_R, 10);
    instructionTableCB[0x74] = Instruction("BIT 6, H", &z80::BIT_B_R, 10);
    instructionTableCB[0x75] = Instruction("BIT 6, L", &z80::BIT_B_R, 10);
    instructionTableCB[0x77] = Instruction("BIT 6, A", &z80::BIT_B_R, 10);
    instructionTableCB[0x76] = Instruction("BIT 6, (HL)", &z80::BIT_B_HL, 10);

    instructionTableCB[0xF0] = Instruction("SET 6, B", &z80::SET_B_R, 10);
    instructionTableCB[0xF1] = Instruction("SET 6, C", &z80::SET_B_R, 10);
    instructionTableCB[0xF2] = Instruction("SET 6, D", &z80::SET_B_R, 10);
    instructionTableCB[0xF3] = Instruction("SET 6, E", &z80::SET_B_R, 10);
    instructionTableCB[0xF4] = Instruction("SET 6, H", &z80::SET_B_R, 10);
    instructionTableCB[0xF5] = Instruction("SET 6, L", &z80::SET_B_R, 10);
    instructionTableCB[0xF7] = Instruction("SET 6, A", &z80::SET_B_R, 10);
    instructionTableCB[0xF6] = Instruction("SET 6, (HL)", &z80::SET_B_HL, 10);

    instructionTableCB[0xB0] = Instruction("RES 6, B", &z80::RES_B_R, 10);
    instructionTableCB[0xB1] = Instruction("RES 6, C", &z80::RES_B_R, 10);
    instructionTableCB[0xB2] = Instruction("RES 6, D", &z80::RES_B_R, 10);
    instructionTableCB[0xB3] = Instruction("RES 6, E", &z80::RES_B_R, 10);
    instructionTableCB[0xB4] = Instruction("RES 6, H", &z80::RES_B_R, 10);
    instructionTableCB[0xB5] = Instruction("RES 6, L", &z80::RES_B_R, 10);
    instructionTableCB[0xB7] = Instruction("RES 6, A", &z80::RES_B_R, 10);
    instructionTableCB[0xB6] = Instruction("RES 6, (HL)", &z80::RES_B_HL, 10);

    instructionTableCB[0x78] = Instruction("BIT 7, B", &z80::BIT_B_R, 10);
    instructionTableCB[0x79] = Instruction("BIT 7, C", &z80::BIT_B_R, 10);
    instructionTableCB[0x7A] = Instruction("BIT 7, D", &z80::BIT_B_R, 10);
    instructionTableCB[0x7B] = Instruction("BIT 7, E", &z80::BIT_B_R, 10);
    instructionTableCB[0x7C] = Instruction("BIT 7, H", &z80::BIT_B_R, 10);
    instructionTableCB[0x7D] = Instruction("BIT 7, L", &z80::BIT_B_R, 10);
    instructionTableCB[0x7F] = Instruction("BIT 7, A", &z80::BIT_B_R, 10);
    instructionTableCB[0x7E] = Instruction("BIT 7, (HL)", &z80::BIT_B_HL, 10);

    instructionTableCB[0xF8] = Instruction("SET 7, B", &z80::SET_B_R, 10);
    instructionTableCB[0xF9] = Instruction("SET 7, C", &z80::SET_B_R, 10);
    instructionTableCB[0xFA] = Instruction("SET 7, D", &z80::SET_B_R, 10);
    instructionTableCB[0xFB] = Instruction("SET 7, E", &z80::SET_B_R, 10);
    instructionTableCB[0xFC] = Instruction("SET 7, H", &z80::SET_B_R, 10);
    instructionTableCB[0xFD] = Instruction("SET 7, L", &z80::SET_B_R, 10);
    instructionTableCB[0xFF] = Instruction("SET 7, A", &z80::SET_B_R, 10);
    instructionTableCB[0xFE] = Instruction("SET 7, (HL)", &z80::SET_B_HL, 10);

    instructionTableCB[0xB8] = Instruction("RES 7, B", &z80::RES_B_R, 10);
    instructionTableCB[0xB9] = Instruction("RES 7, C", &z80::RES_B_R, 10);
    instructionTableCB[0xBA] = Instruction("RES 7, D", &z80::RES_B_R, 10);
    instructionTableCB[0xBB] = Instruction("RES 7, E", &z80::RES_B_R, 10);
    instructionTableCB[0xBC] = Instruction("RES 7, H", &z80::RES_B_R, 10);
    instructionTableCB[0xBD] = Instruction("RES 7, L", &z80::RES_B_R, 10);
    instructionTableCB[0xBF] = Instruction("RES 7, A", &z80::RES_B_R, 10);
    instructionTableCB[0xBE] = Instruction("RES 7, (HL)", &z80::RES_B_HL, 10);


    instructionTableDDCB[0x40] = Instruction("BIT 0, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x41] = Instruction("BIT 0, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x42] = Instruction("BIT 0, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x43] = Instruction("BIT 0, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x44] = Instruction("BIT 0, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x45] = Instruction("BIT 0, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x46] = Instruction("BIT 0, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x47] = Instruction("BIT 0, (IX+d)", &z80::BIT_B_IX_D, 10);

    instructionTableDDCB[0xC0] = Instruction("SET 0, (IX+d), B", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xC1] = Instruction("SET 0, (IX+d), C", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xC2] = Instruction("SET 0, (IX+d), D", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xC3] = Instruction("SET 0, (IX+d), E", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xC4] = Instruction("SET 0, (IX+d), H", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xC5] = Instruction("SET 0, (IX+d), L", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xC6] = Instruction("SET 0, (IX+d)", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xC7] = Instruction("SET 0, (IX+d) ,A", &z80::SET_B_IX_D, 10);

    instructionTableDDCB[0x80] = Instruction("RES 0, (IX+d), B", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x81] = Instruction("RES 0, (IX+d), C", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x82] = Instruction("RES 0, (IX+d), D", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x83] = Instruction("RES 0, (IX+d), E", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x84] = Instruction("RES 0, (IX+d), H", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x85] = Instruction("RES 0, (IX+d), L", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x86] = Instruction("RES 0, (IX+d)", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x87] = Instruction("RES 0, (IX+d) ,A", &z80::RES_B_IX_D, 10);

    instructionTableDDCB[0x48] = Instruction("BIT 1, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x49] = Instruction("BIT 1, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x4A] = Instruction("BIT 1, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x4B] = Instruction("BIT 1, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x4C] = Instruction("BIT 1, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x4D] = Instruction("BIT 1, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x4E] = Instruction("BIT 1, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x4F] = Instruction("BIT 1, (IX+d)", &z80::BIT_B_IX_D, 10);

    instructionTableDDCB[0xC8] = Instruction("SET 1, (IX+d), B", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xC9] = Instruction("SET 1, (IX+d), C", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xCA] = Instruction("SET 1, (IX+d), D", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xCB] = Instruction("SET 1, (IX+d), E", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xCC] = Instruction("SET 1, (IX+d), H", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xCD] = Instruction("SET 1, (IX+d), L", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xCE] = Instruction("SET 1, (IX+d),", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xCF] = Instruction("SET 1, (IX+d), A", &z80::SET_B_IX_D, 10);

    instructionTableDDCB[0x88] = Instruction("RES 1, (IX+d), B", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x89] = Instruction("RES 1, (IX+d), C", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x8A] = Instruction("RES 1, (IX+d), D", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x8B] = Instruction("RES 1, (IX+d), E", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x8C] = Instruction("RES 1, (IX+d), H", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x8D] = Instruction("RES 1, (IX+d), L", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x8E] = Instruction("RES 1, (IX+d),", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x8F] = Instruction("RES 1, (IX+d), A", &z80::RES_B_IX_D, 10);

    instructionTableDDCB[0x50] = Instruction("BIT 2, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x51] = Instruction("BIT 2, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x52] = Instruction("BIT 2, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x53] = Instruction("BIT 2, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x54] = Instruction("BIT 2, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x55] = Instruction("BIT 2, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x56] = Instruction("BIT 2, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x57] = Instruction("BIT 2, (IX+d)", &z80::BIT_B_IX_D, 10);

    instructionTableDDCB[0xD0] = Instruction("SET 2, (IX+d), B", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xD1] = Instruction("SET 2, (IX+d), C", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xD2] = Instruction("SET 2, (IX+d), D", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xD3] = Instruction("SET 2, (IX+d), E", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xD4] = Instruction("SET 2, (IX+d), H", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xD5] = Instruction("SET 2, (IX+d), L", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xD6] = Instruction("SET 2, (IX+d),  ", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xD7] = Instruction("SET 2, (IX+d), A", &z80::SET_B_IX_D, 10);

    instructionTableDDCB[0x90] = Instruction("RES 2, (IX+d), B", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x91] = Instruction("RES 2, (IX+d), C", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x92] = Instruction("RES 2, (IX+d), D", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x93] = Instruction("RES 2, (IX+d), E", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x94] = Instruction("RES 2, (IX+d), H", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x95] = Instruction("RES 2, (IX+d), L", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x96] = Instruction("RES 2, (IX+d),  ", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x97] = Instruction("RES 2, (IX+d), A", &z80::RES_B_IX_D, 10);

    instructionTableDDCB[0x58] = Instruction("BIT 3, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x59] = Instruction("BIT 3, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x5A] = Instruction("BIT 3, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x5B] = Instruction("BIT 3, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x5C] = Instruction("BIT 3, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x5D] = Instruction("BIT 3, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x5E] = Instruction("BIT 3, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x5F] = Instruction("BIT 3, (IX+d)", &z80::BIT_B_IX_D, 10);

    instructionTableDDCB[0xD8] = Instruction("SET 3, (IX+d), B", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xD9] = Instruction("SET 3, (IX+d), C", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xDA] = Instruction("SET 3, (IX+d), D", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xDB] = Instruction("SET 3, (IX+d), E", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xDC] = Instruction("SET 3, (IX+d), H", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xDD] = Instruction("SET 3, (IX+d), L", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xDE] = Instruction("SET 3, (IX+d),  ", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xDF] = Instruction("SET 3, (IX+d), A", &z80::SET_B_IX_D, 10);

    instructionTableDDCB[0x98] = Instruction("RES 3, (IX+d), B", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x99] = Instruction("RES 3, (IX+d), C", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x9A] = Instruction("RES 3, (IX+d), D", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x9B] = Instruction("RES 3, (IX+d), E", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x9C] = Instruction("RES 3, (IX+d), H", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x9D] = Instruction("RES 3, (IX+d), L", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x9E] = Instruction("RES 3, (IX+d),  ", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0x9F] = Instruction("RES 3, (IX+d), A", &z80::RES_B_IX_D, 10);

    instructionTableDDCB[0x60] = Instruction("BIT 4, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x61] = Instruction("BIT 4, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x62] = Instruction("BIT 4, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x63] = Instruction("BIT 4, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x64] = Instruction("BIT 4, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x65] = Instruction("BIT 4, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x66] = Instruction("BIT 4, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x67] = Instruction("BIT 4, (IX+d)", &z80::BIT_B_IX_D, 10);

    instructionTableDDCB[0xE0] = Instruction("SET 4, (IX+d), B", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xE1] = Instruction("SET 4, (IX+d), C", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xE2] = Instruction("SET 4, (IX+d), D", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xE3] = Instruction("SET 4, (IX+d), E", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xE4] = Instruction("SET 4, (IX+d), H", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xE5] = Instruction("SET 4, (IX+d), L", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xE6] = Instruction("SET 4, (IX+d),  ", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xE7] = Instruction("SET 4, (IX+d), A", &z80::SET_B_IX_D, 10);

    instructionTableDDCB[0xA0] = Instruction("RES 4, (IX+d), B", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xA1] = Instruction("RES 4, (IX+d), C", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xA2] = Instruction("RES 4, (IX+d), D", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xA3] = Instruction("RES 4, (IX+d), E", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xA4] = Instruction("RES 4, (IX+d), H", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xA5] = Instruction("RES 4, (IX+d), L", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xA6] = Instruction("RES 4, (IX+d),  ", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xA7] = Instruction("RES 4, (IX+d), A", &z80::RES_B_IX_D, 10);

    instructionTableDDCB[0x68] = Instruction("BIT 5, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x69] = Instruction("BIT 5, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x6A] = Instruction("BIT 5, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x6B] = Instruction("BIT 5, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x6C] = Instruction("BIT 5, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x6D] = Instruction("BIT 5, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x6E] = Instruction("BIT 5, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x6F] = Instruction("BIT 5, (IX+d)", &z80::BIT_B_IX_D, 10);

    instructionTableDDCB[0xE8] = Instruction("SET 5, (IX+d), B", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xE9] = Instruction("SET 5, (IX+d), C", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xEA] = Instruction("SET 5, (IX+d), D", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xEB] = Instruction("SET 5, (IX+d), E", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xEC] = Instruction("SET 5, (IX+d), H", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xED] = Instruction("SET 5, (IX+d), L", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xEE] = Instruction("SET 5, (IX+d),  ", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xEF] = Instruction("SET 5, (IX+d), A", &z80::SET_B_IX_D, 10);

    instructionTableDDCB[0xA8] = Instruction("RES 5, (IX+d), B", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xA9] = Instruction("RES 5, (IX+d), C", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xAA] = Instruction("RES 5, (IX+d), D", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xAB] = Instruction("RES 5, (IX+d), E", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xAC] = Instruction("RES 5, (IX+d), H", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xAD] = Instruction("RES 5, (IX+d), L", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xAE] = Instruction("RES 5, (IX+d),  ", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xAF] = Instruction("RES 5, (IX+d), A", &z80::RES_B_IX_D, 10);

    instructionTableDDCB[0x70] = Instruction("BIT 6, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x71] = Instruction("BIT 6, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x72] = Instruction("BIT 6, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x73] = Instruction("BIT 6, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x74] = Instruction("BIT 6, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x75] = Instruction("BIT 6, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x76] = Instruction("BIT 6, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x77] = Instruction("BIT 6, (IX+d)", &z80::BIT_B_IX_D, 10);

    instructionTableDDCB[0xF0] = Instruction("SET 6, (IX+d), B", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xF1] = Instruction("SET 6, (IX+d), C", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xF2] = Instruction("SET 6, (IX+d), D", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xF3] = Instruction("SET 6, (IX+d), E", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xF4] = Instruction("SET 6, (IX+d), H", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xF5] = Instruction("SET 6, (IX+d), L", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xF6] = Instruction("SET 6, (IX+d),  ", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xF7] = Instruction("SET 6, (IX+d), A", &z80::SET_B_IX_D, 10);

    instructionTableDDCB[0xB0] = Instruction("RES 6, (IX+d), B", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xB1] = Instruction("RES 6, (IX+d), C", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xB2] = Instruction("RES 6, (IX+d), D", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xB3] = Instruction("RES 6, (IX+d), E", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xB4] = Instruction("RES 6, (IX+d), H", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xB5] = Instruction("RES 6, (IX+d), L", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xB6] = Instruction("RES 6, (IX+d),  ", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xB7] = Instruction("RES 6, (IX+d), A", &z80::RES_B_IX_D, 10);

    instructionTableDDCB[0x78] = Instruction("BIT 7, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x79] = Instruction("BIT 7, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x7A] = Instruction("BIT 7, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x7B] = Instruction("BIT 7, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x7C] = Instruction("BIT 7, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x7D] = Instruction("BIT 7, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x7E] = Instruction("BIT 7, (IX+d)", &z80::BIT_B_IX_D, 10);
    instructionTableDDCB[0x7F] = Instruction("BIT 7, (IX+d)", &z80::BIT_B_IX_D, 10);

    instructionTableDDCB[0xF8] = Instruction("SET 7, (IX+d), B", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xF9] = Instruction("SET 7, (IX+d), C", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xFA] = Instruction("SET 7, (IX+d), D", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xFB] = Instruction("SET 7, (IX+d), E", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xFC] = Instruction("SET 7, (IX+d), H", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xFD] = Instruction("SET 7, (IX+d), L", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xFE] = Instruction("SET 7, (IX+d),  ", &z80::SET_B_IX_D, 10);
    instructionTableDDCB[0xFF] = Instruction("SET 7, (IX+d), A", &z80::SET_B_IX_D, 10);

    instructionTableDDCB[0xB8] = Instruction("RES 7, (IX+d), B", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xB9] = Instruction("RES 7, (IX+d), C", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xBA] = Instruction("RES 7, (IX+d), D", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xBB] = Instruction("RES 7, (IX+d), E", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xBC] = Instruction("RES 7, (IX+d), H", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xBD] = Instruction("RES 7, (IX+d), L", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xBE] = Instruction("RES 7, (IX+d),  ", &z80::RES_B_IX_D, 10);
    instructionTableDDCB[0xBF] = Instruction("RES 7, (IX+d), A", &z80::RES_B_IX_D, 10);

    instructionTableDDCB[0x00] = Instruction("RLC (IX+d), B", &z80::RLC_IX_D, 10);
    instructionTableDDCB[0x01] = Instruction("RLC (IX+d), C", &z80::RLC_IX_D, 10);
    instructionTableDDCB[0x02] = Instruction("RLC (IX+d), D", &z80::RLC_IX_D, 10);
    instructionTableDDCB[0x03] = Instruction("RLC (IX+d), E", &z80::RLC_IX_D, 10);
    instructionTableDDCB[0x04] = Instruction("RLC (IX+d), H", &z80::RLC_IX_D, 10);
    instructionTableDDCB[0x05] = Instruction("RLC (IX+d), L", &z80::RLC_IX_D, 10);
    instructionTableDDCB[0x06] = Instruction("RLC (IX+d)", &z80::RLC_IX_D, 10);
    instructionTableDDCB[0x07] = Instruction("RLC (IX+d), A", &z80::RLC_IX_D, 10);

    instructionTableDDCB[0x10] = Instruction("RL (IX+d), B", &z80::RL_IX_D, 10);
    instructionTableDDCB[0x11] = Instruction("RL (IX+d), C", &z80::RL_IX_D, 10);
    instructionTableDDCB[0x12] = Instruction("RL (IX+d), D", &z80::RL_IX_D, 10);
    instructionTableDDCB[0x13] = Instruction("RL (IX+d), E", &z80::RL_IX_D, 10);
    instructionTableDDCB[0x14] = Instruction("RL (IX+d), H", &z80::RL_IX_D, 10);
    instructionTableDDCB[0x15] = Instruction("RL (IX+d), L", &z80::RL_IX_D, 10);
    instructionTableDDCB[0x16] = Instruction("RL (IX+d)", &z80::RL_IX_D, 10);
    instructionTableDDCB[0x17] = Instruction("RL (IX+d), A", &z80::RL_IX_D, 10);

    instructionTableDDCB[0x08] = Instruction("RRC (IX+d), B", &z80::RRC_IX_D, 10);
    instructionTableDDCB[0x09] = Instruction("RRC (IX+d), C", &z80::RRC_IX_D, 10);
    instructionTableDDCB[0x0A] = Instruction("RRC (IX+d), D", &z80::RRC_IX_D, 10);
    instructionTableDDCB[0x0B] = Instruction("RRC (IX+d), E", &z80::RRC_IX_D, 10);
    instructionTableDDCB[0x0C] = Instruction("RRC (IX+d), H", &z80::RRC_IX_D, 10);
    instructionTableDDCB[0x0D] = Instruction("RRC (IX+d), L", &z80::RRC_IX_D, 10);
    instructionTableDDCB[0x0E] = Instruction("RRC (IX+d)", &z80::RRC_IX_D, 10);
    instructionTableDDCB[0x0F] = Instruction("RRC (IX+d), A", &z80::RRC_IX_D, 10);

    instructionTableDDCB[0x18] = Instruction("RR (IX+d), B", &z80::RR_IX_D, 10);
    instructionTableDDCB[0x19] = Instruction("RR (IX+d), C", &z80::RR_IX_D, 10);
    instructionTableDDCB[0x1A] = Instruction("RR (IX+d), D", &z80::RR_IX_D, 10);
    instructionTableDDCB[0x1B] = Instruction("RR (IX+d), E", &z80::RR_IX_D, 10);
    instructionTableDDCB[0x1C] = Instruction("RR (IX+d), H", &z80::RR_IX_D, 10);
    instructionTableDDCB[0x1D] = Instruction("RR (IX+d), L", &z80::RR_IX_D, 10);
    instructionTableDDCB[0x1E] = Instruction("RR (IX+d)", &z80::RR_IX_D, 10);
    instructionTableDDCB[0x1F] = Instruction("RR (IX+d), A", &z80::RR_IX_D, 10);

    instructionTableDDCB[0x20] = Instruction("SLA (IX+d), B", &z80::SLA_IX_D, 10);
    instructionTableDDCB[0x21] = Instruction("SLA (IX+d), C", &z80::SLA_IX_D, 10);
    instructionTableDDCB[0x22] = Instruction("SLA (IX+d), D", &z80::SLA_IX_D, 10);
    instructionTableDDCB[0x23] = Instruction("SLA (IX+d), E", &z80::SLA_IX_D, 10);
    instructionTableDDCB[0x24] = Instruction("SLA (IX+d), H", &z80::SLA_IX_D, 10);
    instructionTableDDCB[0x25] = Instruction("SLA (IX+d), L", &z80::SLA_IX_D, 10);
    instructionTableDDCB[0x26] = Instruction("SLA (IX+d)", &z80::SLA_IX_D, 10);
    instructionTableDDCB[0x27] = Instruction("SLA (IX+d), A", &z80::SLA_IX_D, 10);

    instructionTableDDCB[0x30] = Instruction("SLS (IX+d), B", &z80::SLS_IX_D, 10);
    instructionTableDDCB[0x31] = Instruction("SLS (IX+d), C", &z80::SLS_IX_D, 10);
    instructionTableDDCB[0x32] = Instruction("SLS (IX+d), D", &z80::SLS_IX_D, 10);
    instructionTableDDCB[0x33] = Instruction("SLS (IX+d), E", &z80::SLS_IX_D, 10);
    instructionTableDDCB[0x34] = Instruction("SLS (IX+d), H", &z80::SLS_IX_D, 10);
    instructionTableDDCB[0x35] = Instruction("SLS (IX+d), L", &z80::SLS_IX_D, 10);
    instructionTableDDCB[0x36] = Instruction("SLS (IX+d)", &z80::SLS_IX_D, 10);
    instructionTableDDCB[0x37] = Instruction("SLS (IX+d), A", &z80::SLS_IX_D, 10);

    instructionTableDDCB[0x28] = Instruction("SRA (IX+d), B", &z80::SRA_IX_D, 10);
    instructionTableDDCB[0x29] = Instruction("SRA (IX+d), C", &z80::SRA_IX_D, 10);
    instructionTableDDCB[0x2A] = Instruction("SRA (IX+d), D", &z80::SRA_IX_D, 10);
    instructionTableDDCB[0x2B] = Instruction("SRA (IX+d), E", &z80::SRA_IX_D, 10);
    instructionTableDDCB[0x2C] = Instruction("SRA (IX+d), H", &z80::SRA_IX_D, 10);
    instructionTableDDCB[0x2D] = Instruction("SRA (IX+d), L", &z80::SRA_IX_D, 10);
    instructionTableDDCB[0x2E] = Instruction("SRA (IX+d)", &z80::SRA_IX_D, 10);
    instructionTableDDCB[0x2F] = Instruction("SRA (IX+d), A", &z80::SRA_IX_D, 10);

    instructionTableDDCB[0x38] = Instruction("SRL (IX+d), B", &z80::SRL_IX_D, 10);
    instructionTableDDCB[0x39] = Instruction("SRL (IX+d), C", &z80::SRL_IX_D, 10);
    instructionTableDDCB[0x3A] = Instruction("SRL (IX+d), D", &z80::SRL_IX_D, 10);
    instructionTableDDCB[0x3B] = Instruction("SRL (IX+d), E", &z80::SRL_IX_D, 10);
    instructionTableDDCB[0x3C] = Instruction("SRL (IX+d), H", &z80::SRL_IX_D, 10);
    instructionTableDDCB[0x3D] = Instruction("SRL (IX+d), L", &z80::SRL_IX_D, 10);
    instructionTableDDCB[0x3E] = Instruction("SRL (IX+d)", &z80::SRL_IX_D, 10);
    instructionTableDDCB[0x3F] = Instruction("SRL (IX+d), A", &z80::SRL_IX_D, 10);

    instructionTableFDCB[0x00] = Instruction("RLC (IY+d), B", &z80::RLC_IY_D, 10);
    instructionTableFDCB[0x01] = Instruction("RLC (IY+d), C", &z80::RLC_IY_D, 10);
    instructionTableFDCB[0x02] = Instruction("RLC (IY+d), D", &z80::RLC_IY_D, 10);
    instructionTableFDCB[0x03] = Instruction("RLC (IY+d), E", &z80::RLC_IY_D, 10);
    instructionTableFDCB[0x04] = Instruction("RLC (IY+d), H", &z80::RLC_IY_D, 10);
    instructionTableFDCB[0x05] = Instruction("RLC (IY+d), L", &z80::RLC_IY_D, 10);
    instructionTableFDCB[0x06] = Instruction("RLC (IY+d)", &z80::RLC_IY_D, 10);
    instructionTableFDCB[0x07] = Instruction("RLC (IY+d), A", &z80::RLC_IY_D, 10);

    instructionTableFDCB[0x10] = Instruction("RL (IY+d), B", &z80::RL_IY_D, 10);
    instructionTableFDCB[0x11] = Instruction("RL (IY+d), C", &z80::RL_IY_D, 10);
    instructionTableFDCB[0x12] = Instruction("RL (IY+d), D", &z80::RL_IY_D, 10);
    instructionTableFDCB[0x13] = Instruction("RL (IY+d), E", &z80::RL_IY_D, 10);
    instructionTableFDCB[0x14] = Instruction("RL (IY+d), H", &z80::RL_IY_D, 10);
    instructionTableFDCB[0x15] = Instruction("RL (IY+d), L", &z80::RL_IY_D, 10);
    instructionTableFDCB[0x16] = Instruction("RL (IY+d)", &z80::RL_IY_D, 10);
    instructionTableFDCB[0x17] = Instruction("RL (IY+d), A", &z80::RL_IY_D, 10);

    instructionTableFDCB[0x08] = Instruction("RRC (IY+d), B", &z80::RRC_IY_D, 10);
    instructionTableFDCB[0x09] = Instruction("RRC (IY+d), C", &z80::RRC_IY_D, 10);
    instructionTableFDCB[0x0A] = Instruction("RRC (IY+d), D", &z80::RRC_IY_D, 10);
    instructionTableFDCB[0x0B] = Instruction("RRC (IY+d), E", &z80::RRC_IY_D, 10);
    instructionTableFDCB[0x0C] = Instruction("RRC (IY+d), H", &z80::RRC_IY_D, 10);
    instructionTableFDCB[0x0D] = Instruction("RRC (IY+d), L", &z80::RRC_IY_D, 10);
    instructionTableFDCB[0x0E] = Instruction("RRC (IY+d)", &z80::RRC_IY_D, 10);
    instructionTableFDCB[0x0F] = Instruction("RRC (IY+d), A", &z80::RRC_IY_D, 10);

    instructionTableFDCB[0x18] = Instruction("RR (IY+d), B", &z80::RR_IY_D, 10);
    instructionTableFDCB[0x19] = Instruction("RR (IY+d), C", &z80::RR_IY_D, 10);
    instructionTableFDCB[0x1A] = Instruction("RR (IY+d), D", &z80::RR_IY_D, 10);
    instructionTableFDCB[0x1B] = Instruction("RR (IY+d), E", &z80::RR_IY_D, 10);
    instructionTableFDCB[0x1C] = Instruction("RR (IY+d), H", &z80::RR_IY_D, 10);
    instructionTableFDCB[0x1D] = Instruction("RR (IY+d), L", &z80::RR_IY_D, 10);
    instructionTableFDCB[0x1E] = Instruction("RR (IY+d)", &z80::RR_IY_D, 10);
    instructionTableFDCB[0x1F] = Instruction("RR (IY+d), A", &z80::RR_IY_D, 10);

    instructionTableFDCB[0x20] = Instruction("SLA (IY+d), B", &z80::SLA_IY_D, 10);
    instructionTableFDCB[0x21] = Instruction("SLA (IY+d), C", &z80::SLA_IY_D, 10);
    instructionTableFDCB[0x22] = Instruction("SLA (IY+d), D", &z80::SLA_IY_D, 10);
    instructionTableFDCB[0x23] = Instruction("SLA (IY+d), E", &z80::SLA_IY_D, 10);
    instructionTableFDCB[0x24] = Instruction("SLA (IY+d), H", &z80::SLA_IY_D, 10);
    instructionTableFDCB[0x25] = Instruction("SLA (IY+d), L", &z80::SLA_IY_D, 10);
    instructionTableFDCB[0x26] = Instruction("SLA (IY+d)", &z80::SLA_IY_D, 10);
    instructionTableFDCB[0x27] = Instruction("SLA (IY+d), A", &z80::SLA_IY_D, 10);

    instructionTableFDCB[0x30] = Instruction("SLS (IY+d), B", &z80::SLS_IY_D, 10);
    instructionTableFDCB[0x31] = Instruction("SLS (IY+d), C", &z80::SLS_IY_D, 10);
    instructionTableFDCB[0x32] = Instruction("SLS (IY+d), D", &z80::SLS_IY_D, 10);
    instructionTableFDCB[0x33] = Instruction("SLS (IY+d), E", &z80::SLS_IY_D, 10);
    instructionTableFDCB[0x34] = Instruction("SLS (IY+d), H", &z80::SLS_IY_D, 10);
    instructionTableFDCB[0x35] = Instruction("SLS (IY+d), L", &z80::SLS_IY_D, 10);
    instructionTableFDCB[0x36] = Instruction("SLS (IY+d)", &z80::SLS_IY_D, 10);
    instructionTableFDCB[0x37] = Instruction("SLS (IY+d), A", &z80::SLS_IY_D, 10);

    instructionTableFDCB[0x28] = Instruction("SRA (IY+d), B", &z80::SRA_IY_D, 10);
    instructionTableFDCB[0x29] = Instruction("SRA (IY+d), C", &z80::SRA_IY_D, 10);
    instructionTableFDCB[0x2A] = Instruction("SRA (IY+d), D", &z80::SRA_IY_D, 10);
    instructionTableFDCB[0x2B] = Instruction("SRA (IY+d), E", &z80::SRA_IY_D, 10);
    instructionTableFDCB[0x2C] = Instruction("SRA (IY+d), H", &z80::SRA_IY_D, 10);
    instructionTableFDCB[0x2D] = Instruction("SRA (IY+d), L", &z80::SRA_IY_D, 10);
    instructionTableFDCB[0x2E] = Instruction("SRA (IY+d)", &z80::SRA_IY_D, 10);
    instructionTableFDCB[0x2F] = Instruction("SRA (IY+d), A", &z80::SRA_IY_D, 10);

    instructionTableFDCB[0x38] = Instruction("SRL (IY+d), B", &z80::SRL_IY_D, 10);
    instructionTableFDCB[0x39] = Instruction("SRL (IY+d), C", &z80::SRL_IY_D, 10);
    instructionTableFDCB[0x3A] = Instruction("SRL (IY+d), D", &z80::SRL_IY_D, 10);
    instructionTableFDCB[0x3B] = Instruction("SRL (IY+d), E", &z80::SRL_IY_D, 10);
    instructionTableFDCB[0x3C] = Instruction("SRL (IY+d), H", &z80::SRL_IY_D, 10);
    instructionTableFDCB[0x3D] = Instruction("SRL (IY+d), L", &z80::SRL_IY_D, 10);
    instructionTableFDCB[0x3E] = Instruction("SRL (IY+d)", &z80::SRL_IY_D, 10);
    instructionTableFDCB[0x3F] = Instruction("SRL (IY+d), A", &z80::SRL_IY_D, 10);

    instructionTableFDCB[0x40] = Instruction("BIT 0, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x41] = Instruction("BIT 0, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x42] = Instruction("BIT 0, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x43] = Instruction("BIT 0, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x44] = Instruction("BIT 0, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x45] = Instruction("BIT 0, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x46] = Instruction("BIT 0, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x47] = Instruction("BIT 0, (IY+d)", &z80::BIT_B_IY_D, 10);

    instructionTableFDCB[0xC0] = Instruction("SET 0, (IY+d), B", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xC1] = Instruction("SET 0, (IY+d), C", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xC2] = Instruction("SET 0, (IY+d), D", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xC3] = Instruction("SET 0, (IY+d), E", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xC4] = Instruction("SET 0, (IY+d), H", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xC5] = Instruction("SET 0, (IY+d), L", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xC6] = Instruction("SET 0, (IY+d)", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xC7] = Instruction("SET 0, (IY+d) ,A", &z80::SET_B_IY_D, 10);

    instructionTableFDCB[0x80] = Instruction("RES 0, (IY+d), B", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x81] = Instruction("RES 0, (IY+d), C", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x82] = Instruction("RES 0, (IY+d), D", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x83] = Instruction("RES 0, (IY+d), E", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x84] = Instruction("RES 0, (IY+d), H", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x85] = Instruction("RES 0, (IY+d), L", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x86] = Instruction("RES 0, (IY+d)", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x87] = Instruction("RES 0, (IY+d) ,A", &z80::RES_B_IY_D, 10);

    instructionTableFDCB[0x48] = Instruction("BIT 1, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x49] = Instruction("BIT 1, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x4A] = Instruction("BIT 1, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x4B] = Instruction("BIT 1, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x4C] = Instruction("BIT 1, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x4D] = Instruction("BIT 1, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x4E] = Instruction("BIT 1, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x4F] = Instruction("BIT 1, (IY+d)", &z80::BIT_B_IY_D, 10);

    instructionTableFDCB[0xC8] = Instruction("SET 1, (IY+d), B", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xC9] = Instruction("SET 1, (IY+d), C", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xCA] = Instruction("SET 1, (IY+d), D", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xCB] = Instruction("SET 1, (IY+d), E", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xCC] = Instruction("SET 1, (IY+d), H", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xCD] = Instruction("SET 1, (IY+d), L", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xCE] = Instruction("SET 1, (IY+d),", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xCF] = Instruction("SET 1, (IY+d), A", &z80::SET_B_IY_D, 10);

    instructionTableFDCB[0x88] = Instruction("RES 1, (IY+d), B", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x89] = Instruction("RES 1, (IY+d), C", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x8A] = Instruction("RES 1, (IY+d), D", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x8B] = Instruction("RES 1, (IY+d), E", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x8C] = Instruction("RES 1, (IY+d), H", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x8D] = Instruction("RES 1, (IY+d), L", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x8E] = Instruction("RES 1, (IY+d),", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x8F] = Instruction("RES 1, (IY+d), A", &z80::RES_B_IY_D, 10);

    instructionTableFDCB[0x50] = Instruction("BIT 2, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x51] = Instruction("BIT 2, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x52] = Instruction("BIT 2, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x53] = Instruction("BIT 2, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x54] = Instruction("BIT 2, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x55] = Instruction("BIT 2, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x56] = Instruction("BIT 2, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x57] = Instruction("BIT 2, (IY+d)", &z80::BIT_B_IY_D, 10);

    instructionTableFDCB[0xD0] = Instruction("SET 2, (IY+d), B", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xD1] = Instruction("SET 2, (IY+d), C", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xD2] = Instruction("SET 2, (IY+d), D", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xD3] = Instruction("SET 2, (IY+d), E", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xD4] = Instruction("SET 2, (IY+d), H", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xD5] = Instruction("SET 2, (IY+d), L", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xD6] = Instruction("SET 2, (IY+d),  ", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xD7] = Instruction("SET 2, (IY+d), A", &z80::SET_B_IY_D, 10);

    instructionTableFDCB[0x90] = Instruction("RES 2, (IY+d), B", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x91] = Instruction("RES 2, (IY+d), C", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x92] = Instruction("RES 2, (IY+d), D", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x93] = Instruction("RES 2, (IY+d), E", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x94] = Instruction("RES 2, (IY+d), H", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x95] = Instruction("RES 2, (IY+d), L", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x96] = Instruction("RES 2, (IY+d),  ", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x97] = Instruction("RES 2, (IY+d), A", &z80::RES_B_IY_D, 10);

    instructionTableFDCB[0x58] = Instruction("BIT 3, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x59] = Instruction("BIT 3, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x5A] = Instruction("BIT 3, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x5B] = Instruction("BIT 3, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x5C] = Instruction("BIT 3, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x5D] = Instruction("BIT 3, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x5E] = Instruction("BIT 3, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x5F] = Instruction("BIT 3, (IY+d)", &z80::BIT_B_IY_D, 10);

    instructionTableFDCB[0xD8] = Instruction("SET 3, (IY+d), B", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xD9] = Instruction("SET 3, (IY+d), C", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xDA] = Instruction("SET 3, (IY+d), D", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xDB] = Instruction("SET 3, (IY+d), E", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xDC] = Instruction("SET 3, (IY+d), H", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xDD] = Instruction("SET 3, (IY+d), L", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xDE] = Instruction("SET 3, (IY+d),  ", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xDF] = Instruction("SET 3, (IY+d), A", &z80::SET_B_IY_D, 10);

    instructionTableFDCB[0x98] = Instruction("RES 3, (IY+d), B", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x99] = Instruction("RES 3, (IY+d), C", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x9A] = Instruction("RES 3, (IY+d), D", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x9B] = Instruction("RES 3, (IY+d), E", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x9C] = Instruction("RES 3, (IY+d), H", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x9D] = Instruction("RES 3, (IY+d), L", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x9E] = Instruction("RES 3, (IY+d),  ", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0x9F] = Instruction("RES 3, (IY+d), A", &z80::RES_B_IY_D, 10);

    instructionTableFDCB[0x60] = Instruction("BIT 4, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x61] = Instruction("BIT 4, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x62] = Instruction("BIT 4, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x63] = Instruction("BIT 4, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x64] = Instruction("BIT 4, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x65] = Instruction("BIT 4, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x66] = Instruction("BIT 4, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x67] = Instruction("BIT 4, (IY+d)", &z80::BIT_B_IY_D, 10);

    instructionTableFDCB[0xE0] = Instruction("SET 4, (IY+d), B", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xE1] = Instruction("SET 4, (IY+d), C", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xE2] = Instruction("SET 4, (IY+d), D", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xE3] = Instruction("SET 4, (IY+d), E", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xE4] = Instruction("SET 4, (IY+d), H", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xE5] = Instruction("SET 4, (IY+d), L", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xE6] = Instruction("SET 4, (IY+d),  ", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xE7] = Instruction("SET 4, (IY+d), A", &z80::SET_B_IY_D, 10);

    instructionTableFDCB[0xA0] = Instruction("RES 4, (IY+d), B", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xA1] = Instruction("RES 4, (IY+d), C", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xA2] = Instruction("RES 4, (IY+d), D", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xA3] = Instruction("RES 4, (IY+d), E", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xA4] = Instruction("RES 4, (IY+d), H", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xA5] = Instruction("RES 4, (IY+d), L", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xA6] = Instruction("RES 4, (IY+d),  ", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xA7] = Instruction("RES 4, (IY+d), A", &z80::RES_B_IY_D, 10);

    instructionTableFDCB[0x68] = Instruction("BIT 5, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x69] = Instruction("BIT 5, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x6A] = Instruction("BIT 5, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x6B] = Instruction("BIT 5, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x6C] = Instruction("BIT 5, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x6D] = Instruction("BIT 5, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x6E] = Instruction("BIT 5, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x6F] = Instruction("BIT 5, (IY+d)", &z80::BIT_B_IY_D, 10);

    instructionTableFDCB[0xE8] = Instruction("SET 5, (IY+d), B", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xE9] = Instruction("SET 5, (IY+d), C", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xEA] = Instruction("SET 5, (IY+d), D", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xEB] = Instruction("SET 5, (IY+d), E", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xEC] = Instruction("SET 5, (IY+d), H", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xED] = Instruction("SET 5, (IY+d), L", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xEE] = Instruction("SET 5, (IY+d),  ", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xEF] = Instruction("SET 5, (IY+d), A", &z80::SET_B_IY_D, 10);

    instructionTableFDCB[0xA8] = Instruction("RES 5, (IY+d), B", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xA9] = Instruction("RES 5, (IY+d), C", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xAA] = Instruction("RES 5, (IY+d), D", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xAB] = Instruction("RES 5, (IY+d), E", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xAC] = Instruction("RES 5, (IY+d), H", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xAD] = Instruction("RES 5, (IY+d), L", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xAE] = Instruction("RES 5, (IY+d),  ", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xAF] = Instruction("RES 5, (IY+d), A", &z80::RES_B_IY_D, 10);

    instructionTableFDCB[0x70] = Instruction("BIT 6, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x71] = Instruction("BIT 6, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x72] = Instruction("BIT 6, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x73] = Instruction("BIT 6, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x74] = Instruction("BIT 6, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x75] = Instruction("BIT 6, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x76] = Instruction("BIT 6, (IY+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x77] = Instruction("BIT 6, (IY+d)", &z80::BIT_B_IY_D, 10);

    instructionTableFDCB[0xF0] = Instruction("SET 6, (IY+d), B", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xF1] = Instruction("SET 6, (IY+d), C", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xF2] = Instruction("SET 6, (IY+d), D", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xF3] = Instruction("SET 6, (IY+d), E", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xF4] = Instruction("SET 6, (IY+d), H", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xF5] = Instruction("SET 6, (IY+d), L", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xF6] = Instruction("SET 6, (IY+d),  ", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xF7] = Instruction("SET 6, (IY+d), A", &z80::SET_B_IY_D, 10);

    instructionTableFDCB[0xB0] = Instruction("RES 6, (IY+d), B", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xB1] = Instruction("RES 6, (IY+d), C", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xB2] = Instruction("RES 6, (IY+d), D", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xB3] = Instruction("RES 6, (IY+d), E", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xB4] = Instruction("RES 6, (IY+d), H", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xB5] = Instruction("RES 6, (IY+d), L", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xB6] = Instruction("RES 6, (IY+d),  ", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xB7] = Instruction("RES 6, (IY+d), A", &z80::RES_B_IY_D, 10);

    instructionTableFDCB[0x78] = Instruction("BIT 7, (Iy+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x79] = Instruction("BIT 7, (Iy+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x7A] = Instruction("BIT 7, (Iy+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x7B] = Instruction("BIT 7, (Iy+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x7C] = Instruction("BIT 7, (Iy+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x7D] = Instruction("BIT 7, (Iy+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x7E] = Instruction("BIT 7, (Iy+d)", &z80::BIT_B_IY_D, 10);
    instructionTableFDCB[0x7F] = Instruction("BIT 7, (Iy+d)", &z80::BIT_B_IY_D, 10);

    instructionTableFDCB[0xF8] = Instruction("SET 7, (IY+d), B", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xF9] = Instruction("SET 7, (IY+d), C", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xFA] = Instruction("SET 7, (IY+d), D", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xFB] = Instruction("SET 7, (IY+d), E", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xFC] = Instruction("SET 7, (IY+d), H", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xFD] = Instruction("SET 7, (IY+d), L", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xFE] = Instruction("SET 7, (IY+d),  ", &z80::SET_B_IY_D, 10);
    instructionTableFDCB[0xFF] = Instruction("SET 7, (IY+d), A", &z80::SET_B_IY_D, 10);

    instructionTableFDCB[0xB8] = Instruction("RES 7, (IY+d), B", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xB9] = Instruction("RES 7, (IY+d), C", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xBA] = Instruction("RES 7, (IY+d), D", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xBB] = Instruction("RES 7, (IY+d), E", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xBC] = Instruction("RES 7, (IY+d), H", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xBD] = Instruction("RES 7, (IY+d), L", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xBE] = Instruction("RES 7, (IY+d),  ", &z80::RES_B_IY_D, 10);
    instructionTableFDCB[0xBF] = Instruction("RES 7, (IY+d), A", &z80::RES_B_IY_D, 10);
}

uint8_t z80::fetchImmediate()
{
    PC++;
    return bus->read(PC);
}

uint8_t z80::fetchHL()
{
    uint16_t adress = (H << 8) | L;
    uint8_t value = bus->read(adress);
    return value;
}
uint16_t z80::getBC()
{
    // Combine B (high byte) and C (low byte) to form a 16-bit address
    uint16_t address = (B << 8) | C; // iF does not work try to cast before...
    return address;
}
uint16_t z80::getBC1()
{
    // Combine B (high byte) and C (low byte) to form a 16-bit address
    uint16_t address = (B1 << 8) | C1; // iF does not work try to cast before...
    return address;
}
uint16_t z80::getDE()
{
    // Combine D (high byte) and E (low byte) to form a 16-bit address
    uint16_t address = (D << 8) | E; // iF does not work try to cast before...
    return address;
}
uint16_t z80::getDE1()
{
    // Combine D (high byte) and E (low byte) to form a 16-bit address
    uint16_t address = (D1 << 8) | E1; // iF does not work try to cast before...
    return address;
}
uint16_t z80::getHL()
{
    // Combine D (high byte) and E (low byte) to form a 16-bit address
    uint16_t address = (H << 8) | L; // iF does not work try to cast before...
    return address;
}
uint16_t z80::getHL1()
{
    // Combine D (high byte) and E (low byte) to form a 16-bit address
    uint16_t address = (H1 << 8) | L1; // iF does not work try to cast before...
    return address;
}
uint16_t z80::getAF()
{
    // Combine D (high byte) and E (low byte) to form a 16-bit address
    uint16_t address = (A << 8) | F; // iF does not work try to cast before...
    return address;
}
uint16_t z80::getAF1()
{
    // Combine D (high byte) and E (low byte) to form a 16-bit address
    uint16_t address = (A1 << 8) | F1; // iF does not work try to cast before...
    return address;
}

// Function to execute instructions based on opcode
void z80::execute(uint8_t opCode)
{
    // Check for regular instructions in the main table
    if (instructionTable.find(opCode) != instructionTable.end())
    {
        IncrementRefreshRegister(1);
        Instruction &instruction = instructionTable[opCode]; // Get the instruction
        instruction.execute(*this, opCode);
        PC++;
    }
    // Handle DD-prefixed instructions
    else if (opCode == 0xDD)
    {
        PC++;                   // Move to the next part of long opCode
        opCode = bus->read(PC); // Read the next opcode
        if (opCode == 0xCB)
        {
            IncrementRefreshRegister(2);
            opCode = bus->read(PC + 2); // dont increment PC here the function call fetchImmidiate 2 times so it will do it.
            if (instructionTableDDCB.find(opCode) != instructionTableDDCB.end())
            {
                Instruction &instruction = instructionTableDDCB[opCode];
                instruction.execute(*this, opCode); // Execute the instruction
                PC++;
            }
        }
        else
        {
            if(opCode==0x00 || opCode==0x64 || opCode==0x6D ){
               IncrementRefreshRegister(1);
            }
            else{
                IncrementRefreshRegister(2);
            }
            
            if (instructionTableDD.find(opCode) != instructionTableDD.end())
            {
                Instruction &instruction = instructionTableDD[opCode];
                instruction.execute(*this, opCode); // Execute the instruction
                PC++;
            }
        }
    }
    // Handle FD-prefixed instructions
    else if (opCode == 0xFD)
    {
     
        PC++;                   // Move to the next part of long opCode
        opCode = bus->read(PC); // Read the next opcode
        if (opCode == 0xCB)
        {
            IncrementRefreshRegister(2);
            opCode = bus->read(PC + 2); // dont increment PC here the function call fetchImmidiate 2 times so it will do it.
            if (instructionTableFDCB.find(opCode) != instructionTableFDCB.end())
            {
                Instruction &instruction = instructionTableFDCB[opCode];
                instruction.execute(*this, opCode); // Execute the instruction
                PC++;
            }
        }
        else
        {
              if(opCode ==0x64 || opCode ==0x6d){
               IncrementRefreshRegister(1);
            }
            else{
                IncrementRefreshRegister(2);
            }
            
            if (instructionTableFD.find(opCode) != instructionTableFD.end())
            {
                Instruction &instruction = instructionTableFD[opCode];
                instruction.execute(*this, opCode); // Execute the instruction
                PC++;
            }
        }
    }
    // Handle ED-prefixed instructions
    else if (opCode == 0xED)
    {
        
        
        IncrementRefreshRegister(2);
            
        PC++;                   // Move to the next part of long opCode
        opCode = bus->read(PC); // Read the next opcode
        if (instructionTableED.find(opCode) != instructionTableED.end())
        {
            Instruction &instruction = instructionTableED[opCode];
            instruction.execute(*this, opCode); // Execute the instruction
            PC++;
        }
    }
    else if (opCode == 0xCB)
    {
        IncrementRefreshRegister(2);
        PC++;                   // Move to the next part of long opCode
        opCode = bus->read(PC); // Read the next opcode
        if (instructionTableCB.find(opCode) != instructionTableCB.end())
        {
            Instruction &instruction = instructionTableCB[opCode];
            instruction.execute(*this, opCode); // Execute the instruction
            PC++;
        }
    }
    else
    {
        std::cout << "Opcode not found: 0x"
                  << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(opCode)
                  << " at PC: "
                  << std::dec << PC // Print PC in decimal
                  << std::endl;
    }
}

/*****************************************|
 *                                        |
 *         8-Bit Load Group               |
 *                                        |
 *****************************************/

void z80::LD_R_R(uint8_t opCode)
{
    uint8_t srcRegIndex = (opCode & 0b00000111);
    uint8_t destRegIndex = (opCode & 0b00111000) >> 3;
    uint8_t srcRegValue = readFromRegister(srcRegIndex);
    writeToRegister(destRegIndex, srcRegValue);
}

void z80::LD_HL_R(uint8_t opCode)
{
    uint8_t targetRegister = opCode & 0b00000111;
    uint8_t value = readFromRegister(targetRegister);
    // Write the value to the bus
    bus->write(getHL(), value);
}

void z80::LD_R_N(uint8_t opCode)
{
    uint8_t destinationRegister = (opCode & 0b00111000) >> 3;
    uint8_t value = fetchImmediate();
    writeToRegister(destinationRegister, value);
}

void z80::LD_R_HL(uint8_t opCode)
{
    uint8_t destinationRegister = (opCode & 0b00111000) >> 3;
    uint8_t value = fetchHL(); // Fecthes what ever is in the location pointed by HL
    writeToRegister(destinationRegister, value);
}

void z80::LD_R_IX_D(uint8_t opCode)
{
    uint8_t destinationRegister = (opCode & 0b00111000) >> 3;
    int8_t displacement = fetchImmediate();       // Signed displacement
    uint8_t value = bus->read(IX + displacement); // FIND A BETTER IMPLEMENTATION FOR IX; HL SO On
    writeToRegister(destinationRegister, value);
}

void z80::LD_R_IY_D(uint8_t opCode)
{
    uint8_t destinationRegister = (opCode & 0b00111000) >> 3;
    int8_t displacement = fetchImmediate(); // Signed displacement
    uint8_t value = bus->read(IY + displacement);
    writeToRegister(destinationRegister, value);
}

void z80::LD_IX_D_R(uint8_t opCode)
{
    uint8_t targetRegister = opCode & 0b00000111;
    uint8_t value = readFromRegister(targetRegister);
    int8_t d = fetchImmediate();
    uint16_t address = IX + d; // FIND A BETTER IMPLEMENTATION FOR IX; HL SO On
    bus->write(address, value);
}

void z80::LD_IY_D_R(uint8_t opCode)
{
    uint8_t targetRegister = opCode & 0b00000111;
    uint8_t value = readFromRegister(targetRegister);
    int8_t d = fetchImmediate();
    uint16_t address = IY + d;
    bus->write(address, value);
}
void z80::LD_HL_N(uint8_t opCode)
{
    uint8_t value = fetchImmediate();
    bus->write(getHL(), value);
}

void z80::LD_IX_D_N(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t value = fetchImmediate();
    uint16_t address = IX + d; // FIND A BETTER IMPLEMENTATION FOR IX; HL SO On
    bus->write(address, value);
}
void z80::LD_IY_D_N(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t value = fetchImmediate();
    uint16_t address = IY + d; // FIND A BETTER IMPLEMENTATION FOR IX; HL SO On
    bus->write(address, value);
}

void z80::LD_A_BC(uint8_t opCode)
{
    uint16_t address = getBC();
    uint8_t value = bus->read(address);
    A = value;
}
void z80::LD_A_DE(uint8_t opCode)
{
    uint16_t address = getDE();
    uint8_t value = bus->read(address);
    A = value;
}
void z80::LD_A_NN(uint8_t opCode)
{
    uint8_t lowByte = fetchImmediate();
    uint8_t highByte = fetchImmediate();
    uint16_t address = (highByte << 8) | lowByte;
    A = bus->read(address);
}
void z80::LD_BC_A(uint8_t opCode)
{
    uint8_t value = A;
    uint16_t address = getBC();
    bus->write(address, value);
}
void z80::LD_DE_A(uint8_t opCode)
{
    uint8_t value = A;
    uint16_t address = getDE();
    bus->write(address, value);
}
void z80::LD_NN_A(uint8_t opCode)
{
    uint8_t value = A;
    uint8_t lowByte = fetchImmediate();
    uint8_t highByte = fetchImmediate();
    uint16_t address = (highByte << 8) | lowByte;
    bus->write(address, value);
}

void z80::LD_A_I(uint8_t opCode)
{
    A = I;
    int8_t signedI = (int8_t)I;
    signedI < 0 ? setFlag(S) : clearFlag(S);
    I == 0x00 ? setFlag(Z) : clearFlag(Z);
    clearFlag(H_flag);
    IFF2 ? setFlag(P) : clearFlag(P);
    // If an interrupt occurs during execution of this instruction, the Parity flag contains a 0.
    clearFlag(N);

     ((A & 0x08) > 0)? setFlag(X) : clearFlag(X);
    ((A & 0x20) > 0) ? setFlag(U) : clearFlag(U);
    
}

void z80::LD_A_R(uint8_t opCode)
{
    A = R;
    int8_t signedR = (int8_t)R;
    signedR < 0 ? setFlag(S) : clearFlag(S);
    R == 0x00 ? setFlag(Z) : clearFlag(Z);
    clearFlag(H_flag);
    IFF2 ? setFlag(P) : clearFlag(P);
    // If an interrupt occurs during execution of this instruction, the Parity flag contains a 0.
    clearFlag(N);

    ((A & 0x08) > 0)? setFlag(X) : clearFlag(X);
    ((A & 0x20) > 0) ? setFlag(U) : clearFlag(U);
}

void z80::LD_I_A(uint8_t opCode)
{
    I = A;
}

void z80::LD_R_A(uint8_t opCode)
{
    R = A;
}

/*****************************************|
 *                                        |
 *         16-Bit Load Group              |
 *                                        |
 *****************************************/

void z80::writeToRegisterPair(uint8_t reg, uint16_t value)
{

    switch (reg)
    {
    case 0:
        B = (value & 0xFF00) >> 8;
        C = (value & 0x00FF);
        break;
    case 1:
        D = (value & 0xFF00) >> 8;
        E = (value & 0x00FF);
        break;
    case 2:
        H = (value & 0xFF00) >> 8;
        L = (value & 0x00FF);
        break;
    case 3:
        SP = value;
        break;
    default:
        break;
    }
}
void z80::setAF(uint16_t value)
{
    A = (value & 0xFF00) >> 8;
    F = (value & 0x00FF);
}
void z80::setBC(uint16_t value)
{
    B = (value & 0xFF00) >> 8;
    C = (value & 0x00FF);
}
void z80::setDE(uint16_t value)
{
    D = (value & 0xFF00) >> 8;
    E = (value & 0x00FF);
}
void z80::setHL(uint16_t value)
{
    H = (value & 0xFF00) >> 8;
    L = (value & 0x00FF);
}
void z80::setAF1(uint16_t value)
{
    A1 = (value & 0xFF00) >> 8;
    F1 = (value & 0x00FF);
}
void z80::setBC1(uint16_t value)
{
    B1 = (value & 0xFF00) >> 8;
    C1 = (value & 0x00FF);
}
void z80::setDE1(uint16_t value)
{
    D1 = (value & 0xFF00) >> 8;
    E1 = (value & 0x00FF);
}
void z80::setHL1(uint16_t value)
{
    H1 = (value & 0xFF00) >> 8;
    L1 = (value & 0x00FF);
}
uint16_t z80::readFromRegisterPair(uint8_t reg)
{

    switch (reg)
    {
    case 0:
        return getBC();
        break;
    case 1:
        return getDE();
        break;
    case 2:
        return getHL();
        break;
    case 3:
        return SP;
        break;
    default:
        return 0;
    }
}
uint16_t z80::readFromRegisterPair2(uint8_t reg)
{

    switch (reg)
    {
    case 0:
        return getBC();
        break;
    case 1:
        return getDE();
        break;
    case 2:
        return IX;
        break;
    case 3:
        return SP;
        break;
    default:
        return 0;
    }
}
uint16_t z80::readFromRegisterPair3(uint8_t reg)
{

    switch (reg)
    {
    case 0:
        return getBC();
        break;
    case 1:
        return getDE();
        break;
    case 2:
        return IY;
        break;
    case 3:
        return SP;
        break;
    default:
        return 0;
    }
}

void z80::LD_DD_NN(uint8_t opCode)
{
    uint8_t destination = (opCode >> 4) & 0x03;
    uint8_t loByte = fetchImmediate();
    uint8_t hiByte = fetchImmediate();

    uint16_t value = (hiByte << 8) | loByte;

    writeToRegisterPair(destination, value);
}
void z80::LD_IX_NN(uint8_t opCode)
{
    uint8_t loByte = fetchImmediate();
    uint8_t hiByte = fetchImmediate();
    uint16_t value = (hiByte << 8) | loByte;
    IX = value;
}

void z80::LD_IXl_N(uint8_t opCode)
{
    uint8_t n = fetchImmediate();
    uint8_t hiByte = (IX >> 8);

    uint16_t value = ((hiByte << 8) + n);
    IX = value;
}
void z80::LD_IXH_N(uint8_t opCode)
{
    uint8_t n = fetchImmediate();
    uint8_t loByte = (IX & 0x00FF);

    uint16_t value = (n << 8) | loByte;
    IX = value;
}
void z80::LD_IY_L_N(uint8_t opCode)
{
    uint8_t n = fetchImmediate();
    uint8_t hiByte = (IY >> 8);

    uint16_t value = ((hiByte << 8) + n);
    IY = value;
}
void z80::LD_IY_H_N(uint8_t opCode)
{
    uint8_t n = fetchImmediate();
    uint8_t loByte = (IY & 0x00FF);

    uint16_t value = (n << 8) | loByte;
    IY = value;
}
void z80::LD_IXH_IXL(uint8_t opCode)
{
    uint8_t loByte = (IX & 0x00FF);
    IX = ((loByte << 8) + loByte);
}
void z80::LD_IYH_IYL(uint8_t opCode)
{
    uint8_t loByte = (IY & 0x00FF);
    IY = ((loByte << 8) + loByte);
}
void z80::LD_IYL_IYH(uint8_t opCode)
{
    uint8_t hiByte = (IY >> 8);
    IY = (hiByte << 8) + hiByte;
}


void z80::LD_IXL_IXH(uint8_t opCode)
{
    uint8_t hiByte = (IX >> 8);
    IX = (hiByte << 8) + hiByte;
}

void z80::LD_IXH_R(uint8_t opCode)
{
    uint8_t src = opCode & 0b00000111;
    uint8_t n = readFromRegister(src);
    uint8_t loByte = (IX & 0x00FF);

    IX = ((n << 8) + loByte);
}

void z80::LD_IXl_R(uint8_t opCode)
{
    uint8_t src = opCode & 0b00000111;
    uint8_t n = readFromRegister(src);
    uint8_t hiByte = (IX >> 8);

    IX = ((hiByte << 8) + n);
}

void z80::LD_IY_H_R(uint8_t opCode)
{
    uint8_t src = opCode & 0b00000111;
    uint8_t n = readFromRegister(src);
    uint8_t loByte = (IY & 0x00FF);

    IY = ((n << 8) + loByte);
}

void z80::LD_IY_L_R(uint8_t opCode)
{
    uint8_t src = opCode & 0b00000111;
    uint8_t n = readFromRegister(src);
    uint8_t hiByte = (IY >> 8);

    IY = ((hiByte << 8) + n);
}

void z80::LD_R_IXH(uint8_t opCode)
{
    uint8_t dest = (opCode & 0b00111000) >> 3;
    uint8_t hiByte = (IX >> 8);

    writeToRegister(dest, hiByte);
}
void z80::LD_R_IXl(uint8_t opCode)
{
    uint8_t dest = (opCode & 0b00111000) >> 3;
    uint8_t loByte = (IX & 0x00FF);

    writeToRegister(dest, loByte);
}

void z80::LD_R_IY_H(uint8_t opCode)
{
    uint8_t dest = (opCode & 0b00111000) >> 3;
    uint8_t hiByte = (IY >> 8);

    writeToRegister(dest, hiByte);
}
void z80::LD_R_IY_L(uint8_t opCode)
{
    uint8_t dest = (opCode & 0b00111000) >> 3;
    uint8_t loByte = (IY & 0x00FF);

    writeToRegister(dest, loByte);
}

void z80::LD_IY_NN(uint8_t opCode)
{
    uint8_t loByte = fetchImmediate();
    uint8_t hiByte = fetchImmediate();
    uint16_t value = (hiByte << 8) | loByte;
    IY = value;
}
// load HL (nn)
void z80::LD_HL_NN(uint8_t opCode)
{
    uint8_t loByte = fetchImmediate();
    uint8_t hiByte = fetchImmediate();
    uint16_t address = (hiByte << 8) | loByte;
    uint16_t address2 = address + 1;
    uint8_t value1 = bus->read(address);
    uint8_t value2 = bus->read(address2);
    H = value2;
    L = value1;
}
void z80::LD_DD_nn(uint8_t opCode)
{
    uint8_t destination = (opCode >> 4) & 0x03;
    uint8_t loByte = fetchImmediate();
    uint8_t hiByte = fetchImmediate();
    uint16_t address = (hiByte << 8) | loByte;
    uint16_t address2 = address + 1;
    uint8_t value1 = bus->read(address);
    uint8_t value2 = bus->read(address2);

    switch (destination)
    {
    case 0:
        B = value2;
        C = value1;
        break;
    case 1:
        D = value2;
        E = value1;
        break;
    case 2:
        H = value2;
        L = value1;
        break;
    case 3:
        SP = uint16_t(value2 << 8) | value1;
        break;
    default:
        break;
    }
}
void z80::LD_IX_NN2(uint8_t opCode)
{
    uint8_t loByte = fetchImmediate();
    uint8_t hiByte = fetchImmediate();
    uint16_t value = (hiByte << 8) | loByte;

    uint16_t address = (hiByte << 8) | loByte;
    uint16_t address2 = address + 1;
    uint8_t value1 = bus->read(address);
    uint8_t value2 = bus->read(address2);

    IX = uint16_t(value2 << 8) | value1;
}
void z80::LD_IY_NN2(uint8_t opCode)
{
    uint8_t loByte = fetchImmediate();
    uint8_t hiByte = fetchImmediate();
    uint16_t value = (hiByte << 8) | loByte;

    uint16_t address = (hiByte << 8) | loByte;
    uint16_t address2 = address + 1;
    uint8_t value1 = bus->read(address);
    uint8_t value2 = bus->read(address2);
    IY = uint16_t(value2 << 8) | value1;
}
void z80::LD_NN_HL(uint8_t opCode)
{
    uint8_t loByte = fetchImmediate();
    uint8_t hiByte = fetchImmediate();
    uint16_t address = (hiByte << 8) | loByte;
    bus->write(address, L);
    bus->write(address + 1, H);
}
void z80::LD_NN_DD(uint8_t opCode)
{
    uint8_t destination = (opCode >> 4) & 0x03;
    uint8_t loByte = fetchImmediate();
    uint8_t hiByte = fetchImmediate();
    uint16_t address = (hiByte << 8) | loByte;
    uint16_t address2 = address + 1;

    switch (destination)
    {
    case 0:
        bus->write(address2, B);
        bus->write(address, C);
        break;
    case 1:
        bus->write(address2, D);
        bus->write(address, E);
        break;
    case 2:
        bus->write(address2, H);
        bus->write(address, L);
        break;
    case 3:
    {
        uint8_t P = SP & 0xFF;
        uint8_t S = (SP >> 8) & 0xFF;
        bus->write(address2, S);
        bus->write(address, P);
    }

    break;
    default:
        break;
    }
}
void z80::LD_NN_IX(uint8_t opCode)
{
    uint8_t loByte = fetchImmediate();
    uint8_t hiByte = fetchImmediate();
    uint16_t address = (hiByte << 8) | loByte;
    bus->write(address, IX & 0xFF);
    bus->write(address + 1, (IX >> 8) & 0xFF);
}
void z80::LD_NN_IY(uint8_t opCode)
{
    uint8_t loByte = fetchImmediate();
    uint8_t hiByte = fetchImmediate();
    uint16_t address = (hiByte << 8) | loByte;
    bus->write(address, IY & 0xFF);
    bus->write(address + 1, (IY >> 8) & 0xFF);
}
void z80::LD_SP_HL(uint8_t opCode)
{
    uint8_t hiByte = H;
    uint8_t loByte = L;
    uint16_t value = (hiByte << 8) | loByte;
    SP = value;
}
void z80::LD_SP_IX(uint8_t opCode)
{
    SP = IX;
}
void z80::LD_SP_IY(uint8_t opCode)
{
    SP = IY;
}
void z80::PUSH_QQ(uint8_t opCode)
{
    uint8_t destination = (opCode >> 4) & 0x03;
    switch (destination)
    {
    case 0:
        SP--;
        bus->write(SP, B);
        SP--;
        bus->write(SP, C);
        break;
    case 1:
        SP--;
        bus->write(SP, D);
        SP--;
        bus->write(SP, E);
        break;
    case 2:
        SP--;
        bus->write(SP, H);
        SP--;
        bus->write(SP, L);
        break;
    case 3:
        SP--;
        bus->write(SP, A);
        SP--;
        bus->write(SP, F);

        break;
    default:
        break;
    }
}
void z80::PUSH_IX(uint8_t opCode)
{
    uint8_t hi = (IX >> 8) & 0xFF;
    uint8_t lo = IX & 0xFF;
    SP--;
    bus->write(SP, hi);
    SP--;
    bus->write(SP, lo);
}
void z80::PUSH_IY(uint8_t opCode)
{
    uint8_t hi = (IY >> 8) & 0xFF;
    uint8_t lo = IY & 0xFF;
    SP--;
    bus->write(SP, hi);
    SP--;
    bus->write(SP, lo);
}
void z80::POP_QQ(uint8_t opCode)
{
    uint8_t destination = (opCode >> 4) & 0x03;
    switch (destination)
    {
    case 0:
        C = bus->read(SP);
        SP++;
        B = bus->read(SP);
        SP++;
        break;
    case 1:
        E = bus->read(SP);
        SP++;
        D = bus->read(SP);
        SP++;
        break;
    case 2:
        L = bus->read(SP);
        SP++;
        H = bus->read(SP);
        SP++;
        break;
    case 3:
        F = bus->read(SP);
        SP++;
        A = bus->read(SP);
        SP++;
        break;
    default:
        break;
    }
}
void z80::POP_IX(uint8_t opCode)
{
    uint8_t lo = bus->read(SP);
    SP++;
    uint8_t hi = bus->read(SP);
    SP++;
    uint16_t value = (hi << 8) | lo;
    IX = value;
}
void z80::POP_IY(uint8_t opCode)
{
    uint8_t lo = bus->read(SP);
    SP++;
    uint8_t hi = bus->read(SP);
    SP++;
    uint16_t value = (hi << 8) | lo;
    IY = value;
}

/*****************************************|
 *                                        |
 *         Exchange, Block,               |
 *      Transfer and SearchGroup          |
 *                                        |
 *****************************************/

void z80::EX_DE_HL(uint8_t opCode)
{
    uint16_t DE = getDE();
    uint16_t HL = getHL();

    H = (DE >> 8) & 0xFF;
    L = DE & 0xFF;

    D = (HL >> 8) & 0xFF;
    E = HL & 0xFF;
}
void z80::EX_AF_AF1(uint8_t opCode)
{
    uint16_t AF = getAF();
    uint16_t AF1 = getAF1();

    A = ((AF1 & 0xFF00) >> 8);
    F = (AF1 & 0x00FF);

    A1 = ((AF & 0xFF00) >> 8);
    F1 = (AF & 0x00FF);
}

void z80::EXX(uint8_t opCode)
{
    uint16_t temp = getBC();

    B = ((getBC1() & 0xFF00) >> 8);
    C = (getBC1() & 0x00FF);

    B1 = ((temp & 0xFF00) >> 8);
    C1 = (temp & 0x00FF);

    temp = getDE();

    D = ((getDE1() & 0xFF00) >> 8);
    E = (getDE1() & 0x00FF);

    D1 = ((temp & 0xFF00) >> 8);
    E1 = (temp & 0x00FF);

    temp = getHL();

    H = ((getHL1() & 0xFF00) >> 8);
    L = (getHL1() & 0x00FF);

    H1 = ((temp & 0xFF00) >> 8);
    L1 = (temp & 0x00FF);
}

void z80::EX_SP_HL(uint8_t opCode)
{
    uint8_t hi = (getHL() >> 8) & 0xFF;
    uint8_t lo = getHL() & 0xFF;

    L = bus->read(SP);
    H = bus->read(SP + 1);

    bus->write(SP, lo);
    bus->write(SP + 1, hi);
    MPTR = getHL();
}

void z80::EX_SP_IX(uint8_t opCode)
{
    uint8_t hi = (IX >> 8) & 0xFF;
    uint8_t lo = IX & 0xFF;

    uint8_t hiByte = bus->read(SP + 1);
    uint8_t loByte = bus->read(SP);

    IX = (hiByte << 8) | loByte;

    bus->write(SP, lo);
    bus->write(SP + 1, hi);
    MPTR = IX;
}
void z80::EX_SP_IY(uint8_t opCode)
{
    uint8_t hi = (IY >> 8) & 0xFF;
    uint8_t lo = IY & 0xFF;

    uint8_t hiByte = bus->read(SP + 1);
    uint8_t loByte = bus->read(SP);

    IY = (hiByte << 8) | loByte;

    bus->write(SP, lo);
    bus->write(SP + 1, hi);

    MPTR = IY;
}

void z80::LDI(uint8_t opCode)
{
    uint16_t srcAddress = getHL();
    uint16_t destAddress = getDE();

    uint8_t data = bus->read(srcAddress);
    bus->write(destAddress, data);

     uint8_t temp1 = (data + A);

    uint16_t temp;

    temp = getHL();
    temp++;
    writeToRegisterPair(2, temp);

    uint16_t DE_ = getDE();
    DE_++;
    uint8_t hi2 = (DE_ >> 8) & 0xFF;
    uint8_t lo2 = DE_ & 0xFF;

    D = hi2;
    E = lo2;

    uint16_t BC_ = getBC();
    BC_--;
    uint8_t hi = (BC_ >> 8) & 0xFF;
    uint8_t lo = BC_ & 0xFF;

    B = hi;
    C = lo;

    clearFlag(H_flag);
    getBC() != 0 ? setFlag(P) : clearFlag(P);
    clearFlag(N);
    

    ((temp1 & 0x02) > 0) ? setFlag(U) : clearFlag(U);
    ((temp1 & 0x08) > 0) ? setFlag(X) : clearFlag(X);


}

// Interrupts are recognized and two refresh cycles are executed after each data transfer. When the
// BC is set to 0 prior to instruction execution, the instruction loops through 64 KB
void z80::LDIR(uint8_t opCode)
{
    uint16_t srcAddress = getHL();
    uint16_t destAddress = getDE();

    uint8_t data = bus->read(srcAddress);
    bus->write(destAddress, data);

    uint8_t temp = data + A;

    uint16_t HL_ = getHL();
    HL_++;
    uint8_t hi1 = (HL_ >> 8) & 0xFF;
    uint8_t lo1 = HL_ & 0xFF;

    H = hi1;
    L = lo1;

    uint16_t DE_ = getDE();
    DE_++;
    uint8_t hi2 = (DE_ >> 8) & 0xFF;
    uint8_t lo2 = DE_ & 0xFF;

    D = hi2;
    E = lo2;

    uint16_t BC_ = getBC();
    BC_--;
    uint8_t hi = (BC_ >> 8) & 0xFF;
    uint8_t lo = BC_ & 0xFF;

    B = hi;
    C = lo;

    clearFlag(H_flag);
    getBC() != 0 ? setFlag(P) : clearFlag(P);
    clearFlag(N);

    ((temp & 0x02) > 0) ? setFlag(U) : clearFlag(U);

    ((temp & 0x08) > 0) ? setFlag(X) : clearFlag(X);

    if (getBC() != 0)
    {
        PC--;
        PC--;

        MPTR = PC + 1;
    }
}
void z80::LDD(uint8_t opCode)
{
    uint16_t srcAddress = getHL();
    uint16_t destAddress = getDE();

    uint8_t data = bus->read(srcAddress);
    bus->write(destAddress, data);

 uint8_t temp = (data + A);
    uint16_t HL = getHL();
    HL--;
    uint8_t hi1 = (HL >> 8) & 0xFF;
    uint8_t lo1 = HL & 0xFF;

    H = hi1;
    L = lo1;

    uint16_t DE = getDE();
    DE--;
    uint8_t hi2 = (DE >> 8) & 0xFF;
    uint8_t lo2 = DE & 0xFF;

    D = hi2;
    E = lo2;

    uint16_t BC = getBC();
    BC--;
    uint8_t hi = (BC >> 8) & 0xFF;
    uint8_t lo = BC & 0xFF;

    B = hi;
    C = lo;

    clearFlag(H);
    getBC() != 0 ? setFlag(P) : clearFlag(P);
    clearFlag(N);

    ((temp & 0x02) > 0) ? setFlag(U) : clearFlag(U);

    ((temp & 0x08) > 0) ? setFlag(X) : clearFlag(X);

     
}

// When the BC is set to 0, prior to instruction execution, the instruction loops through
// 64 KB
void z80::LDDR(uint8_t opCode)
{
    uint16_t srcAddress = getHL();
    uint16_t destAddress = getDE();

    uint8_t data = bus->read(srcAddress);
    bus->write(destAddress, data);

     uint8_t temp = (data + A);

    uint16_t HL = getHL();
    HL--;
    uint8_t hi1 = (HL >> 8) & 0xFF;
    uint8_t lo1 = HL & 0xFF;

    H = hi1;
    L = lo1;

    uint16_t DE = getDE();
    DE--;
    uint8_t hi2 = (DE >> 8) & 0xFF;
    uint8_t lo2 = DE & 0xFF;

    D = hi2;
    E = lo2;

    uint16_t BC = getBC();
    BC--;
    uint8_t hi = (BC >> 8) & 0xFF;
    uint8_t lo = BC & 0xFF;

    B = hi;
    C = lo;

    clearFlag(H_flag);
    getBC() != 0 ? setFlag(P) : clearFlag(P);
    clearFlag(N);
    ((temp & 0x02) > 0) ? setFlag(U) : clearFlag(U);

    ((temp & 0x08) > 0) ? setFlag(X) : clearFlag(X);

    if (getBC() != 0)
    {
        PC--;
        PC--;
        MPTR = PC+1;
    }
}
void z80::CPI(uint8_t opCode)
{

    uint16_t address = getHL();

    uint8_t value = bus->read(address);

    int8_t diff = ( A- value);

    uint8_t c = isFlagSet(C_flag);

   uint16_t HL_ = getHL();
    HL_++;
    uint8_t hi1 = (HL_ >> 8) & 0xFF;
    uint8_t lo1 = HL_ & 0xFF;

    H = hi1;
    L = lo1;

    uint16_t BC_ = getBC();
    BC_--;
    uint8_t hi = (BC_ >> 8) & 0xFF;
    uint8_t lo = BC_ & 0xFF;

    B = hi;
    C = lo;

    CP_Flags(A, value);

    c ? setFlag(C_flag) : clearFlag(C_flag);
    diff == 0 ? setFlag(Z) : clearFlag(Z);
    getBC() != 0 ? setFlag(P) : clearFlag(P);
    setFlag(N);

    uint8_t temp = A - value - isFlagSet(H_flag);

    ((temp & 0x02) > 0) ? setFlag(U) : clearFlag(U);
    ((temp & 0x08) > 0) ? setFlag(X) : clearFlag(X);

    MPTR++;
}
/*If BC is set to 0 before instruction execution, the instruction loops through 64 KB if no
match is found.

CPI INSTRUCTION TESTS AND IMPLEMENTATION MAY BE INCORRECT!!*/
void z80::CPIR(uint8_t opCode)
{

    uint16_t address = getHL();

    uint8_t value = bus->read(address);

    int8_t temp = ( A- value);

     uint16_t HL_ = getHL();
    HL_++;
    uint8_t hi1 = (HL_ >> 8) & 0xFF;
    uint8_t lo1 = HL_ & 0xFF;

    H = hi1;
    L = lo1;

    uint16_t BC_ = getBC();
    BC_--;
    uint8_t hi = (BC_ >> 8) & 0xFF;
    uint8_t lo = BC_ & 0xFF;

    B = hi;
    C = lo;


    uint8_t c = isFlagSet(C_flag) ? 1 : 0;

    CP_Flags(A, value);

    c ? setFlag(C_flag) : clearFlag(C_flag);

    getBC() != 0 ? setFlag(P) : clearFlag(P);
    setFlag(N);

    int8_t result = A - value -(isFlagSet(H_flag) ? 1 : 0);

    ((temp & 0x02) > 0) ? setFlag(U) : clearFlag(U);

    ((temp & 0x08) > 0) ? setFlag(X) : clearFlag(X);

    if (getBC() != 0 && result != 0)
    {
        PC--;
        PC--;
        MPTR = PC +1;
    }
    else{
        MPTR++;
    }
}
void z80::CPD(uint8_t opCode)
{

    uint16_t address = getHL();

    uint8_t value = bus->read(address);

     int8_t temp = ( A- value);

     uint16_t HL_ = getHL();
    HL_--;
    uint8_t hi1 = (HL_ >> 8) & 0xFF;
    uint8_t lo1 = HL_ & 0xFF;

    H = hi1;
    L = lo1;

    uint16_t BC_ = getBC();
    BC_--;
    uint8_t hi = (BC_ >> 8) & 0xFF;
    uint8_t lo = BC_ & 0xFF;

    B = hi;
    C = lo;


    uint8_t c = isFlagSet(C_flag) ? 1 : 0;

    CP_Flags(A, value);

    c ? setFlag(C_flag) : clearFlag(C_flag);

    getBC() != 0 ? setFlag(P) : clearFlag(P);
    setFlag(N);

    int8_t result = A - value -(isFlagSet(H_flag) ? 1 : 0);

    ((temp & 0x02) > 0) ? setFlag(U) : clearFlag(U);

    ((temp & 0x08) > 0) ? setFlag(X) : clearFlag(X);

    MPTR--;
}
/*Interrupts are recognized and two refresh cycles execute after each data transfer.
When the BC is set to 0,
prior to instruction execution,
the instruction loops through 64 KB if no match is found.*/
void z80::CPDR(uint8_t opCode)
{

    uint16_t address = getHL();

    uint8_t value = bus->read(address);

    uint16_t HL = getHL();
    HL--;
    uint8_t hi1 = (HL >> 8) & 0xFF;
    uint8_t lo1 = HL & 0xFF;

    uint16_t BC = getBC();
    BC--;
    uint8_t hi = (BC >> 8) & 0xFF;
    uint8_t lo = BC & 0xFF;

    B = hi;
    C = lo;

    H = hi1;
    L = lo1;

    
   uint8_t c = isFlagSet(C_flag) ? 1 : 0;

    CP_Flags(A, value);

    c ? setFlag(C_flag) : clearFlag(C_flag);

    getBC() != 0 ? setFlag(P) : clearFlag(P);
    setFlag(N);

    int8_t result = A - value -(isFlagSet(H_flag) ? 1 : 0);

    ((result & 0x02) > 0) ? setFlag(U) : clearFlag(U);

    ((result & 0x08) > 0) ? setFlag(X) : clearFlag(X);

    if (getBC() != 0 && result != 0)
    {
        PC--;
        PC--;
    }
       
}

/*****************************************|
 *                                        |
 *         8-Bit-Arithmetic Group         |
 *                                        |
 ========================================*/
uint8_t z80::Add8_Bit(uint8_t a, uint8_t b)
{
    uint16_t fullSum = a + b;
    uint8_t sum = fullSum & 0xFF;

    clearFlag(N);

    sum == 0 ? setFlag(Z) : clearFlag(Z);

    (sum & 0x80) ? setFlag(S) : clearFlag(S);

    ((a & 0x0F) + (b & 0x0F) > 0x0F) ? setFlag(H_flag) : clearFlag(H_flag);

    (fullSum > 0xFF) ? setFlag(C_flag) : clearFlag(C_flag);

    (((a ^ b) & 0x80) == 0 && ((a ^ sum) & 0x80) != 0) ? setFlag(P) : clearFlag(P);

    ((sum & 0x08) > 0) ? setFlag(X) : clearFlag(X);
    ((sum & 0x20) > 0) ? setFlag(U) : clearFlag(U);

    return sum;
}
uint8_t z80::Sub8_Bit(uint8_t a, uint8_t b)
{
    uint16_t fullSum = a - b;      // Adding the two 8-bit values as 16-bit.
    uint8_t diff = fullSum & 0xFF; // Keep the lower 8 bits

    setFlag(N); // Set the N flag for subtraction

    // Z flag: Set if the result is 0, otherwise reset.
    diff == 0 ? setFlag(Z) : clearFlag(Z);

    // S flag: Set if the result is negative (MSB is 1), otherwise reset.
    (diff & 0x80) > 0 ? setFlag(S) : clearFlag(S);

    // H flag: Set if there was a borrow from bit 4 (half-carry), otherwise reset.
    ((a & 0x0F) - (b & 0x0F) < 0x00) ? setFlag(H_flag) : clearFlag(H_flag);

    // C flag: Set if there is a borrow
    (a < b) ? setFlag(C_flag) : clearFlag(C_flag);

    // P flag: Set if there is an overflow (signed overflow), otherwise reset.
    (((a ^ b) & 0x80) != 0 && ((b ^ diff) & 0x80) == 0) ? setFlag(P) : clearFlag(P);

    ((diff & 0x08) > 0) ? setFlag(X) : clearFlag(X);
    ((diff & 0x20) > 0) ? setFlag(U) : clearFlag(U);

    return diff; // Return the 8-bit result of the subtraction.
}
uint8_t z80::And8_Bit(uint8_t a, uint8_t b)
{

    uint8_t result = a & b;

    clearFlag(N);
    clearFlag(C_flag);
    setFlag(H_flag);
    result == 0x00 ? setFlag(Z) : clearFlag(Z);
    (result & 0x80) > 0 ? setFlag(S) : clearFlag(S);
    isEvenParity(result) ? setFlag(P) : clearFlag(P);
    ((result & 0x08) > 0) ? setFlag(X) : clearFlag(X);
    ((result & 0x20) > 0) ? setFlag(U) : clearFlag(U);
    return result;
}
uint8_t z80::Or8_Bit(uint8_t a, uint8_t b)
{

    uint8_t result = a | b;

    clearFlag(N);
    clearFlag(C_flag);
    clearFlag(H_flag);
    result == 0x00 ? setFlag(Z) : clearFlag(Z);
    (result & 0x80) > 0 ? setFlag(S) : clearFlag(S);
    isEvenParity(result) ? setFlag(P) : clearFlag(P);

    ((result & 0x08) > 0) ? setFlag(X) : clearFlag(X);
    ((result & 0x20) > 0) ? setFlag(U) : clearFlag(U);

    return result;
}
uint8_t z80::Xor8_Bit(uint8_t a, uint8_t b)
{

    uint8_t result = a ^ b;

    clearFlag(N);
    clearFlag(C_flag);
    clearFlag(H_flag);
    result == 0x00 ? setFlag(Z) : clearFlag(Z);
    (result & 0x80) > 0 ? setFlag(S) : clearFlag(S);
    isEvenParity(result) ? setFlag(P) : clearFlag(P);

    ((result & 0x08) > 0) ? setFlag(X) : clearFlag(X);
    ((result & 0x20) > 0) ? setFlag(U) : clearFlag(U);
    return result;
}
uint8_t z80::CP_Flags(uint8_t a, uint8_t b)
{
    uint8_t result = a - b;
    setFlag(N);
    (b > A) ? setFlag(C_flag) : clearFlag(C_flag);
    ((A & 0x0F) - (b & 0x0F) < 0x00) ? setFlag(H_flag) : clearFlag(H_flag);
    result == 0x00 ? setFlag(Z) : clearFlag(Z);
    result > 0x07F ? setFlag(S) : clearFlag(S);
    (((A ^ b) & 0x80) != 0 && ((b ^ result) & 0x80) == 0) ? setFlag(P) : clearFlag(P);

    ((b & 0x08) > 0) ? setFlag(X) : clearFlag(X);
    ((b & 0x20) > 0) ? setFlag(U) : clearFlag(U);
    return result;
}

bool z80::isEvenParity(uint8_t value)
{
    int count = 0;
    while (value)
    {
        count += value & 1; // Increment count if the least significant bit is 1
        value >>= 1;        // Right shift the value by 1 bit
    }
    return (count % 2) == 0;
}

void z80::ADD_A_R(uint8_t opCode)
{
    uint8_t src = opCode & 0b00000111;
    uint8_t srcValue = readFromRegister(src);
    A = Add8_Bit(A, srcValue);
}
void z80::ADD_A_IX_H(uint8_t opCode)
{
    uint8_t n = ((IX & 0xFF00) >> 8);
    A = Add8_Bit(A, n);
}
void z80::ADD_A_IX_L(uint8_t opCode)
{
    uint8_t n = (IX & 0x00FF);
    A = Add8_Bit(A, n);
}
void z80::ADD_A_IY_H(uint8_t opCode)
{
    uint8_t n = ((IY & 0xFF00) >> 8);
    A = Add8_Bit(A, n);
}
void z80::ADD_A_IY_L(uint8_t opCode)
{
    uint8_t n = (IY & 0x00FF);
    A = Add8_Bit(A, n);
}
void z80::SUB_A_IX_H(uint8_t opCode)
{
    uint8_t n = ((IX & 0xFF00) >> 8);
    A = Sub8_Bit(A, n);
}
void z80::SUB_A_IX_L(uint8_t opCode)
{
    uint8_t n = (IX & 0x00FF);
    A = Sub8_Bit(A, n);
}
void z80::SUB_A_IY_H(uint8_t opCode)
{
    uint8_t n = ((IY & 0xFF00) >> 8);
    A = Sub8_Bit(A, n);
}
void z80::SUB_A_IY_L(uint8_t opCode)
{
    uint8_t n = (IY & 0x00FF);
    A = Sub8_Bit(A, n);
}

void z80::ADD_A_N(uint8_t opCode)
{
    uint8_t value = fetchImmediate();
    A = Add8_Bit(A, value);
}
void z80::ADD_A_HL(uint8_t opCode)
{
    uint8_t value = bus->read(getHL());
    A = Add8_Bit(A, value);
}
void z80::ADD_A_IX_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint16_t address = IX + d;
    uint8_t value = bus->read(address);
    A = Add8_Bit(A, value);
}
void z80::ADD_A_IY_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint16_t address = IY + d;
    uint8_t value = bus->read(address);
    A = Add8_Bit(A, value);
}
void z80::ADC_A_s(uint8_t opCode)
{
    uint8_t src = opCode & 0b00000111;
    uint8_t srcValue = readFromRegister(src);
    uint8_t carry = isFlagSet(C_flag) ? 1 : 0;
    srcValue += carry;
    A = Add8_Bit(A, srcValue);
}
void z80::ADC_A_N(uint8_t opCode)
{
    uint8_t value = fetchImmediate();
    uint8_t carry = isFlagSet(C_flag) ? 1 : 0;
    value += carry;
    A = Add8_Bit(A, value);
}
void z80::ADC_A_HL(uint8_t opCode)
{
    uint8_t value = bus->read(getHL());
    uint8_t carry = isFlagSet(C_flag) ? 1 : 0;
    value += carry;
    A = Add8_Bit(A, value);
}
void z80::ADC_A_IX_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint16_t address = IX + d;
    uint8_t value = bus->read(address);
    uint8_t carry = isFlagSet(C_flag) ? 1 : 0;
    value += carry;
    A = Add8_Bit(A, value);
}
void z80::ADC_A_IX_H(uint8_t opCode)
{
    uint8_t n = ((IX & 0xFF00) >> 8);
    uint8_t carry = isFlagSet(C_flag) ? 1 : 0;
    A = Add8_Bit(A, n + carry);
}
void z80::ADC_A_IX_L(uint8_t opCode)
{
    uint8_t n = ((IX & 0x00FF));
    uint8_t carry = isFlagSet(C_flag) ? 1 : 0;
    A = Add8_Bit(A, n + carry);
}
void z80::ADC_A_IY_H(uint8_t opCode)
{
    uint8_t n = ((IY & 0xFF00) >> 8);
    uint8_t carry = isFlagSet(C_flag) ? 1 : 0;
    A = Add8_Bit(A, n + carry);
}
void z80::ADC_A_IY_L(uint8_t opCode)
{
    uint8_t n = ((IY & 0x00FF));
    uint8_t carry = isFlagSet(C_flag) ? 1 : 0;
    A = Add8_Bit(A, n + carry);
}
void z80::SBC_A_IX_H(uint8_t opCode)
{
    uint8_t n = ((IX & 0xFF00) >> 8);
    uint8_t carry = isFlagSet(C_flag) ? 1 : 0;
    A = Sub8_Bit(A, n + carry);
}
void z80::SBC_A_IX_L(uint8_t opCode)
{
    uint8_t n = ((IX & 0x00FF));
    uint8_t carry = isFlagSet(C_flag) ? 1 : 0;
    A = Sub8_Bit(A, n + carry);
}

void z80::SBC_A_IY_H(uint8_t opCode)
{
    uint8_t n = ((IY & 0xFF00) >> 8);
    uint8_t carry = isFlagSet(C_flag) ? 1 : 0;
    A = Sub8_Bit(A, n + carry);
}
void z80::SBC_A_IY_L(uint8_t opCode)
{
    uint8_t n = ((IY & 0x00FF));
    uint8_t carry = isFlagSet(C_flag) ? 1 : 0;
    A = Sub8_Bit(A, n + carry);
}
void z80::ADC_A_IY_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint16_t address = IY + d;
    uint8_t value = bus->read(address);
    uint8_t carry = isFlagSet(C_flag) ? 1 : 0;
    value += carry;
    A = Add8_Bit(A, value);
}
void z80::SUB_A_R(uint8_t opCode)
{
    uint8_t src = opCode & 0b00000111;
    uint8_t srcValue = readFromRegister(src);
    A = Sub8_Bit(A, srcValue);
}
void z80::SUB_A_N(uint8_t opCode)
{
    uint8_t value = fetchImmediate();
    A = Sub8_Bit(A, value);
}
void z80::SUB_A_HL(uint8_t opCode)
{
    uint8_t value = bus->read(getHL());
    A = Sub8_Bit(A, value);
}
void z80::SUB_A_IX_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint16_t address = IX + d;
    uint8_t value = bus->read(address);
    A = Sub8_Bit(A, value);
}
void z80::SUB_A_IY_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint16_t address = IY + d;
    uint8_t value = bus->read(address);
    A = Sub8_Bit(A, value);
}
void z80::SBC_A_S(uint8_t opCode)
{
    uint8_t src = opCode & 0b00000111;
    uint8_t srcValue = readFromRegister(src);
    uint8_t carry = isFlagSet(C_flag) ? 1 : 0;
    srcValue += carry;
    A = Sub8_Bit(A, srcValue);
}
void z80::SBC_A_N(uint8_t opCode)
{
    uint8_t value = fetchImmediate();
    uint8_t carry = isFlagSet(C_flag) ? 1 : 0;
    value += carry;
    A = Sub8_Bit(A, value);
}
void z80::SBC_A_HL(uint8_t opCode)
{
    uint8_t value = bus->read(getHL());
    uint8_t carry = isFlagSet(C_flag) ? 1 : 0;
    value += carry;
    A = Sub8_Bit(A, value);
}
void z80::SBC_A_IX_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint16_t address = IX + d;
    uint8_t value = bus->read(address);
    uint8_t carry = isFlagSet(C_flag) ? 1 : 0;
    value += carry;
    A = Sub8_Bit(A, value);
}
void z80::SBC_A_IY_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint16_t address = IY + d;
    uint8_t value = bus->read(address);
    uint8_t carry = isFlagSet(C_flag) ? 1 : 0;
    value += carry;
    A = Sub8_Bit(A, value);
}
void z80::AND_A_R(uint8_t opCode)
{
    uint8_t src = opCode & 0b00000111;
    uint8_t srcValue = readFromRegister(src);
    A = And8_Bit(A, srcValue);
}
void z80::AND_A_N(uint8_t opCode)
{
    uint8_t value = fetchImmediate();
    A = And8_Bit(A, value);
}
void z80::AND_A_HL(uint8_t opCode)
{
    uint8_t value = bus->read(getHL());
    A = And8_Bit(A, value);
}
void z80::AND_A_IX_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint16_t address = IX + d;
    uint8_t value = bus->read(address);
    A = And8_Bit(A, value);
}

void z80::AND_A_IX_H(uint8_t opCode)
{
    uint8_t n = ((IX & 0xFF00) >> 8);
    A = And8_Bit(A, n);
}
void z80::AND_A_IY_H(uint8_t opCode)
{
    uint8_t n = ((IY & 0xFF00) >> 8);
    A = And8_Bit(A, n);
}
void z80::AND_A_IX_L(uint8_t opCode)
{
    uint8_t n = (IX & 0x00FF);
    A = And8_Bit(A, n);
}
void z80::AND_A_IY_L(uint8_t opCode)
{
    uint8_t n = (IY & 0x00FF);
    A = And8_Bit(A, n);
}

void z80::XOR_A_IX_H(uint8_t opCode)
{
    uint8_t n = ((IX & 0xFF00) >> 8);
    A = Xor8_Bit(A, n);
}
void z80::XOR_A_IY_H(uint8_t opCode)
{
    uint8_t n = ((IY & 0xFF00) >> 8);
    A = Xor8_Bit(A, n);
}
void z80::OR_A_IX_L(uint8_t opCode)
{
    uint8_t n = (IX & 0x00FF);
    A = Or8_Bit(A, n);
}

void z80::OR_A_IX_H(uint8_t opCode)
{
    uint8_t n = ((IX & 0xFF00) >> 8);
    A = Or8_Bit(A, n);
}

void z80::OR_A_IY_L(uint8_t opCode)
{
    uint8_t n = (IY & 0x00FF);
    A = Or8_Bit(A, n);
}

void z80::OR_A_IY_H(uint8_t opCode)
{
    uint8_t n = ((IY & 0xFF00) >> 8);
    A = Or8_Bit(A, n);
}

void z80::CP_IX_H(uint8_t opCode)
{
    uint8_t n = ((IX & 0xFF00) >> 8);
    uint8_t diff = A - n;
    CP_Flags(A, n);
}

void z80::CP_IX_L(uint8_t opCode)
{
    uint8_t n = (IX & 0x00FF);
    uint8_t diff = A - n;
    CP_Flags(A, n);
}


void z80::CP_A_IY_H(uint8_t opCode)
{
    uint8_t n = ((IY & 0xFF00) >> 8);
    uint8_t diff = A - n;
    CP_Flags(A, n);
}

void z80::CP_A_IY_L(uint8_t opCode)
{
    uint8_t n = (IY & 0x00FF);
    uint8_t diff = A - n;
    CP_Flags(A, n);
}

void z80::XOR_A_IX_L(uint8_t opCode)
{
    uint8_t n = (IX & 0x00FF);
    A = Xor8_Bit(A, n);
}

void z80::XOR_A_IY_L(uint8_t opCode)
{
    uint8_t n = (IY & 0x00FF);
    A = Xor8_Bit(A, n);
}
void z80::AND_A_IY_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint16_t address = IY + d;
    uint8_t value = bus->read(address);
    A = And8_Bit(A, value);
}
void z80::OR_A_R(uint8_t opCode)
{
    uint8_t src = opCode & 0b00000111;
    uint8_t srcValue = readFromRegister(src);
    A = Or8_Bit(A, srcValue);
}
void z80::OR_A_N(uint8_t opCode)
{
    uint8_t value = fetchImmediate();
    A = Or8_Bit(A, value);
}
void z80::OR_A_HL(uint8_t opCode)
{
    uint8_t value = bus->read(getHL());
    A = Or8_Bit(A, value);
}
void z80::OR_A_IX_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint16_t address = IX + d;
    uint8_t value = bus->read(address);
    A = Or8_Bit(A, value);
}
void z80::OR_A_IY_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint16_t address = IY + d;
    uint8_t value = bus->read(address);
    A = Or8_Bit(A, value);
}
void z80::XOR_A_R(uint8_t opCode)
{
    uint8_t src = opCode & 0b00000111;
    uint8_t srcValue = readFromRegister(src);
    A = Xor8_Bit(A, srcValue);
}
void z80::XOR_A_N(uint8_t opCode)
{
    uint8_t value = fetchImmediate();
    A = Xor8_Bit(A, value);
}
void z80::XOR_A_HL(uint8_t opCode)
{
    uint8_t value = bus->read(getHL());
    A = Xor8_Bit(A, value);
}
void z80::XOR_A_IX_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint16_t address = IX + d;
    uint8_t value = bus->read(address);
    A = Xor8_Bit(A, value);
}
void z80::XOR_A_IY_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint16_t address = IY + d;
    uint8_t value = bus->read(address);
    A = Xor8_Bit(A, value);
}
void z80::CP_R(uint8_t opCode)
{
    uint8_t src = opCode & 0b00000111;
    uint8_t srcValue = readFromRegister(src);
    CP_Flags(A, srcValue);
}
void z80::CP_N(uint8_t opCode)
{
    uint8_t value = fetchImmediate();
    CP_Flags(A, value);
}

void z80::CP_HL(uint8_t opCode)
{
    uint8_t value = bus->read(getHL());
    CP_Flags(A, value);
}
void z80::CP_IX_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint16_t address = IX + d;
    uint8_t value = bus->read(address);
    CP_Flags(A, value);
}
void z80::CP_IY_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint16_t address = IY + d;
    uint8_t value = bus->read(address);
    CP_Flags(A, value);
}

void z80::IncFlags(uint8_t value, uint8_t result)
{

    clearFlag(N);
    (int8_t) result < 0 ? setFlag(S) : clearFlag(S);
    result == 0 ? setFlag(Z) : clearFlag(Z);
    value == 0x7F ? setFlag(P) : clearFlag(P);
    ((value & 0x0F) + (0x01 & 0x0F) > 0x0F) ? setFlag(H_flag) : clearFlag(H_flag);

    ((result & 0x08) > 0) ? setFlag(X) : clearFlag(X);
    ((result & 0x20) > 0) ? setFlag(U) : clearFlag(U);
}
void z80::DecFlags(uint8_t value, uint8_t result)
{

    setFlag(N);
    (int8_t) result < 0 ? setFlag(S) : clearFlag(S);
    result == 0 ? setFlag(Z) : clearFlag(Z);
    value == 0x80 ? setFlag(P) : clearFlag(P);
    (value & 0x0F) < (0x01 & 0x0F) ? setFlag(H_flag) : clearFlag(H_flag);

    ((result & 0x08) > 0) ? setFlag(X) : clearFlag(X);
    ((result & 0x20) > 0) ? setFlag(U) : clearFlag(U);
}

void z80::INC_R(uint8_t opCode)
{
    uint8_t src = (opCode & 0b00111000) >> 3;
    uint8_t result = readFromRegister(src);
    IncFlags(result, result + 1);
    result++;
    writeToRegister(src, result);
}

void z80::INC_HL(uint8_t opCode)
{
    uint8_t target = bus->read(getHL());
    IncFlags(target, target + 1);
    target++;
    bus->write(getHL(), target);
}
void z80::INC_IX_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t target = bus->read(IX + d);
    IncFlags(target, target + 1);
    target++;
    bus->write(IX + d, target);
}
void z80::INC_IY_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t target = bus->read(IY + d);
    IncFlags(target, target + 1);
    target++;
    bus->write(IY + d, target);
}
void z80::DEC_R(uint8_t opCode)
{
    uint8_t src = (opCode & 0b00111000) >> 3;
    uint8_t result = readFromRegister(src);
    DecFlags(result, result - 1);
    result--;
    writeToRegister(src, result);
}
void z80::DEC_HL(uint8_t opCode)
{
    uint8_t target = bus->read(getHL());
    target--;
    bus->write(getHL(), target);
    DecFlags(target, target);
}
void z80::DEC_IX_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t target = bus->read(IX + d);
    DecFlags(target, target - 1);
    target--;
    bus->write(IX + d, target);
}
void z80::DEC_IY_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t target = bus->read(IY + d);
    DecFlags(target, target - 1);
    target--;
    bus->write(IY + d, target);
}

/*****************************************|
 *                                        |
 *         General-Purpose Arithmetic     |
 *                 and CPU                |
 *             Control Groups             |
 *                                        |
 *****************************************/

void z80::DAA(uint8_t opCode)
{

    int i = 0;
    if (isFlagSet(H_flag) || ((A & 0xF) > 9))
    {
        i++;
    }
    if (isFlagSet(C_flag) || (A > 0x99))
    {
        i += 2;
        setFlag(C_flag);
    }

    if (isFlagSet(N) && !isFlagSet(H_flag))
    {
        clearFlag(H_flag);
    }
    else
    {
        if (isFlagSet(N) && isFlagSet(H_flag))
        {
            (A & 0x0F) < 6 ? setFlag(H_flag) : clearFlag(H_flag);
        }
        else
        {
            (A & 0x0F) >= 0x0A ? setFlag(H_flag) : clearFlag(H_flag);
        }
    }

    switch (i)
    {
    case 1:
        A += isFlagSet(N) ? 0xFA : 0x06; // -6:6
        break;
    case 2:
        A += isFlagSet(N) ? 0xA0 : 0x60; // -0x60:0x60
        break;
    case 3:
        A += isFlagSet(N) ? 0x9A : 0x66; // -0x66:0x66
        break;
    }
    A == 0 ? setFlag(Z) : clearFlag(Z);
    isEvenParity(A) ? setFlag(P) : clearFlag(P);
    ((A & 0x80) > 0) ? setFlag(S) : clearFlag(S);

    ((A & 0x08) > 0) ? setFlag(X) : clearFlag(X);
    ((A & 0x20) > 0) ? setFlag(U) : clearFlag(U);
}
void z80::CPL(uint8_t opCode)
{
    A = ~A;
    setFlag(H_flag);
    setFlag(N);
    ((A & 0x08) > 0) ? setFlag(X) : clearFlag(X);
    ((A & 0x20) > 0) ? setFlag(U) : clearFlag(U);
}
void z80::NEG(uint8_t opCode)
{
    uint8_t value = A;
    A = ~A;
    A++;
    ((A & 0x80) > 0) ? setFlag(S) : clearFlag(S);
    (A == 0) ? setFlag(Z) : clearFlag(Z);
    ((value & 0x0F) + (((~value) + 1) & 0x0F) > 0xF) ? setFlag(H_flag) : clearFlag(H_flag);
    value == 0x80 ? setFlag(P) : clearFlag(P);
    setFlag(N);
    value != 0 ? setFlag(C_flag) : clearFlag(C_flag);

    ((A & 0x08) > 0) ? setFlag(X) : clearFlag(X);
    ((A & 0x20) > 0) ? setFlag(U) : clearFlag(U);
}
void z80::CCF(uint8_t opCode)
{
    bool flag = isFlagSet(C_flag);

    flag ? clearFlag(C_flag) : setFlag(C_flag);
    flag ? setFlag(H_flag) : clearFlag(H_flag);
    clearFlag(N);

    uint8_t x = ((0) ^ (F) | A);

    ((x & 0x08) > 0) ? setFlag(X) : clearFlag(X);
    ((x & 0x20) > 0) ? setFlag(U) : clearFlag(U);
}
void z80::SCF(uint8_t opCode)
{
    setFlag(C_flag);
    clearFlag(H_flag);
    clearFlag(N);

    uint8_t x = ((0) ^ (F) | A);

    ((x & 0x08) > 0) ? setFlag(X) : clearFlag(X);
    ((x & 0x20) > 0) ? setFlag(U) : clearFlag(U);
}
void z80::NOP(uint8_t opCode)
{
    // do nothing
}

/*The HALT instruction suspends CPU operation until a subsequent interrupt or reset is
received. While in the HALT state, the processor executes NOPs to maintain memory
refresh logic.*/

// ToDo Make sure this works
void z80::HALT(uint8_t opCode)
{
    if (!halted)
    {
        halted = true; // Set the CPU to the halted state
        PC--;          // Keep the PC at the current HALT instruction
    }
}
void z80::DI(uint8_t opCode)
{
    IFF2 = false;
    IFF1 = false;
}
void z80::EI(uint8_t opCode)
{
    IFF2 = true;
    IFF1 = true;
}
void z80::IM0(uint8_t opCode)
{
    interruptMode = InterruptMode::Mode0;
}
void z80::IM1(uint8_t opCode)
{
    interruptMode = InterruptMode::Mode1;
}
void z80::IM2(uint8_t opCode)
{
    interruptMode = InterruptMode::Mode2;
}

/*****************************************|
 *                                        |
 *         16-Bit-Arithmetic Group        |
 *                                        |
 ========================================*/

uint16_t z80::Add16_Bit(uint16_t a, uint16_t b)
{
    uint32_t fullSum = a + b;
    uint16_t sum = fullSum & 0xFFFF;

    clearFlag(N);

    if (fullSum > 0xFFFF)
    {
        setFlag(C_flag);
    }
    else
    {
        clearFlag(C_flag);
    }

    if (((a & 0x0FFF) + (b & 0x0FFF)) > 0x0FFF)
    {
        setFlag(H_flag);
    }
    else
    {
        clearFlag(H_flag);
    }
    ((sum & 0x0800) > 0) ? setFlag(X) : clearFlag(X);
    ((sum & 0x2000) > 0) ? setFlag(U) : clearFlag(U);

    MPTR = a + 1;

    return sum;
}
uint16_t z80::Add16_BitCarry(uint16_t a, uint16_t b)
{
    uint16_t c = 0;
    isFlagSet(C_flag) ? c = 1 : c = 0;
    uint32_t fullSum = a + b + c;
    uint16_t sum = fullSum & 0xFFFF;

    (sum & 0b1000000000000000) > 0 ? setFlag(S) : clearFlag(S);

    sum == 0 ? setFlag(Z) : clearFlag(Z);
    clearFlag(N);
    if (fullSum > 0xFFFF)
    {
        setFlag(C_flag);
    }
    else
    {
        clearFlag(C_flag);
    }
    if (((a & 0x0FFF) + (b & 0x0FFF)) > 0x0FFF)
    {
        setFlag(H_flag);
    }
    else
    {
        clearFlag(H_flag);
    }
    // Overflow flag (P/V)
    uint16_t hiA = a & 0x8000;     // Extract sign bit of `a`
    uint16_t hiB = b & 0x8000;     // Extract sign bit of `b`
    uint16_t hiSum = sum & 0x8000; // Extract sign bit of the result

    // Set the overflow flag if the signs of `a` and `b` are the same,
    // but the sign of the result is different
    if (((hiA == hiB) && (hiA != hiSum)))
    {
        setFlag(P); // Overflow
    }
    else
    {
        clearFlag(P);
    }

    ((sum & 0x0800) > 0) ? setFlag(X) : clearFlag(X);
    ((sum & 0x2000) > 0) ? setFlag(U) : clearFlag(U);
MPTR = a + 1;
    return sum;
}
uint16_t z80::Sub16_Bit(uint16_t a, uint16_t b)
{
    uint16_t c = 0;
    isFlagSet(C_flag) ? c = 1 : c = 0;
    uint16_t sum = a - b - c;
    

    ((sum & 0x8000) > 0) ? setFlag(S) : clearFlag(S);
    a < (b + c) ? setFlag(C_flag) : clearFlag(C_flag);
    sum == 0 ? setFlag(Z) : clearFlag(Z);
    setFlag(N);

    if ((a & 0xFFF) < (b & 0xFFF) + c)
    {
        setFlag(H_flag);
    }
    else
    {
        clearFlag(H_flag);
    }
    if ((((a ^ (b + c)) & 0x8000) != 0)         // Different sign
        && ((((b + c) ^ (sum)) & 0x8000) == 0)) // Same sign
    {
        setFlag(P);
    }
    else
    {
        clearFlag(P);
    }
((sum & 0x0800) > 0) ? setFlag(X) : clearFlag(X);
    ((sum & 0x2000) > 0) ? setFlag(U) : clearFlag(U);

    MPTR = a + 1;
    return sum;
}
void z80::ADD_HL_SS(uint8_t opCode)
{
    uint8_t src = (opCode & 0b00110000) >> 4;
    uint16_t srcValue = readFromRegisterPair(src);
    uint16_t sum = Add16_Bit(getHL(), srcValue);
    // 2 for HL
    writeToRegisterPair(2, sum);
}
void z80::ADC_HL_SS(uint8_t opCode)
{
    uint8_t src = (opCode & 0b00110000) >> 4;
    uint16_t srcValue = readFromRegisterPair(src);
    uint16_t sum = Add16_BitCarry(getHL(), srcValue);
    writeToRegisterPair(2, sum);
}

void z80::SBC_HL_SS(uint8_t opCode)
{
    uint8_t src = (opCode & 0b00110000) >> 4;
    uint16_t srcValue = readFromRegisterPair(src);

    uint8_t c = isFlagSet(C_flag) ? 1 : 0;
    uint16_t result = Sub16_Bit(getHL(), srcValue);

    writeToRegisterPair(2, result);
}
void z80::ADD_IX_PP(uint8_t opCode)
{
    uint8_t src = (opCode & 0b00110000) >> 4;
    uint16_t srcValue = readFromRegisterPair2(src); // this is for reading from IX and not HL
    uint16_t sum = Add16_Bit(IX, srcValue);
    IX = sum;
}
void z80::ADD_IY_RR(uint8_t opCode)
{
    uint8_t src = (opCode & 0b00110000) >> 4;
    uint16_t srcValue = readFromRegisterPair3(src); // this is for reading from IX and not HL
    MPTR = (IY + 1);
    uint16_t sum = Add16_Bit(IY, srcValue);
    IY = sum;
}
void z80::INC_SS(uint8_t opCode)
{
    uint8_t src = (opCode & 0b00110000) >> 4;

    uint16_t sum = readFromRegisterPair(src);

    sum += 1;

    writeToRegisterPair(src, sum);
}
void z80::INC_IX(uint8_t opCode)
{
    IX += 1;
}
void z80::INC_IXH(uint8_t opCode)
{
    uint8_t hiByte = (IX >> 8);
    uint8_t loByte = IX & 0x00FF;
    uint8_t incVal = (hiByte + 1);
    IX = ((incVal << 8) + loByte);

    IncFlags(hiByte, incVal);
}
void z80::INC_IXl(uint8_t opCode)
{
    uint8_t hiByte = (IX >> 8);
    uint8_t loByte = IX & 0x00FF;
    uint8_t incVal = (loByte + 1);
    IX = ((hiByte << 8) + incVal);

    IncFlags(loByte, incVal);
}
void z80::INC_IY_H(uint8_t opCode)
{
    uint8_t hiByte = (IY >> 8);
    uint8_t loByte = IY & 0x00FF;
    uint8_t incVal = (hiByte + 1);
    IY = ((incVal << 8) + loByte);

    IncFlags(hiByte, incVal);
}
void z80::INC_IY_L(uint8_t opCode)
{
    uint8_t hiByte = (IY >> 8);
    uint8_t loByte = IY & 0x00FF;
    uint8_t incVal = (loByte + 1);
    IY = ((hiByte << 8) + incVal);

    IncFlags(loByte, incVal);
}
void z80::INC_IY(uint8_t opCode)
{
    IY += 1;
}
void z80::DEC_SS(uint8_t opCode)
{
    uint8_t src = (opCode & 0b00110000) >> 4;
    uint16_t sum = readFromRegisterPair(src);
    sum--;
    writeToRegisterPair(src, sum);
}
void z80::DEC_IX(uint8_t opCode)
{
    IX -= 1;
}
void z80::DEC_IXH(uint8_t opCode)
{
    uint8_t hiByte = (IX >> 8);
    uint8_t loByte = IX & 0x00FF;
    uint8_t incVal = (hiByte - 1);
    IX = ((incVal << 8) + loByte);

    DecFlags(hiByte, incVal);
}

void z80::DEC_IXl(uint8_t opCode)
{
    uint8_t hiByte = (IX >> 8);
    uint8_t loByte = IX & 0x00FF;
    uint8_t incVal = (loByte - 1);
    IX = ((hiByte << 8) + incVal);

    DecFlags(hiByte, incVal);
}
void z80::DEC_IY_H(uint8_t opCode)
{
    uint8_t hiByte = (IY >> 8);
    uint8_t loByte = IY & 0x00FF;
    uint8_t incVal = (hiByte - 1);
    IY = ((incVal << 8) + loByte);

    DecFlags(hiByte, incVal);
}

void z80::DEC_IY_L(uint8_t opCode)
{
    uint8_t hiByte = (IY >> 8);
    uint8_t loByte = IY & 0x00FF;
    uint8_t incVal = (loByte - 1);
    IY = ((hiByte << 8) + incVal);

    DecFlags(hiByte, incVal);
}
void z80::DEC_IY(uint8_t opCode)
{
    IY -= 1;
}

/*****************************************|
 *                                        |
 *         Rotate And Shift Group         |
 *                                        |
 ========================================*/

void z80::RLCA(uint8_t opCode)
{

    uint8_t c = 0;

    if ((A & 0b10000000) > 0)
    {
        c = 1;
    }
    A = (A << 1) + c;
    clearFlag(H_flag);
    clearFlag(N);

    c == 1 ? setFlag(C_flag) : clearFlag(C_flag);

    ((A & 0x08) > 0) ? setFlag(X) : clearFlag(X);
    ((A & 0x20) > 0) ? setFlag(U) : clearFlag(U);

}
void z80::RLA(uint8_t opCode)
{
    uint8_t currC = isFlagSet(C_flag); // returns boolean

    uint8_t newC = A & 0b10000000;

    A = (A << 1) + currC;
    clearFlag(H_flag);
    clearFlag(N);

    newC == 1 ? setFlag(C_flag) : clearFlag(C_flag);

    ((A & 0x08) > 0) ? setFlag(X) : clearFlag(X);
    ((A & 0x20) > 0) ? setFlag(U) : clearFlag(U);
}
void z80::RRCA(uint8_t opCode)
{

    uint8_t c = 0;

    if ((A & 0b00000001) > 0)
    {
        c = 0b10000000;
    }

    A = (A >> 1) + c;

    c == 0b10000000 ? setFlag(C_flag) : clearFlag(C_flag);
    clearFlag(H_flag);
    clearFlag(N);

    ((A & 0x08) > 0) ? setFlag(X) : clearFlag(X);
    ((A & 0x20) > 0) ? setFlag(U) : clearFlag(U);
}

void z80::RRA(uint8_t opCode)
{

    uint8_t currC = 0;
    isFlagSet(C_flag) ? currC = 0b10000000 : 0;
    uint8_t newC = A & 0b00000001;
    A = (A >> 1) + currC;

    newC == 1 ? setFlag(C_flag) : clearFlag(C_flag);
    clearFlag(H_flag);
    clearFlag(N);

    ((A & 0x08) > 0) ? setFlag(X) : clearFlag(X);
    ((A & 0x20) > 0) ? setFlag(U) : clearFlag(U);
}
void z80::RLC_R(uint8_t opCode)
{
    uint8_t c = 0;
    uint8_t src = opCode & 0b00000111;
    uint8_t value = readFromRegister(src);
    if ((value & 0b10000000) > 0)
    {
        c = 1;
    }

    value = ((value << 1) + c);
    writeToRegister(src, value);

    c == 1 ? setFlag(C_flag) : clearFlag(C_flag);
    rotateFlags(value);
}

void z80::rotateFlags(uint8_t value)
{
    clearFlag(N);
    clearFlag(H_flag);
    value == 0 ? setFlag(Z) : clearFlag(Z);
    value >= 0x80 ? setFlag(S) : clearFlag(S);
    isEvenParity(value) ? setFlag(P) : clearFlag(P);

    ((value & 0x08) > 0) ? setFlag(X) : clearFlag(X);
    ((value & 0x20) > 0) ? setFlag(U) : clearFlag(U);
}
void z80::RLC_HL(uint8_t opCode)
{
    uint8_t c = 0;
    uint8_t value = bus->read(getHL());
    if ((value & 0b10000000) > 0)
    {
        c = 1;
    }

    value = ((value << 1) + c);
    bus->write(getHL(), value);

    c == 1 ? setFlag(C_flag) : clearFlag(C_flag);
    rotateFlags(value);
}
// This instruction does undocumented stuff the whole thing is like 4 bytes and the d is the third
void z80::RLC_IX_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t dest = fetchImmediate();
    dest = dest & 0b00000111;

    uint8_t c = 0;
    uint8_t value = bus->read(IX + d);
    if ((value & 0b10000000) > 0)
    {
        c = 1;
    }

    value = ((value << 1) + c);

    if (dest != 6)
    {
        writeToRegister(dest, value);
    }

    bus->write(IX + d, value);

    c == 1 ? setFlag(C_flag) : clearFlag(C_flag);
    rotateFlags(value);
}
void z80::RLC_IY_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t dest = fetchImmediate();
    dest = dest & 0b00000111;

    uint8_t c = 0;
    uint8_t value = bus->read(IY + d);
    if ((value & 0b10000000) > 0)
    {
        c = 1;
    }
    value = ((value << 1) + c);
    if (dest != 6)
    {
        writeToRegister(dest, value);
    }

    bus->write(IY + d, value);

    c == 1 ? setFlag(C_flag) : clearFlag(C_flag);
    rotateFlags(value);
}

void z80::RL_R(uint8_t opCode)
{
    uint8_t src = opCode & 0b00000111;
    uint8_t value = readFromRegister(src);

    uint8_t currC = isFlagSet(C_flag);
    uint8_t newC = value & 0b10000000;

    value = (value << 1) + currC;

    writeToRegister(src, value);

    newC > 1 ? setFlag(C_flag) : clearFlag(C_flag);
    rotateFlags(value);
}
void z80::RL_HL(uint8_t opCode)
{

    uint8_t value = bus->read(getHL());

    uint8_t currC = isFlagSet(C_flag);
    uint8_t newC = value & 0b10000000;

    value = (value << 1) + currC;

    bus->write(getHL(), value);

    newC > 1 ? setFlag(C_flag) : clearFlag(C_flag);
    rotateFlags(value);
}
void z80::RL_IX_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t dest = fetchImmediate();
    dest = dest & 0b00000111;
    uint8_t value = bus->read(IX + d);

    uint8_t currC = isFlagSet(C_flag);
    uint8_t newC = value & 0b10000000;

    value = (value << 1) + currC;

    bus->write(IX + d, value);

    if (dest != 6)
    {
        writeToRegister(dest, value);
    }

    newC > 0 ? setFlag(C_flag) : clearFlag(C_flag);
    rotateFlags(value);
}
void z80::RL_IY_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t dest = fetchImmediate();
    dest = dest & 0b00000111;
    uint8_t value = bus->read(IY + d);

    uint8_t currC = isFlagSet(C_flag);
    uint8_t newC = value & 0b10000000;

    value = (value << 1) + currC;

    bus->write(IY + d, value);

    if (dest != 6)
    {
        writeToRegister(dest, value);
    }

    newC > 1 ? setFlag(C_flag) : clearFlag(C_flag);
    rotateFlags(value);
}

void z80::RRC_R(uint8_t opCode)
{
    uint8_t src = opCode & 0b00000111;
    uint8_t value = readFromRegister(src);

    uint8_t c = 0;
    if ((value & 0b00000001) > 0)
    {
        c = 1;
    }

    value = (value >> 1);
    if (c == 1)
    {
        value |= 0b10000000;
    }
    writeToRegister(src, value);

    c == 1 ? setFlag(C_flag) : clearFlag(C_flag);
    rotateFlags(value);
}
void z80::RRC_HL(uint8_t opCode)
{
    uint8_t value = bus->read(getHL());

    uint8_t c = 0;
    if ((value & 0b00000001) > 0)
    {
        c = 1;
    }

    value = value >> 1;
    if (c == 1)
    {
        value |= 0b10000000;
    }
    bus->write(getHL(), value);

    c == 1 ? setFlag(C_flag) : clearFlag(C_flag);

    rotateFlags(value);
}
void z80::RRC_IX_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t dest = fetchImmediate();
    dest = dest & 0b00000111;
    uint8_t value = bus->read(IX + d);

    uint8_t c = 0;
    if ((value & 0b00000001) > 0)
    {
        c = 1;
    }

    value = value >> 1;
    if (c == 1)
    {
        value |= 0b10000000;
    }
    bus->write(IX + d, value);

    if (dest != 6)
    {
        writeToRegister(dest, value);
    }

    c == 1 ? setFlag(C_flag) : clearFlag(C_flag);

    rotateFlags(value);
}
void z80::RRC_IY_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t dest = fetchImmediate();
    dest = opCode & 0b00000111;
    uint8_t value = bus->read(IY + d);

    uint8_t c = 0;
    if ((value & 0b00000001) > 0)
    {
        c = 1;
    }

    value = value >> 1;
    if (c == 1)
    {
        value |= 0b10000000;
    }
    bus->write(IY + d, value);

    if (dest != 6)
    {
        writeToRegister(dest, value);
    }

    c == 1 ? setFlag(C_flag) : clearFlag(C_flag);

    rotateFlags(value);
}

void z80::RR_R(uint8_t opCode)
{
    uint8_t src = opCode & 0b00000111;
    uint8_t value = readFromRegister(src);

    uint8_t currC = isFlagSet(C_flag);
    uint8_t newC = value & 0b00000001;

    value = (value >> 1) + currC;

    writeToRegister(src, value);

    newC > 0 ? setFlag(C_flag) : clearFlag(C_flag);
    rotateFlags(value);
}
void z80::RR_HL(uint8_t opCode)
{
    uint8_t value = bus->read(getHL());

    uint8_t currC = isFlagSet(C_flag);
    uint8_t newC = value & 0b00000001;

    value = (value >> 1);

    if (currC == 1)
    {
        value |= 0b10000000;
    }

    bus->write(getHL(), value);

    newC > 0 ? setFlag(C_flag) : clearFlag(C_flag);
    rotateFlags(value);
}
void z80::RR_IX_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t dest = fetchImmediate();
    dest = dest & 0b00000111;
    uint8_t value = bus->read(IX + d);

    uint8_t currC = isFlagSet(C_flag);
    uint8_t newC = value & 0b00000001;

    value = (value >> 1);

    if (currC == 1)
    {
        value |= 0b10000000;
    }

    bus->write(IX + d, value);

    if (dest != 6)
    {
        writeToRegister(dest, value);
    }

    newC > 0 ? setFlag(C_flag) : clearFlag(C_flag);
    rotateFlags(value);
}
void z80::RR_IY_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t dest = fetchImmediate();
    dest = dest & 0b00000111;
    uint8_t value = bus->read(IY + d);

    uint8_t currC = isFlagSet(C_flag);
    uint8_t newC = value & 0b00000001;

    value = (value >> 1);

    if (currC == 1)
    {
        value |= 0b10000000;
    }

    bus->write(IY + d, value);

    if (dest != 6)
    {
        writeToRegister(dest, value);
    }

    newC > 0 ? setFlag(C_flag) : clearFlag(C_flag);
    rotateFlags(value);
}
void z80::SLA_R(uint8_t opCode)
{
    uint8_t src = opCode & 0b00000111;
    uint8_t value = readFromRegister(src);

    uint8_t newC = value & 0b10000000;

    value = value << 1;

    writeToRegister(src, value);

    newC > 0 ? setFlag(C_flag) : clearFlag(C_flag);
    shiftFlags(value);
}
void z80::SLA_HL(uint8_t opCode)
{
    uint8_t value = bus->read(getHL());

    uint8_t newC = value & 0b10000000;

    value = value << 1;

    bus->write(getHL(), value);

    newC > 0 ? setFlag(C_flag) : clearFlag(C_flag);
    rotateFlags(value);
}

void z80::shiftFlags(uint8_t value)
{
    clearFlag(N);
    clearFlag(H_flag);
    value == 0 ? setFlag(Z) : clearFlag(Z);
    value >= 0x80 ? setFlag(S) : clearFlag(S);
    isEvenParity(value) ? setFlag(P) : clearFlag(P);

    ((value & 0x08) > 0) ? setFlag(X) : clearFlag(X);
    ((value & 0x20) > 0) ? setFlag(U) : clearFlag(U);
}
void z80::SLA_IX_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t dest = fetchImmediate();
    dest = dest & 0b00000111;
    uint8_t value = bus->read(IX + d);

    uint8_t newC = value & 0b10000000;

    value = value << 1;

    bus->write(IX + d, value);

    if (dest != 6)
    {
        writeToRegister(dest, value);
    }

    newC > 0 ? setFlag(C_flag) : clearFlag(C_flag);
    shiftFlags(value);
}
void z80::SLS_IX_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t dest = fetchImmediate();
    dest = dest & 0b00000111;
    uint8_t value = bus->read(IX + d);

    uint8_t newC = value & 0b10000000;

    value = value << 1;
    value |= 0b00000001;

    bus->write(IX + d, value);

    if (dest != 6)
    {
        writeToRegister(dest, value);
    }

    newC > 0 ? setFlag(C_flag) : clearFlag(C_flag);
    shiftFlags(value);
}

void z80::SLS_IY_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t dest = fetchImmediate();
    dest = dest & 0b00000111;
    uint8_t value = bus->read(IY + d);

    uint8_t newC = value & 0b10000000;

    value = value << 1;
    value |= 0b00000001;

    bus->write(IY + d, value);

    if (dest != 6)
    {
        writeToRegister(dest, value);
    }

    newC > 0 ? setFlag(C_flag) : clearFlag(C_flag);
    shiftFlags(value);
}
void z80::SLA_IY_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t dest = fetchImmediate();
    dest = dest & 0b00000111;
    uint8_t value = bus->read(IY + d);

    uint8_t newC = value & 0b10000000;

    value = value << 1;

    bus->write(IY + d, value);

    if (dest != 6)
    {
        writeToRegister(dest, value);
    }

    newC > 0 ? setFlag(C_flag) : clearFlag(C_flag);
    shiftFlags(value);
}

void z80::SRA_R(uint8_t opCode)
{
    uint8_t src = opCode & 0b00000111;
    uint8_t value = readFromRegister(src);

    uint8_t newC = value & 0b00000001;
    uint8_t bit7 = value & 0b10000000;
    value = value >> 1;

    if (bit7)
    {
        value += 0b10000000;
    }

    writeToRegister(src, value);

    newC > 0 ? setFlag(C_flag) : clearFlag(C_flag);
    shiftFlags(value);
}
void z80::SLS_R(uint8_t opCode)
{
    uint8_t src = opCode & 0b00000111;
    uint8_t value = readFromRegister(src);

    uint8_t newC = value & 0b10000000;

    value = value << 1;
    value |= 0b00000001;

    writeToRegister(src, value);

    newC > 0 ? setFlag(C_flag) : clearFlag(C_flag);
    shiftFlags(value);
}

void z80::SLS_HL(uint8_t opCode)
{
    uint8_t value = bus->read(getHL());
    uint8_t newC = value & 0b10000000;
    value = (value << 1);
    value |= 0b00000001; // Set bit 0
    bus->write(getHL(), value);

    newC > 0 ? setFlag(C_flag) : clearFlag(C_flag);
    shiftFlags(value);
}
void z80::SRA_HL(uint8_t opCode)
{

    uint8_t value = bus->read(getHL());

    uint8_t newC = value & 0b00000001;
    uint8_t bit7 = value & 0b10000000;
    value = value >> 1;

    if (bit7)
    {
        value += 0b10000000;
    }

    bus->write(getHL(), value);

    newC > 0 ? setFlag(C_flag) : clearFlag(C_flag);
    shiftFlags(value);
}
void z80::SRA_IX_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t dest = fetchImmediate();
    dest = dest & 0b00000111;
    uint8_t value = bus->read(IX + d);

    uint8_t newC = value & 0b00000001;
    uint8_t bit7 = value & 0b10000000;
    value = value >> 1;

    if (bit7)
    {
        value += 0b10000000;
    }

    bus->write(IX + d, value);

    if (dest != 6)
    {
        writeToRegister(dest, value);
    }

    newC > 0 ? setFlag(C_flag) : clearFlag(C_flag);
    shiftFlags(value);
}
void z80::SRA_IY_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t dest = fetchImmediate();
    dest = dest & 0b00000111;
    uint8_t value = bus->read(IY + d);

    uint8_t newC = value & 0b00000001;
    uint8_t bit7 = value & 0b10000000;
    value = value >> 1;

    if (bit7)
    {
        value += 0b10000000;
    }

    bus->write(IY + d, value);

    if (dest != 6)
    {
        writeToRegister(dest, value);
    }

    newC > 0 ? setFlag(C_flag) : clearFlag(C_flag);
    shiftFlags(value);
}

void z80::SRL_R(uint8_t opCode)
{
    uint8_t src = opCode & 0b00000111;
    uint8_t value = readFromRegister(src);

    uint8_t newC = value & 0b00000001;

    value = value >> 1;

    writeToRegister(src, value);

    newC > 0 ? setFlag(C_flag) : clearFlag(C_flag);
    shiftFlags2(value);
}
void z80::SRL_HL(uint8_t opCode)
{

    uint8_t value = bus->read(getHL());

    uint8_t newC = value & 0b00000001;

    value = value >> 1;

    bus->write(getHL(), value);

    newC > 0 ? setFlag(C_flag) : clearFlag(C_flag);
    shiftFlags2(value);
}

void z80::shiftFlags2(uint8_t value)
{
    clearFlag(N);
    clearFlag(H_flag);
    value == 0 ? setFlag(Z) : clearFlag(Z);
    clearFlag(S);
    isEvenParity(value) ? setFlag(P) : clearFlag(P);

    ((value & 0x08) > 0) ? setFlag(X) : clearFlag(X);
    ((value & 0x20) > 0) ? setFlag(U) : clearFlag(U);
}

void z80::SRL_IX_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t dest = fetchImmediate();
    dest = dest & 0b00000111;
    uint8_t value = bus->read(IX + d);

    uint8_t newC = value & 0b00000001;

    value = value >> 1;

    bus->write(IX + d, value);

    if (dest != 6)
    {
        writeToRegister(dest, value);
    }

    newC > 0 ? setFlag(C_flag) : clearFlag(C_flag);
    shiftFlags2(value);
}
void z80::SRL_IY_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t dest = fetchImmediate();
    dest = dest & 0b00000111;
    uint8_t value = bus->read(IY + d);

    uint8_t newC = value & 0b00000001;
    value = value >> 1;

    bus->write(IY + d, value);

    if (dest != 6)
    {
        writeToRegister(dest, value);
    }

    newC > 0 ? setFlag(C_flag) : clearFlag(C_flag);
    shiftFlags2(value);
}

void z80::RLD(uint8_t opCode)
{

    uint8_t value = bus->read(getHL());
    uint8_t lowNibbleA = (A & 0b00001111);
    uint8_t hiNibbleA = (A & 0b11110000);
    uint8_t lowNibbleValue = (value & 0b00001111);
    uint8_t hiNibbleValue = (value & 0b11110000);

    A = (hiNibbleA + (hiNibbleValue >> 4));
    uint8_t newValue = (lowNibbleValue << 4) + lowNibbleA;

    bus->write(getHL(), newValue);

    clearFlag(N);
    clearFlag(H_flag);
    A == 0 ? setFlag(Z) : clearFlag(Z);
    ((int8_t)A < 0) ? setFlag(S) : clearFlag(S);
    isEvenParity(A) ? setFlag(P) : clearFlag(P);

    ((A & 0x08) > 0)  ? setFlag(X) : clearFlag(X);
    ((A & 0x20) > 0)  ? setFlag(U) : clearFlag(U);
    MPTR = (getHL() + 1);
}
void z80::RRD(uint8_t opCode)
{

    uint8_t value = bus->read(getHL());
    uint8_t lowNibbleA = (A & 0b00001111);
    uint8_t hiNibbleA = (A & 0b11110000);
    uint8_t lowNibbleValue = (value & 0b00001111);
    uint8_t hiNibbleValue = (value & 0b11110000);

    A = (hiNibbleA + lowNibbleValue);
    uint8_t newValue = (lowNibbleA << 4) + (hiNibbleValue >> 4);

    bus->write(getHL(), newValue);

    clearFlag(N);
    clearFlag(H_flag);
    A == 0 ? setFlag(Z) : clearFlag(Z);
    ((int8_t)A < 0) ? setFlag(S) : clearFlag(S);
    isEvenParity(A) ? setFlag(P) : clearFlag(P);

    ((A & 0x08) > 0)  ? setFlag(X) : clearFlag(X);
    ((A & 0x20) > 0)  ? setFlag(U) : clearFlag(U);
    MPTR = (getHL() + 1);
}
/*****************************************|
 *                                        |
 *            Bit Group                   |
 *                                        |
 ========================================*/

void z80::BIT_B_R(uint8_t opCode)
{

    uint8_t src = opCode & 0b00000111;
    uint8_t value = readFromRegister(src);

    uint8_t bit = (opCode & 0b00111000) >> 3;

    uint8_t result = value & (1 << bit);

    clearFlag(N);
    setFlag(H_flag);
    result == 0 ? setFlag(Z) : clearFlag(Z);
    bit == 7 && result != 0 ? setFlag(S) : clearFlag(S);
    result == 0 ? setFlag(P) : clearFlag(P);

    ((value & 0x0008) > 0) ? setFlag(X) : clearFlag(X);
    ((value & 0x0020) > 0) ? setFlag(U) : clearFlag(U);
}
void z80::BIT_B_HL(uint8_t opCode)
{
    uint8_t value = bus->read(getHL());

    uint8_t bit = (opCode & 0b00111000) >> 3;

    uint8_t result = value & (1 << bit);
  

    clearFlag(N);
    setFlag(H_flag);
    result == 0 ? setFlag(Z) : clearFlag(Z);
    bit == 7 && result != 0 ? setFlag(S) : clearFlag(S);
    result == 0 ? setFlag(P) : clearFlag(P);

    ((MPTR & 0x0800) > 0) ? setFlag(X) : clearFlag(X);
    ((MPTR & 0x2000) > 0) ? setFlag(U) : clearFlag(U);
}
void z80::BIT_B_IX_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t bit = fetchImmediate();
    bit = (bit & 0b00111000) >> 3;
    uint8_t value = bus->read(IX + d);

    MPTR = (IX + d);

    uint8_t result = value & (1 << bit);

    clearFlag(N);
    setFlag(H_flag);
    result == 0 ? setFlag(Z) : clearFlag(Z);
    bit == 7 && result != 0 ? setFlag(S) : clearFlag(S);
    result == 0 ? setFlag(P) : clearFlag(P);

    ((MPTR & 0x0800) > 0) ? setFlag(X) : clearFlag(X);
    ((MPTR & 0x2000) > 0) ? setFlag(U) : clearFlag(U);
}
void z80::BIT_B_IY_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t dest = fetchImmediate();
    dest = dest & 0b00000111;
    uint8_t value = bus->read(IY + d);

    uint8_t bit = (opCode & 0b00111000) >> 3;

    uint8_t result = value & (1 << bit);

    MPTR = (IY + d);

    clearFlag(N);
    setFlag(H_flag);
    result == 0 ? setFlag(Z) : clearFlag(Z);
    bit == 7 && result != 0 ? setFlag(S) : clearFlag(S);
    result == 0 ? setFlag(P) : clearFlag(P);
    ((MPTR & 0x0800) > 0) ? setFlag(X) : clearFlag(X);
    ((MPTR & 0x2000) > 0) ? setFlag(U) : clearFlag(U);
}
void z80::SET_B_R(uint8_t opCode)
{

    uint8_t src = opCode & 0b00000111;
    uint8_t value = readFromRegister(src);

    uint8_t bit = (opCode & 0b00111000) >> 3;
    value |= (1 << bit);

    writeToRegister(src, value);
}
void z80::SET_B_HL(uint8_t opCode)
{
    uint8_t value = bus->read(getHL());

    uint8_t bit = (opCode & 0b00111000) >> 3;
    value |= (1 << bit);

    bus->write(getHL(), value);
}
void z80::SET_B_IX_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t dest = fetchImmediate();
    dest = dest & 0b00000111;
    uint8_t value = bus->read(IX + d);
    MPTR = IX +d;
    uint8_t bit = (opCode & 0b00111000) >> 3;
    value |= (1 << bit);

    if (dest != 6)
    {
        writeToRegister(dest, value);
    }

    bus->write(IX + d, value);
}
void z80::SET_B_IY_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t dest = fetchImmediate();
    dest = dest & 0b00000111;
    uint8_t value = bus->read(IY + d);
    MPTR = IY +d; 
    uint8_t bit = (opCode & 0b00111000) >> 3;
    value |= (1 << bit);

    if (dest != 6)
    {
        writeToRegister(dest, value);
    }

    bus->write(IY + d, value);
}
void z80::RES_B_R(uint8_t opCode)
{
    uint8_t src = opCode & 0b00000111;
    uint8_t value = readFromRegister(src);

    uint8_t bit = (opCode & 0b00111000) >> 3;
    value &= ~(1 << bit);

    writeToRegister(src, value);
}
void z80::RES_B_HL(uint8_t opCode)
{
    uint8_t value = bus->read(getHL());

    uint8_t bit = (opCode & 0b00111000) >> 3;
    value &= ~(1 << bit);

    bus->write(getHL(), value);
}
void z80::RES_B_IX_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t dest = fetchImmediate();
    dest = dest & 0b00000111;
    uint8_t value = bus->read(IX + d);

    uint8_t bit = (opCode & 0b00111000) >> 3;
    value &= ~(1 << bit);

    if (dest != 6)
    {
        writeToRegister(dest, value);
    }

    bus->write(IX + d, value);
}
void z80::RES_B_IY_D(uint8_t opCode)
{
    int8_t d = fetchImmediate();
    uint8_t dest = fetchImmediate();
    dest = dest & 0b00000111;
    uint8_t value = bus->read(IY + d);

    uint8_t bit = (opCode & 0b00111000) >> 3;
    value &= ~(1 << bit);

    if (dest != 6)
    {
        writeToRegister(dest, value);
    }

    bus->write(IY + d, value);
}
/*****************************************|
 *                                        |
 *            Jump Group                  |
 *                                        |
 ========================================*/

void z80::JP_NN(uint8_t opCode)
{
    uint8_t loByte = fetchImmediate();
    uint8_t hiByte = fetchImmediate();
    uint16_t address = (hiByte << 8) + loByte;
    PC = address - 1; // This -1 is done because the execute automatically moves to the next PC
    MPTR = address;
}
void z80::JP_CC_NN(uint8_t opCode)
{
    uint8_t loByte = fetchImmediate();
    uint8_t hiByte = fetchImmediate();
    uint16_t address = (hiByte << 8) + loByte;
    uint8_t cc = (opCode & 0b00111000) >> 3;
    if (evaluateCC(cc))
    {
        PC = address - 1; // This -1 is done because the execute automatically moves to the next PC
    }
    MPTR = address;
}
bool z80::evaluateCC(uint8_t result)
{

    switch (result)
    {
    case 0:
        return !isFlagSet(Z); // Non-Zero (NZ)
    case 1:
        return isFlagSet(Z); // Zero (Z)
    case 2:
        return !isFlagSet(C_flag); // No Carry (NC)
    case 3:
        return isFlagSet(C_flag); // Carry (C)
    case 4:
        return !isFlagSet(P); // Parity Odd (PO)
    case 5:
        return isFlagSet(P); // Parity Even (PE)
    case 6:
        return !isFlagSet(S); // Sign Positive (P)
    case 7:
        return isFlagSet(S); // Sign Negative (M)
    default:
        return false;
    }
}

void z80::JR_E(uint8_t opCode)
{
    int8_t e = fetchImmediate();
    PC = PC + (uint16_t)e;
    MPTR = PC;
}

void z80::JR_C_E(uint8_t opCode)
{
    int8_t e = fetchImmediate();

    if (isFlagSet(C_flag))
    {
        PC = PC + (uint16_t)e;
        MPTR = PC;
    }
}
void z80::JR_NC_E(uint8_t opCode)
{
    int8_t e = fetchImmediate();

    if (!isFlagSet(C_flag))
    {
        PC = PC + (uint16_t)e;
        MPTR = PC;
    }
}
void z80::JR_Z_E(uint8_t opCode)
{
    int8_t e = fetchImmediate();

    if (isFlagSet(Z))
    {
        PC = PC + (uint16_t)e;
        MPTR = PC;
    }
}
void z80::JR_NZ_E(uint8_t opCode)
{
    int8_t e = fetchImmediate();

    if (!isFlagSet(Z))
    {
        PC = (uint16_t)PC + e;
        MPTR = PC;
    }
}
void z80::JP_HL(uint8_t opCode)
{
    PC = getHL() - 1;
}
void z80::JP_IX(uint8_t opCode)
{
    PC = IX - 1;
}
void z80::JP_IY(uint8_t opCode)
{
    PC = IY - 1;
}

void z80::DJNZ_E(uint8_t opCode)
{
    int8_t e = fetchImmediate();

    uint8_t initialB = B;
    B--;

    DecFlags(initialB, B);

    if (!isFlagSet(Z))
    {
        PC = PC + e;
        MPTR = PC;
    }
}

/*****************************************|
 *                                        |
 *        Call and Return Group           |
 *                                        |
 ========================================*/

void z80::pushPC()
{
    uint8_t loByte = (PC+1) & 0xff;
    uint8_t hiByte = ((PC+1) >> 8) & 0xff;

    SP--;
    bus->write(SP, hiByte);
    SP--;
    bus->write(SP, loByte);
}
void z80::popPC()
{
    uint8_t loByte = bus->read(SP);
    SP++;
    uint8_t hiByte = bus->read(SP);
    SP++;
    PC = (hiByte << 8) + loByte;
}
void z80::CALL_NN(uint8_t opCode)
{
    uint8_t loByte = fetchImmediate();
    uint8_t hiByte = fetchImmediate();

    pushPC();

    uint16_t address = (hiByte << 8) + loByte;
    PC = address;
    MPTR = address;
    PC--;
}

void z80::CALL_CC_NN(uint8_t opCode)
{

    uint8_t loByte = fetchImmediate();
    uint8_t hiByte = fetchImmediate();

    uint8_t cc = (opCode & 0b00111000) >> 3;

    
    uint16_t address = (hiByte << 8) + loByte;
    
    if (evaluateCC(cc))
    {
        pushPC();
        PC = address-1;
    }
     MPTR = address;
}
void z80::RET(uint8_t opCode)
{
    popPC();
    PC--;
    MPTR = PC;
}
void z80::RET_CC(uint8_t opCode)
{
    uint8_t cc = (opCode & 0b00111000) >> 3;

    if (evaluateCC(cc))
    {
        popPC();
        MPTR = PC;
        PC--;
    }
}
void z80::RETI(uint8_t opCode)
{
    popPC();
    PC--;
    // toDo send signal to interupt device
}
void z80::RETN(uint8_t opCode)
{
    popPC();
    PC--;
    IFF1 = IFF2;
}
void z80::RST_P(uint8_t opCode)
{
    uint8_t value = (opCode & 0b00111000) >> 3;
    uint16_t address = getPageZeroAddress(value);
    
    pushPC();
    PC = address - 1;
}

uint16_t z80::getPageZeroAddress(uint8_t value)
{
    switch (value)
    {
    case 0:
        return 0x0000;
    case 1:
        return 0x0008;
    case 2:
        return 0x0010;
    case 3:
        return 0x0018;
    case 4:
        return 0x0020;
    case 5:
        return 0x0028;
    case 6:
        return 0x0030;
    case 7:
        return 0x0038;
    default:
        return 0x0000;
    }
}

/*****************************************|
 *                                        |
 *             I/O Group                  |
 *                                        |
 ========================================*/

void z80::IN_A_N(uint8_t opCode)
{
    uint8_t port = fetchImmediate();

    uint16_t address = (A << 8) + port;
    uint8_t n = bus->read(address); // toDO test and finish this
    // A = n;
}
void z80::IN_R_C(uint8_t opCode)
{
     uint8_t n = bus->readIO(getBC());
   
     uint8_t dest = (opCode & 0b00111000) >> 3;
     writeToRegister(dest, n);

    clearFlag(N);
    clearFlag(H_flag);
    isEvenParity(n) ? setFlag(P) : clearFlag(P);
    n == 0 ? setFlag(Z) : clearFlag(Z);
    n > 0x7F ? setFlag(S) : clearFlag(S);

    ((n & 0x08) > 0) ? setFlag(X) : clearFlag(X);
    ((n & 0x20) > 0) ? setFlag(U) : clearFlag(U);
}
void z80::INI(uint8_t opCode)
{
    uint8_t n = bus->readIO(C);
    // bus->writeIO(getHL(), n);

    uint16_t HLq = getHL();
    HLq + 1;
    writeToRegisterPair(2, HLq);
    uint8_t val = B;
    B = B - 1;
    uint8_t decVal = B;

    DecFlags(val, decVal);

    uint8_t k = n + ((C + 1) & 255);
    k > 0xFF ? setFlag(H_flag) : clearFlag(H_flag);
    k > 0xFF ? setFlag(C_flag) : clearFlag(C_flag);

    uint8_t x = ((k & 7) ^ decVal);
    isEvenParity(x) ? setFlag(P) : clearFlag(P);
    ((n & 0x80) > 0) ? setFlag(N) : clearFlag(N);
}
void z80::INIR(uint8_t opCode)
{
}
void z80::IND(uint8_t opCode)
{
}
void z80::INDR(uint8_t opCode)
{
}
void z80::OUT_N_A(uint8_t opCode)
{
    uint8_t port = fetchImmediate();
    bus->writeIO(A, port);
}
void z80::OUT_C_R(uint8_t opCode)
{
    uint8_t src = (opCode & 0b00111000) >> 3;
    uint8_t n = readFromRegister(src);
    bus->writeIO(C, n);
}
void z80::OUTI(uint8_t opCode)
{
}
void z80::OTIR(uint8_t opCode)
{
}
void z80::OUTD(uint8_t opCode)
{
}
void z80::OTDR(uint8_t opCode)
{
}