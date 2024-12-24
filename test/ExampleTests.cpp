#include <gtest/gtest.h>
#include "../main.cpp"
#include "../Bus.cpp"
#include "../Memory.cpp"
#include "../Instruction.cpp"

class EightBitLoadTest : public ::testing::Test
{
protected:
    Memory memory;
    Bus bus;
    z80 cpu;

    EightBitLoadTest() : memory(0x10000), bus(memory), cpu()
    {
        cpu.reset(&bus);
    }
};

TEST_F(EightBitLoadTest, LD_R_R)
{
    cpu.B = 0x42; // Set register B to 0x42 (66 in decimal)
    cpu.C = 0x56; // Set register C to 0x56 (86 in decimal)

    // "LD B, C"
    bus.write(cpu.PC, 0x41); // Opcode for LD B, C
    cpu.execute(bus.read(cpu.PC));
    cpu.PC++;

    ASSERT_EQ(cpu.B, 0x56); // B should now contain the value from C (0x56)
    ASSERT_EQ(cpu.C, 0x56); // C should still contain its original value

    // "LD C, B"
    bus.write(cpu.PC, 0x49); // Opcode for LD C, B
    cpu.execute(bus.read(cpu.PC));
    cpu.PC++;

    ASSERT_EQ(cpu.B, 0x56); // B should still contain its value (0x56)
    ASSERT_EQ(cpu.C, 0x56); // C should now contain the value from B (0x56)

    // Assuming the flag checks as in original test
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag)); // Carry flag
    ASSERT_TRUE(cpu.isFlagSet(z80::N));      // Add/Subtract flag not set
    ASSERT_TRUE(cpu.isFlagSet(z80::P));      // Parity/Overflow flag
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag)); // Half Carry flag
    ASSERT_TRUE(cpu.isFlagSet(z80::Z));      // Zero flag
}

TEST_F(EightBitLoadTest, LD_R_N)
{
    uint8_t opcodeB = 0x0E;            // Opcode for "LD C, n"
    uint8_t immediateB = 0x56;         // Immediate value to load into register C
    bus.write(cpu.PC, opcodeB);        // Write opcode to memory at current PC
    bus.write(cpu.PC + 1, immediateB); // Write immediate value to memory at next byte
    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(cpu.C, 0x56);
}
TEST_F(EightBitLoadTest, LD_R_HL)
{
    uint8_t opCode = 0x7E;
    cpu.H = 0x20;
    cpu.L = 0x00;
    uint8_t valueInMemory = 0x42;
    bus.write(0x2000, valueInMemory);
    bus.write(cpu.PC, opCode);
    cpu.execute(bus.read(cpu.PC));

    uint8_t opCodeB = 0x6E;
    cpu.H = 0x10;
    cpu.L = 0x00;
    uint8_t valueInMemoryB = 0x40;
    bus.write(0x1000, valueInMemoryB);
    bus.write(cpu.PC, opCodeB);
    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(cpu.A, 0x42);
    ASSERT_EQ(cpu.L, 0x40);
}

TEST_F(EightBitLoadTest, LD_HL_R)
{

    uint8_t opCode = 0x71;
    cpu.C = 0x40;
    cpu.H = 0x11;
    cpu.L = 0x23;
    bus.write(cpu.PC, opCode);
    cpu.execute(bus.read(cpu.PC));
    ASSERT_EQ(bus.read(0x1123), 0x40);
}

TEST_F(EightBitLoadTest, LD_R_IX_D)
{
    uint8_t opCode = 0xDD;
    uint8_t instruction = 0x46;
    cpu.IX = 0x3000;

    uint8_t displacement = 0x10;

    bus.write(cpu.PC, opCode);
    bus.write(cpu.PC + 1, instruction);
    bus.write(cpu.PC + 2, displacement);
    bus.write(0x3000 + displacement, 0x50);

    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(cpu.B, 0x50);
}

TEST_F(EightBitLoadTest, LD_R_IY_D)
{
    uint8_t opCode = 0xFD;
    uint8_t instruction = 0x46;
    cpu.IY = 0x3000;

    int8_t displacement = 0x10;

    bus.write(cpu.PC, opCode);
    bus.write(cpu.PC + 1, instruction);
    bus.write(cpu.PC + 2, displacement);
    bus.write(0x3000 + displacement, 0x50);

    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(cpu.B, 0x50);
}

TEST_F(EightBitLoadTest, LD_IX_D_R)
{
    uint8_t opCode = 0xDD;
    uint8_t instruction = 0x70;
    int8_t displacement = 0x10;
    cpu.B = 0x40;
    cpu.IX = 0x2000;
    bus.write(cpu.PC, opCode);
    bus.write(cpu.PC + 1, instruction);
    bus.write(cpu.PC + 2, displacement);
    cpu.execute(bus.read(cpu.PC));
    ASSERT_EQ(bus.read(0x2000 + displacement), 0x40);
}

TEST_F(EightBitLoadTest, LD_IY_D_R)
{
    uint8_t opCode = 0xFD;
    uint8_t instruction = 0x70;
    int8_t displacement = 0x10;
    cpu.B = 0x40;
    cpu.IY = 0x2000;
    bus.write(cpu.PC, opCode);
    bus.write(cpu.PC + 1, instruction);
    bus.write(cpu.PC + 2, displacement);
    cpu.execute(bus.read(cpu.PC));
    ASSERT_EQ(bus.read(0x2000 + displacement), 0x40);

    ASSERT_EQ(cpu.PC, 3);
}

TEST_F(EightBitLoadTest, LD_HL_n)
{
    uint8_t opCode = 0x36;         // OpCode for LD (HL), n
    uint16_t hlAddress = 0x4444;   // HL register points to this address
    uint8_t immediateValue = 0x55; // Value to load into memory

    cpu.H = 0x44;
    cpu.L = 0x44;

    // Write the instruction and operand to memory
    bus.write(cpu.PC, opCode);             // Write OpCode
    bus.write(cpu.PC + 1, immediateValue); // Write immediate value (n)

    // Execute the instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify that the value was written to the correct memory address
    ASSERT_EQ(bus.read(hlAddress), immediateValue);

    // Verify that the Program Counter (PC) advanced by 2 (1 byte for opcode, 1 for immediate value)
    ASSERT_EQ(cpu.PC, 2);
}

TEST_F(EightBitLoadTest, LD_IX_D_n)
{
    uint8_t opCode = 0xDD;         // Opcode prefix for IX-indexed instructions
    uint8_t instruction = 0x36;    // Opcode for LD (IX+d), n
    int8_t displacement = 0x05;    // Displacement d (5h in this case)
    uint8_t immediateValue = 0x5A; // Value to load into memory

    cpu.IX = 0x219A;                       // Set the IX register to 0x219A
    bus.write(cpu.PC, opCode);             // Write the opcode to memory
    bus.write(cpu.PC + 1, instruction);    // Write the instruction byte to memory
    bus.write(cpu.PC + 2, displacement);   // Write the displacement byte to memory
    bus.write(cpu.PC + 3, immediateValue); // Write the immediate value (n) to memory

    cpu.execute(bus.read(cpu.PC)); // Execute the instruction

    // Verify that the value was written to the correct memory address
    ASSERT_EQ(bus.read(0x219A + displacement), immediateValue);

    // Verify that the Program Counter (PC) advanced by 4 (1 for opcode, 1 for instruction, 1 for displacement, 1 for immediate value)
    ASSERT_EQ(cpu.PC, 4);
}
TEST_F(EightBitLoadTest, LD_IY_D_n)
{
    uint8_t opCode = 0xFD;         // Opcode prefix for IX-indexed instructions
    uint8_t instruction = 0x36;    // Opcode for LD (IY+d), n
    int8_t displacement = 0x05;    // Displacement d (5h in this case)
    uint8_t immediateValue = 0x5A; // Value to load into memory

    cpu.IY = 0x219A;                       // Set the IY register to 0x219A
    bus.write(cpu.PC, opCode);             // Write the opcode to memory
    bus.write(cpu.PC + 1, instruction);    // Write the instruction byte to memory
    bus.write(cpu.PC + 2, displacement);   // Write the displacement byte to memory
    bus.write(cpu.PC + 3, immediateValue); // Write the immediate value (n) to memory

    cpu.execute(bus.read(cpu.PC)); // Execute the instruction

    // Verify that the value was written to the correct memory address
    ASSERT_EQ(bus.read(0x219A + displacement), immediateValue);

    // Verify that the Program Counter (PC) advanced by 4 (1 for opcode, 1 for instruction, 1 for displacement, 1 for immediate value)
    ASSERT_EQ(cpu.PC, 4);
}
TEST_F(EightBitLoadTest, LD_A_BC)
{
    uint8_t opCode = 0x0A;       // Opcode for LD A, (BC)
    uint16_t bcAddress = 0x4747; // BC register points to this address
    uint8_t memoryValue = 0x12;  // Value at memory location 0x4747

    cpu.B = 0x47;                      // Set the high byte of BC (B register)
    cpu.C = 0x47;                      // Set the low byte of BC (C register)
    bus.write(bcAddress, memoryValue); // Write value 0x12 to memory location 0x4747

    bus.write(cpu.PC, opCode);     // Write the opcode to memory
    cpu.execute(bus.read(cpu.PC)); // Execute the instruction

    // Verify that the value from memory (0x12) was loaded into the accumulator (A)
    ASSERT_EQ(cpu.A, memoryValue);
}
TEST_F(EightBitLoadTest, LD_A_DE)
{
    uint8_t opCode = 0x1A;       // Opcode for LD A, (BC)
    uint16_t bcAddress = 0x4747; // BC register points to this address
    uint8_t memoryValue = 0x12;  // Value at memory location 0x4747

    cpu.D = 0x47;                      // Set the high byte of BC (B register)
    cpu.E = 0x47;                      // Set the low byte of BC (C register)
    bus.write(bcAddress, memoryValue); // Write value 0x12 to memory location 0x4747

    bus.write(cpu.PC, opCode);     // Write the opcode to memory
    cpu.execute(bus.read(cpu.PC)); // Execute the instruction

    // Verify that the value from memory (0x12) was loaded into the accumulator (A)
    ASSERT_EQ(cpu.A, memoryValue);
}

TEST_F(EightBitLoadTest, LD_A_nn)
{
    uint8_t opCode = 0x3A;      // LD A, (nn) opcode
    uint16_t address = 0x8832;  // The memory address to load from
    uint8_t memoryValue = 0x04; // Value at address 0x8832

    // Set up the memory: Write 0x04 to address 0x8832
    bus.write(address, memoryValue);

    // Write the opcode to the program counter
    bus.write(cpu.PC, opCode);

    // Write the low byte (0x32) and high byte (0x88) of the address to memory
    bus.write(cpu.PC + 1, 0x32); // Low byte of the address
    bus.write(cpu.PC + 2, 0x88); // High byte of the address

    // Execute the instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify that A contains the value from memory at address 0x8832
    ASSERT_EQ(cpu.A, memoryValue); // A should be 0x04
}

TEST_F(EightBitLoadTest, LD_DE_A)
{
    uint8_t opCode = 0x12;     // LD (DE), A opcode
    uint16_t address = 0x1128; // DE register pair contains the address 0x1128
    uint8_t valueInA = 0xA0;   // Value to load into memory (Accumulator A)

    // Set DE register pair to 0x1128
    cpu.D = 0x11;
    cpu.E = 0x28;

    // Set the value of A (Accumulator) to 0xA0
    cpu.A = valueInA;

    // Write the opcode to memory at the program counter (CPU.PC)
    bus.write(cpu.PC, opCode);

    // Execute the instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify that the memory at address 0x1128 contains the value from A (0xA0)
    ASSERT_EQ(bus.read(address), valueInA); // Memory at DE address should contain 0xA0
}
TEST_F(EightBitLoadTest, LD_BC_A)
{
    uint8_t opCode = 0x02;     // LD (DE), A opcode
    uint16_t address = 0x1128; // DE register pair contains the address 0x1128
    uint8_t valueInA = 0xA0;   // Value to load into memory (Accumulator A)

    // Set DE register pair to 0x1128
    cpu.B = 0x11;
    cpu.C = 0x28;

    // Set the value of A (Accumulator) to 0xA0
    cpu.A = valueInA;

    // Write the opcode to memory at the program counter (CPU.PC)
    bus.write(cpu.PC, opCode);

    // Execute the instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify that the memory at address 0x1128 contains the value from A (0xA0)
    ASSERT_EQ(bus.read(address), valueInA); // Memory at DE address should contain 0xA0
}

TEST_F(EightBitLoadTest, LD_nn_A)
{
    uint8_t opCode = 0x32;     // LD (nn), A opcode
    uint16_t address = 0x3141; // The memory address nn = 0x3141
    uint8_t valueInA = 0xD7;   // The value in the Accumulator A (0xD7)

    // Set the value of A (Accumulator) to 0xD7
    cpu.A = valueInA;

    // Write the opcode for LD (nn), A at the current program counter
    bus.write(cpu.PC, opCode);

    // Write the low byte (0x41) and high byte (0x31) of the address to memory
    bus.write(cpu.PC + 1, 0x41); // Low byte of the address
    bus.write(cpu.PC + 2, 0x31); // High byte of the address

    // Execute the instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify that the memory at address 0x3141 contains the value from A (0xD7)
    ASSERT_EQ(bus.read(address), valueInA); // Memory at address nn should contain 0xD7
}

TEST_F(EightBitLoadTest, testLD_A_I)
{
    // Arrange
    cpu.I = 0x80;    // Negative value to test S flag
    cpu.IFF2 = true; // Set IFF2 for P/V flag
    cpu.F = 0xFF;    // Initialize all flags to 1 for proper clearing tests

    cpu.setFlag(z80::C_flag); // Set C flag to 1
    uint8_t initialCFlag = (cpu.F & z80::C_flag);

    // Act
    cpu.LD_A_I(0xED); // Example opcode for the LD A, I instruction

    // Assert
    ASSERT_EQ(cpu.A, cpu.I);                        // Check A is loaded with I
    ASSERT_EQ((cpu.F & z80::S) != 0, true);         // S flag should be set (I is negative)
    ASSERT_EQ((cpu.F & z80::Z) != 0, false);        // Z flag should not be set (I != 0)
    ASSERT_EQ((cpu.F & z80::H_flag) != 0, false);   // H flag should be reset
    ASSERT_EQ((cpu.F & z80::P) != 0, cpu.IFF2);     // P/V flag matches IFF2
    ASSERT_EQ((cpu.F & z80::N) != 0, false);        // N flag should be reset
    ASSERT_EQ((cpu.F & z80::C_flag), initialCFlag); // Ensure C flag remains unchanged

    // Additional test case: Z flag when I is zero
    cpu.I = 0x00;
    cpu.LD_A_I(0xED);
    ASSERT_EQ((cpu.F & z80::Z) != 0, true);  // Z flag should be set when I is 0
    ASSERT_EQ((cpu.F & z80::S) != 0, false); // S flag should not be set (I is not negative)
}
TEST_F(EightBitLoadTest, testLD_A_R)
{
    // Arrange
    cpu.I = 0x80; // Negative value to test S flag
    uint8_t opCode = 0x5F;
    cpu.IFF2 = true; // Set IFF2 for P/V flag
    cpu.F = 0xFF;    // Initialize all flags to 1 for proper clearing tests

    cpu.setFlag(z80::C_flag); // Set C flag to 1
    uint8_t initialCFlag = (cpu.F & z80::C_flag);

    bus.write(cpu.PC, 0xED);
    bus.write(cpu.PC + 1, opCode);
    // Act
    cpu.execute(bus.read(cpu.PC)); // Example opcode for the LD A, I instruction

    // Assert
    ASSERT_EQ(cpu.A, cpu.R);                        // Check A is loaded with I
    ASSERT_EQ((cpu.F & z80::S) != 0, true);         // S flag should be set (I is negative)
    ASSERT_EQ((cpu.F & z80::Z) != 0, false);        // Z flag should not be set (I != 0)
    ASSERT_EQ((cpu.F & z80::H_flag) != 0, false);   // H flag should be reset
    ASSERT_EQ((cpu.F & z80::P) != 0, cpu.IFF2);     // P/V flag matches IFF2
    ASSERT_EQ((cpu.F & z80::N) != 0, false);        // N flag should be reset
    ASSERT_EQ((cpu.F & z80::C_flag), initialCFlag); // Ensure C flag remains unchanged

    // Additional test case: Z flag when I is zero
    cpu.I = 0x00;
    cpu.LD_A_R(0xED);
    ASSERT_EQ((cpu.F & z80::Z) != 0, true);  // Z flag should be set when I is 0
    ASSERT_EQ((cpu.F & z80::S) != 0, false); // S flag should not be set (I is not negative)
}
TEST_F(EightBitLoadTest, LD_I_A)
{
    uint8_t opcodeB = 0xED;            // Opcode for "LD C, n"
    uint8_t immediateB = 0x47;         // Immediate value to load into register C
    bus.write(cpu.PC, opcodeB);        // Write opcode to memory at current PC
    bus.write(cpu.PC + 1, immediateB); // Write immediate value to memory at next byte
    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(cpu.I, cpu.A);
}
TEST_F(EightBitLoadTest, LD_R_A)

{
    uint8_t opcodeB = 0xED;            // Opcode for "LD C, n"
    uint8_t immediateB = 0x4F;         // Immediate value to load into register C
    bus.write(cpu.PC, opcodeB);        // Write opcode to memory at current PC
    bus.write(cpu.PC + 1, immediateB); // Write immediate value to memory at next byte
    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(cpu.R, cpu.A);
}
class SixteenBitLoadTest : public ::testing::Test
{
protected:
    Memory memory;
    Bus bus;
    z80 cpu;

    SixteenBitLoadTest() : memory(0x10000), bus(memory), cpu()
    {
        cpu.reset(&bus);
    }
};

TEST_F(SixteenBitLoadTest, LD_DD_nn)
{
    // Set the low and high bytes for the 16-bit value nn
    uint8_t nn_low = 0x00;          // Low byte of nn (5000h)
    uint8_t nn_high = 0x50;         // High byte of nn (5000h)
    bus.write(cpu.PC, 0x01);        // Opcode for LD HL, nn
    bus.write(cpu.PC + 1, nn_low);  // Low byte of nn
    bus.write(cpu.PC + 2, nn_high); // High byte of nn
    cpu.execute(bus.read(cpu.PC));
    ASSERT_EQ(cpu.getBC(), 0x5000); // HL should now be 5000h

    uint8_t nn_low1 = 0x01; // Low byte of nn (5000h)
    uint8_t nn_high1 = 0x50;
    bus.write(cpu.PC, 0x11);        // Opcode for LD HL, nn
    bus.write(cpu.PC + 1, nn_low1); // Low byte of nn
    bus.write(cpu.PC + 2, nn_high1);
    cpu.execute(bus.read(cpu.PC));
    ASSERT_EQ(cpu.getDE(), 0x5001);

    uint8_t nn_low2 = 0x02; // Low byte of nn (5000h)
    uint8_t nn_high2 = 0x50;
    bus.write(cpu.PC, 0x21);        // Opcode for LD HL, nn
    bus.write(cpu.PC + 1, nn_low2); // Low byte of nn
    bus.write(cpu.PC + 2, nn_high2);
    cpu.execute(bus.read(cpu.PC));
    ASSERT_EQ(cpu.getHL(), 0x5002);

    uint8_t nn_low3 = 0x03; // Low byte of nn (5000h)
    uint8_t nn_high3 = 0x50;
    bus.write(cpu.PC, 0x31);        // Opcode for LD HL, nn
    bus.write(cpu.PC + 1, nn_low3); // Low byte of nn
    bus.write(cpu.PC + 2, nn_high3);
    cpu.execute(bus.read(cpu.PC));
    ASSERT_EQ(cpu.SP, 0x5003);
}
TEST_F(SixteenBitLoadTest, LD_IX_nn)
{
    // Set the low and high bytes for the 16-bit value nn
    uint8_t nn_low = 0x00; // Low byte of nn (5000h)
    uint8_t nn_high = 0x50;

    bus.write(cpu.PC, 0xDD);
    bus.write(cpu.PC + 1, 0x21);    // Opcode for LD HL, nn
    bus.write(cpu.PC + 2, nn_low);  // Low byte of nn
    bus.write(cpu.PC + 3, nn_high); // High byte of nn
    cpu.execute(bus.read(cpu.PC));
    ASSERT_EQ(cpu.IX, 0x5000); // HL should now be 5000h
}
TEST_F(SixteenBitLoadTest, LD_IY_nn)
{
    // Set the low and high bytes for the 16-bit value nn
    uint8_t nn_low = 0x00; // Low byte of nn (5000h)
    uint8_t nn_high = 0x50;
    bus.write(cpu.PC, 0xFD);
    bus.write(cpu.PC + 1, 0x21);    // Opcode for LD HL, nn
    bus.write(cpu.PC + 2, nn_low);  // Low byte of nn
    bus.write(cpu.PC + 3, nn_high); // High byte of nn
    cpu.execute(bus.read(cpu.PC));
    ASSERT_EQ(cpu.IY, 0x5000); // HL should now be 5000h
}
TEST_F(SixteenBitLoadTest, LD_HL_NN)
{
    uint16_t nn = 0x4545;    // Address nn
    uint8_t lowByte = 0x37;  // Memory at nn
    uint8_t highByte = 0xA1; // Memory at nn + 1

    // Write values to memory
    memory.write(nn, lowByte);      // Memory[nn] = 0x37
    memory.write(nn + 1, highByte); // Memory[nn+1] = 0xA1

    // LD HL, (nn) opcode is 0x2A followed by nn (low byte first, then high byte)
    bus.write(cpu.PC, 0x2A);                 // Opcode for LD HL, (nn)
    bus.write(cpu.PC + 1, nn & 0xFF);        // Low byte of nn
    bus.write(cpu.PC + 2, (nn >> 8) & 0xFF); // High byte of nn

    // Execute the instruction
    cpu.execute(bus.read(cpu.PC)); // Execute the instruction at PC

    // Check that HL is loaded correctly
    ASSERT_EQ(cpu.H, highByte);     // H should now contain the value at memory[nn+1]
    ASSERT_EQ(cpu.L, lowByte);      // L should now contain the value at memory[nn]
    ASSERT_EQ(cpu.getHL(), 0xA137); // HL should combine to form 0xA137
}
TEST_F(SixteenBitLoadTest, LD_DD_NN2)
{
    uint16_t nn = 0x4545;    // Address nn
    uint8_t lowByte = 0x37;  // Memory at nn
    uint8_t highByte = 0xA1; // Memory at nn + 1

    // Write values to memory
    memory.write(nn, lowByte);      // Memory[nn] = 0x37
    memory.write(nn + 1, highByte); // Memory[nn+1] = 0xA1

    // LD HL, (nn) opcode is 0x2A followed by nn (low byte first, then high byte)
    bus.write(cpu.PC, 0xED);
    bus.write(cpu.PC + 1, 0x6B);             // Opcode for LD HL, (nn)
    bus.write(cpu.PC + 2, nn & 0xFF);        // Low byte of nn
    bus.write(cpu.PC + 3, (nn >> 8) & 0xFF); // High byte of nn

    // Execute the instruction
    cpu.execute(bus.read(cpu.PC)); // Execute the instruction at PC

    // Check that HL is loaded correctly
    ASSERT_EQ(cpu.H, highByte);     // H should now contain the value at memory[nn+1]
    ASSERT_EQ(cpu.L, lowByte);      // L should now contain the value at memory[nn]
    ASSERT_EQ(cpu.getHL(), 0xA137); // HL should combine to form 0xA137

    uint16_t nn1 = 0x4545;    // Address nn
    uint8_t lowByte1 = 0x36;  // Memory at nn
    uint8_t highByte1 = 0xA1; // Memory at nn + 1

    // Write values to memory
    memory.write(nn1, lowByte1);      // Memory[nn] = 0x37
    memory.write(nn1 + 1, highByte1); // Memory[nn+1] = 0xA1

    // LD HL, (nn) opcode is 0x2A followed by nn (low byte first, then high byte)
    bus.write(cpu.PC, 0xED);
    bus.write(cpu.PC + 1, 0x4B);              // Opcode for LD HL, (nn)
    bus.write(cpu.PC + 2, nn1 & 0xFF);        // Low byte of nn
    bus.write(cpu.PC + 3, (nn1 >> 8) & 0xFF); // High byte of nn

    // Execute the instruction
    cpu.execute(bus.read(cpu.PC)); // Execute the instruction at PC

    // Check that HL is loaded correctly
    ASSERT_EQ(cpu.B, highByte1); // H should now contain the value at memory[nn+1]
    ASSERT_EQ(cpu.C, lowByte1);  // L should now contain the value at memory[nn]
    ASSERT_EQ(cpu.getBC(), 0xA136);

    uint16_t nn2 = 0x4545;    // Address nn
    uint8_t lowByte2 = 0x35;  // Memory at nn
    uint8_t highByte2 = 0xA1; // Memory at nn + 1

    // Write values to memory
    memory.write(nn2, lowByte2);      // Memory[nn] = 0x37
    memory.write(nn2 + 1, highByte2); // Memory[nn+1] = 0xA1

    // LD HL, (nn) opcode is 0x2A followed by nn (low byte first, then high byte)
    bus.write(cpu.PC, 0xED);
    bus.write(cpu.PC + 1, 0x7B);             // Opcode for LD HL, (nn)
    bus.write(cpu.PC + 2, nn & 0xFF);        // Low byte of nn
    bus.write(cpu.PC + 3, (nn >> 8) & 0xFF); // High byte of nn

    // Execute the instruction
    cpu.execute(bus.read(cpu.PC)); // Execute the instruction at PC

    // Check that HL is loaded correctly

    ASSERT_EQ(cpu.SP, 0xA135);
}
TEST_F(SixteenBitLoadTest, LD_IX_NN2)
{
    uint16_t nn = 0x4545;    // Address nn
    uint8_t lowByte = 0x37;  // Memory at nn
    uint8_t highByte = 0xA1; // Memory at nn + 1

    // Write values to memory
    memory.write(nn, lowByte);      // Memory[nn] = 0x37
    memory.write(nn + 1, highByte); // Memory[nn+1] = 0xA1

    bus.write(cpu.PC, 0xDD);
    bus.write(cpu.PC + 1, 0x2A);             // Opcode for LD HL, (nn)
    bus.write(cpu.PC + 2, nn & 0xFF);        // Low byte of nn
    bus.write(cpu.PC + 3, (nn >> 8) & 0xFF); // High byte of nn
    // Execute the instruction
    cpu.execute(bus.read(cpu.PC)); // Execute the instruction at PC
    ASSERT_EQ(cpu.IX, 0xA137);     // HL should combine to form 0xA137
}
TEST_F(SixteenBitLoadTest, LD_IY_NN2)
{
    uint16_t nn = 0x4545;    // Address nn
    uint8_t lowByte = 0x37;  // Memory at nn
    uint8_t highByte = 0xA1; // Memory at nn + 1

    // Write values to memory
    memory.write(nn, lowByte);      // Memory[nn] = 0x37
    memory.write(nn + 1, highByte); // Memory[nn+1] = 0xA1

    bus.write(cpu.PC, 0xFD);
    bus.write(cpu.PC + 1, 0x2A);             // Opcode for LD HL, (nn)
    bus.write(cpu.PC + 2, nn & 0xFF);        // Low byte of nn
    bus.write(cpu.PC + 3, (nn >> 8) & 0xFF); // High byte of nn
    // Execute the instruction
    cpu.execute(bus.read(cpu.PC)); // Execute the instruction at PC
    ASSERT_EQ(cpu.IY, 0xA137);     // HL should combine to form 0xA137
}
TEST_F(SixteenBitLoadTest, LD_NN_HL)
{
    // Test with different memory address (nn) and HL register value
    uint16_t address = 0xB229; // Address nn
    uint8_t L = 0x3A;          // L register value
    uint8_t H = 0x48;          // H register value
    cpu.H = H;
    cpu.L = L; // Set HL register to 483Ah
    bus.write(cpu.PC, 0x22);
    bus.write(cpu.PC + 1, 0x29);
    bus.write(cpu.PC + 2, 0xB2);
    // Execute the LD (nn), HL instruction
    cpu.execute(bus.read(cpu.PC)); // 0x22 is the opcode for LD (nn), HL

    // Assert that memory at address nn contains L and at nn + 1 contains H
    ASSERT_EQ(memory.read(address), L);     // Memory[nn] should contain L (0x3A)
    ASSERT_EQ(memory.read(address + 1), H); // Memory[nn + 1] should contain H (0x48)
}

TEST_F(SixteenBitLoadTest, LD_NN_DD)
{
    // Test with different memory address (nn) and HL register value
    uint16_t address = 0xB229; // Address nn
    uint8_t hi = 0x3A;
    uint8_t lo = 0x48;
    cpu.B = hi;
    cpu.C = lo;
    bus.write(cpu.PC, 0xED);
    bus.write(cpu.PC + 1, 0x43);
    bus.write(cpu.PC + 2, 0x29);
    bus.write(cpu.PC + 3, 0xB2);

    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(memory.read(address), cpu.C); // Memory[nn] should contain L (0x3A)
    ASSERT_EQ(memory.read(address + 1), cpu.B);

    uint16_t address3 = 0xB220; // Address nn
    uint8_t hi3 = 0x30;
    uint8_t lo3 = 0x48;
    cpu.D = hi3;
    cpu.E = lo3;
    bus.write(cpu.PC, 0xED);
    bus.write(cpu.PC + 1, 0x53);
    bus.write(cpu.PC + 2, 0x20);
    bus.write(cpu.PC + 3, 0xB2);

    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(memory.read(address3), cpu.E); // Memory[nn] should contain L (0x3A)
    ASSERT_EQ(memory.read(address3 + 1), cpu.D);

    uint16_t address2 = 0xB229; // Address nn
    uint8_t hi2 = 0x3A;
    uint8_t lo2 = 0x48;
    cpu.SP = 0x3A48;
    bus.write(cpu.PC, 0xED);
    bus.write(cpu.PC + 1, 0x73);
    bus.write(cpu.PC + 2, 0x29);
    bus.write(cpu.PC + 3, 0xB2);

    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(memory.read(address2), lo2); // Memory[nn] should contain L (0x3A)
    ASSERT_EQ(memory.read(address2 + 1), hi2);
}

TEST_F(SixteenBitLoadTest, LD_NN_IX)
{

    uint16_t IX_value = 0x5A30; // Example value for IX register
    uint16_t nn = 0x4392;       // Memory address (nn)
    cpu.IX = IX_value;          // Set IX to the desired value

    bus.write(cpu.PC, 0xDD);                 // Prefix for IX instructions
    bus.write(cpu.PC + 1, 0x22);             // Opcode for LD (nn), IX
    bus.write(cpu.PC + 2, nn & 0xFF);        // Low-order byte of nn
    bus.write(cpu.PC + 3, (nn >> 8) & 0xFF); // High-order byte of nn

    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(bus.read(nn), 0x30);     // Low byte of IX
    ASSERT_EQ(bus.read(nn + 1), 0x5A); // High byte of IX
}

TEST_F(SixteenBitLoadTest, LD_NN_IY)
{

    uint16_t IY_value = 0x5A30; // Example value for IX register
    uint16_t nn = 0x4392;       // Memory address (nn)
    cpu.IY = IY_value;          // Set IX to the desired value

    bus.write(cpu.PC, 0xFD);                 // Prefix for IX instructions
    bus.write(cpu.PC + 1, 0x22);             // Opcode for LD (nn), IX
    bus.write(cpu.PC + 2, nn & 0xFF);        // Low-order byte of nn
    bus.write(cpu.PC + 3, (nn >> 8) & 0xFF); // High-order byte of nn

    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(bus.read(nn), 0x30);     // Low byte of IX
    ASSERT_EQ(bus.read(nn + 1), 0x5A); // High byte of IX
}
TEST_F(SixteenBitLoadTest, LD_SP_HL)
{

    cpu.H = 0x44; // Set high byte of HL
    cpu.L = 0x24; // Set low byte of HL

    bus.write(cpu.PC, 0xF9); // Opcode for LD SP, HL
    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(cpu.SP, 0x4424); // SP should now match the value of HL
}
TEST_F(SixteenBitLoadTest, LD_SP_IX)
{

    cpu.IX = 0x2224;         // Set low byte of HL
    bus.write(cpu.PC, 0xDD); // Opcode for LD SP, HL
    bus.write(cpu.PC + 1, 0xF9);
    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(cpu.SP, cpu.IX); // SP should now match the value of IX
}
TEST_F(SixteenBitLoadTest, LD_SP_IY)
{

    cpu.IY = 0x2224;         // Set low byte of HL
    bus.write(cpu.PC, 0xFD); // Opcode for LD SP, HL
    bus.write(cpu.PC + 1, 0xF9);
    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(cpu.SP, 0x2224); // SP should now match the value of IX
}
TEST_F(SixteenBitLoadTest, PUSHQQ)
{
    // Set SP to a known value
    cpu.SP = 0x2000;
    // Initialize HL with a known value
    cpu.A = 0x12; // High byte
    cpu.F = 0x34; // Low byte
    // Write the opcode for PUSH HL to memory
    bus.write(cpu.PC, 0xF5); // Opcode for PUSH HL
    // Execute the instruction
    cpu.execute(bus.read(cpu.PC));
    // Verify that the SP was decremented twice
    ASSERT_EQ(cpu.SP, 0x1FFE);
    // Verify that the stack contains the correct values
    ASSERT_EQ(bus.read(0x1FFF), 0x12); // High byte of HL
    ASSERT_EQ(bus.read(0x1FFE), 0x34); // Low byte of HL
    // Verify that HL remains unchanged
    ASSERT_EQ(cpu.A, 0x12);
    ASSERT_EQ(cpu.F, 0x34);
}
TEST_F(SixteenBitLoadTest, PUSHIX)
{
    // Set SP to a known value
    cpu.SP = 0x2000;
    // Initialize HL with a known value
    cpu.IX = 0x1234;
    // Write the opcode for PUSH HL to memory
    bus.write(cpu.PC, 0xDD);
    bus.write(cpu.PC + 1, 0xE5); // Opcode for PUSH HL
    // Execute the instruction
    cpu.execute(bus.read(cpu.PC));
    // Verify that the SP was decremented twice
    ASSERT_EQ(cpu.SP, 0x1FFE);
    // Verify that the stack contains the correct values
    ASSERT_EQ(bus.read(0x1FFF), 0x12); // High byte of HL
    ASSERT_EQ(bus.read(0x1FFE), 0x34); // Low byte of HL
}
TEST_F(SixteenBitLoadTest, PUSHIY)
{
    // Set SP to a known value
    cpu.SP = 0x2000;
    // Initialize HL with a known value
    cpu.IY = 0x1234;
    // Write the opcode for PUSH HL to memory
    bus.write(cpu.PC, 0xFD);
    bus.write(cpu.PC + 1, 0xE5); // Opcode for PUSH HL
    // Execute the instruction
    cpu.execute(bus.read(cpu.PC));
    // Verify that the SP was decremented twice
    ASSERT_EQ(cpu.SP, 0x1FFE);
    // Verify that the stack contains the correct values
    ASSERT_EQ(bus.read(0x1FFF), 0x12); // High byte of HL
    ASSERT_EQ(bus.read(0x1FFE), 0x34); // Low byte of HL
}
TEST_F(SixteenBitLoadTest, POP_QQ)
{
    // Initialize SP to point to the stack location
    cpu.SP = 0xFFF0;
    // Set memory values at stack location
    bus.write(0xFFF0, 0x34); // Low byte
    bus.write(0xFFF1, 0x12); // High byte
    // Set the opcode for POP BC
    uint8_t opcode = 0xC1; // POP BC
    // Execute POP instruction
    cpu.execute(opcode);
    // Check register values
    ASSERT_EQ(cpu.B, 0x12); // High byte of BC
    ASSERT_EQ(cpu.C, 0x34); // Low byte of BC
    ASSERT_EQ(cpu.SP, 0xFFF2);

    cpu.SP = 0xFFF0;
    bus.write(0xFFF0, 0x56); // Low byte
    bus.write(0xFFF1, 0x78); // High byte

    uint8_t opcode1 = 0xD1; // POP DE
    cpu.execute(opcode1);

    ASSERT_EQ(cpu.D, 0x78); // High byte of DE
    ASSERT_EQ(cpu.E, 0x56); // Low byte of DE
    ASSERT_EQ(cpu.SP, 0xFFF2);

    cpu.SP = 0xFFF0;
    bus.write(0xFFF0, 0x9A); // Low byte
    bus.write(0xFFF1, 0xBC); // High byte

    uint8_t opcode2 = 0xE1; // POP HL
    cpu.execute(opcode2);

    ASSERT_EQ(cpu.H, 0xBC); // High byte of HL
    ASSERT_EQ(cpu.L, 0x9A); // Low byte of HL
    ASSERT_EQ(cpu.SP, 0xFFF2);
}

TEST_F(SixteenBitLoadTest, POP_IX)
{
    // Initialize SP to point to the stack location
    cpu.SP = 0x1000;

    // Set memory values at stack location
    bus.write(0x1000, 0x55); // Low byte of IX
    bus.write(0x1001, 0x33); // High byte of IX

    // Set the opcode for POP IX
    uint8_t opcode = 0xE1;   // POP IX opcode (extended prefix ED, E1 for IX)
    bus.write(cpu.PC, 0xDD); // Low byte of IX
    bus.write(cpu.PC + 1, opcode);

    // Execute POP instruction
    cpu.execute(bus.read(cpu.PC));

    // Check IX register values
    ASSERT_EQ(cpu.IX, 0x3355); // IX should contain the combined value 0x3355

    // Check if SP has incremented correctly
    ASSERT_EQ(cpu.SP, 0x1002);
}
TEST_F(SixteenBitLoadTest, POP_IY)
{
    // Initialize SP to point to the stack location
    cpu.SP = 0x1000;

    // Set memory values at stack location
    bus.write(0x1000, 0x55); // Low byte of IX
    bus.write(0x1001, 0x33); // High byte of IX

    // Set the opcode for POP IX
    uint8_t opcode = 0xE1;   // POP IX opcode (extended prefix ED, E1 for IX)
    bus.write(cpu.PC, 0xFD); // Low byte of IX
    bus.write(cpu.PC + 1, opcode);

    // Execute POP instruction
    cpu.execute(bus.read(cpu.PC));

    // Check IX register values
    ASSERT_EQ(cpu.IY, 0x3355); // IX should contain the combined value 0x3355

    // Check if SP has incremented correctly
    ASSERT_EQ(cpu.SP, 0x1002);
}

class ExchangeBlockTransferandSearchGroupTest : public ::testing::Test
{
protected:
    Memory memory;
    Bus bus;
    z80 cpu;

    ExchangeBlockTransferandSearchGroupTest() : memory(0x10000), bus(memory), cpu()
    {
        cpu.reset(&bus);
    }
};
TEST_F(ExchangeBlockTransferandSearchGroupTest, EX_DE_HL)
{
    // Initialize DE and HL registers with known values
    cpu.D = 0x28;
    cpu.E = 0x22; // DE = 0x2822
    cpu.H = 0x49;
    cpu.L = 0x9A; // HL = 0x499A

    // Execute the EX DE, HL instruction
    uint8_t opcode = 0xEB; // Opcode for EX DE, HL
    cpu.execute(opcode);

    // Verify that DE and HL have been swapped
    ASSERT_EQ(cpu.D, 0x49); // DE now contains the high byte of HL
    ASSERT_EQ(cpu.E, 0x9A); // DE now contains the low byte of HL
    ASSERT_EQ(cpu.H, 0x28); // HL now contains the high byte of DE
    ASSERT_EQ(cpu.L, 0x22); // HL now contains the low byte of DE
}
TEST_F(ExchangeBlockTransferandSearchGroupTest, EX_AF_AF1)
{
    // Initialize AF and AF' registers with known values
    cpu.A = 0x99;
    cpu.F = 0x00; // AF = 0x9900
    cpu.A1 = 0x59;
    cpu.F1 = 0x44; // AF' = 0x5944

    // Execute the EX AF, AF' instruction
    uint8_t opcode = 0x08; // Opcode for EX AF, AF'
    cpu.execute(opcode);

    // Verify that AF and AF' have been swapped
    ASSERT_EQ(cpu.A, 0x59);  // AF now contains the high byte of AF'
    ASSERT_EQ(cpu.F, 0x44);  // AF now contains the low byte of AF'
    ASSERT_EQ(cpu.A1, 0x99); // AF' now contains the high byte of AF
    ASSERT_EQ(cpu.F1, 0x00); // AF' now contains the low byte of AF
}

TEST_F(ExchangeBlockTransferandSearchGroupTest, EXX)
{
    // Initialize primary registers
    cpu.B = 0x44;
    cpu.C = 0x5A; // BC = 0x445A
    cpu.D = 0x3D;
    cpu.E = 0xA2; // DE = 0x3DA2
    cpu.H = 0x88;
    cpu.L = 0x59; // HL = 0x8859

    // Initialize shadow registers
    cpu.B1 = 0x09;
    cpu.C1 = 0x88; // BC' = 0x0988
    cpu.D1 = 0x93;
    cpu.E1 = 0x00; // DE' = 0x9300
    cpu.H1 = 0x00;
    cpu.L1 = 0xE7; // HL' = 0x00E7

    // Execute the EXX instruction
    uint8_t opcode = 0xD9; // Opcode for EXX
    cpu.execute(opcode);

    // Verify primary registers now contain the shadow values
    ASSERT_EQ(cpu.B, 0x09);
    ASSERT_EQ(cpu.C, 0x88); // BC = 0x0988
    ASSERT_EQ(cpu.D, 0x93);
    ASSERT_EQ(cpu.E, 0x00); // DE = 0x9300
    ASSERT_EQ(cpu.H, 0x00);
    ASSERT_EQ(cpu.L, 0xE7); // HL = 0x00E7

    // Verify shadow registers now contain the primary values
    ASSERT_EQ(cpu.B1, 0x44);
    ASSERT_EQ(cpu.C1, 0x5A); // BC' = 0x445A
    ASSERT_EQ(cpu.D1, 0x3D);
    ASSERT_EQ(cpu.E1, 0xA2); // DE' = 0x3DA2
    ASSERT_EQ(cpu.H1, 0x88);
    ASSERT_EQ(cpu.L1, 0x59); // HL' = 0x8859
}
TEST_F(ExchangeBlockTransferandSearchGroupTest, EX_SP_HL)
{
    // Initialize HL register
    cpu.H = 0x70;
    cpu.L = 0x12; // HL = 0x7012

    // Initialize Stack Pointer
    cpu.SP = 0x8856;

    // Set memory at SP and SP+1
    bus.write(cpu.SP, 0x11);     // Memory[SP] = 0x11 (low byte)
    bus.write(cpu.SP + 1, 0x22); // Memory[SP+1] = 0x22 (high byte)

    // Execute the EX (SP), HL instruction
    uint8_t opcode = 0xE3; // Opcode for EX (SP), HL
    cpu.execute(opcode);

    // Verify HL contains the memory values
    ASSERT_EQ(cpu.H, 0x22); // HL high byte is now from Memory[SP+1]
    ASSERT_EQ(cpu.L, 0x11); // HL low byte is now from Memory[SP]

    // Verify memory contains the previous HL values
    ASSERT_EQ(bus.read(cpu.SP), 0x12);     // Memory[SP] contains HL low byte
    ASSERT_EQ(bus.read(cpu.SP + 1), 0x70); // Memory[SP+1] contains HL high byte

    // Verify SP remains unchanged
    ASSERT_EQ(cpu.SP, 0x8856);
}
TEST_F(ExchangeBlockTransferandSearchGroupTest, EX_SP_IX)
{
    // Initialize IX register
    cpu.IX = 0x3988;

    // Initialize Stack Pointer
    cpu.SP = 0x0100;

    // Set memory at SP and SP+1
    bus.write(cpu.SP, 0x90);     // Memory[SP] = 0x90 (low byte)
    bus.write(cpu.SP + 1, 0x48); // Memory[SP+1] = 0x48 (high byte)

    bus.write(cpu.PC, 0xDD); // Memory[SP] = 0x90 (low byte)
    bus.write(cpu.PC + 1, 0xE3);
    cpu.execute(bus.read(cpu.PC));

    // Verify IX contains the memory values
    ASSERT_EQ(cpu.IX, 0x4890);

    // Verify memory contains the previous IX values
    ASSERT_EQ(bus.read(cpu.SP), 0x88);     // Memory[SP] contains IX low byte
    ASSERT_EQ(bus.read(cpu.SP + 1), 0x39); // Memory[SP+1] contains IX high byte

    // Verify SP remains unchanged
    ASSERT_EQ(cpu.SP, 0x0100);
}
TEST_F(ExchangeBlockTransferandSearchGroupTest, EX_SP_IY)
{
    // Initialize IX register
    cpu.IY = 0x3988;

    // Initialize Stack Pointer
    cpu.SP = 0x0100;

    // Set memory at SP and SP+1
    bus.write(cpu.SP, 0x90);     // Memory[SP] = 0x90 (low byte)
    bus.write(cpu.SP + 1, 0x48); // Memory[SP+1] = 0x48 (high byte)

    bus.write(cpu.PC, 0xFD); // Memory[SP] = 0x90 (low byte)
    bus.write(cpu.PC + 1, 0xE3);
    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(cpu.IY, 0x4890);

    // Verify memory contains the previous IX values
    ASSERT_EQ(bus.read(cpu.SP), 0x88);     // Memory[SP] contains IX low byte
    ASSERT_EQ(bus.read(cpu.SP + 1), 0x39); // Memory[SP+1] contains IX high byte

    // Verify SP remains unchanged
    ASSERT_EQ(cpu.SP, 0x0100);
}
TEST_F(ExchangeBlockTransferandSearchGroupTest, LDI)
{

    cpu.H = 0xd0;
    cpu.L = 0x97; // HL = 0x1111
    cpu.D = 0x95;
    cpu.E = 0xc1; // DE = 0x2222
    cpu.B = 0x3d;
    cpu.C = 0x11; // BC = 7
    cpu.F = 0xC9;
    cpu.A = 0x1b;

    // Memory at DE = 0x66

    memory.write(0xd097, 0xb7);

    // Write the LDI instruction at the program counter
    memory.write(cpu.PC, 0xED);     // First byte of LDI opcode
    memory.write(cpu.PC + 1, 0xA0); // Second byte of LDI opcode

    // Execute the instruction
    cpu.execute(memory.read(cpu.PC)); // Execute ED

    // Verify results
    ASSERT_EQ(cpu.getHL(), 0xd098); // HL incremented
    ASSERT_EQ(cpu.getDE(), 0x95c2); // DE incremented
    ASSERT_EQ(cpu.getBC(), 0x3d10); // BC decremented

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));
    ASSERT_TRUE(cpu.isFlagSet(z80::S));                 // H is reset
    ASSERT_TRUE(cpu.isFlagSet(z80::Z));                 // N is reset
    ASSERT_EQ(cpu.isFlagSet(z80::P), cpu.getBC() != 0); // P/V set if BC != 0
}

TEST_F(ExchangeBlockTransferandSearchGroupTest, LDIR)
{
    // Initialize CPU registers
    cpu.setAF(0x1045);
    cpu.setBC(0x0010);
    cpu.setDE(0xaad8);

    cpu.setHL(0x558e);
    uint16_t address = 0x558e;
    cpu.PC = 0x00;

    uint16_t PC = cpu.PC;

    // Memory setup
    memory.write(address, 0x53);      // Memory at HL = 0x88
    memory.write(address + 1, 0x94);  // Memory at HL+1 = 0x36
    memory.write(address + 2, 0x30);  // Memory at HL+2 = 0xA5
    memory.write(address + 3, 0x05);  // Memory at DE = 0x00
    memory.write(address + 4, 0x44);  // Memory at DE+1 = 0x00
    memory.write(address + 5, 0x24);  // Memory at DE+2 = 0x00
    memory.write(address + 6, 0x52);  // Memory at HL = 0x88
    memory.write(address + 7, 0xb9);  // Memory at HL+1 = 0x36
    memory.write(address + 8, 0xe9);  // Memory at HL+2 = 0xA5
    memory.write(address + 9, 0x77);  // Memory at DE = 0x00
    memory.write(address + 10, 0x23); // Memory at DE+1 = 0x00
    memory.write(address + 11, 0x71); // Memory at DE+2 = 0x00

    memory.write(address + 12, 0xe2); // Memory at HL+2 = 0xA5
    memory.write(address + 13, 0x5c); // Memory at DE = 0x00
    memory.write(address + 14, 0xfb); // Memory at DE+1 = 0x00
    memory.write(address + 15, 0x49); // Memory at DE+2 = 0x00

    // Write the LDIR instruction at the program counter
    memory.write(cpu.PC, 0xED);     // First byte of LDIR opcode
    memory.write(cpu.PC + 1, 0xB0); // Second byte of LDIR opcode

    // Execute the instruction

    uint8_t count = 0;

    do
    {
        cpu.execute(memory.read(cpu.PC));

    } while (cpu.PC < 2);

    // Verify results
    ASSERT_EQ(cpu.getAF(), 0x1049); // HL incremented by 3
    ASSERT_EQ(cpu.getBC(), 0x0000); // DE incremented by 3
    ASSERT_EQ(cpu.getDE(), 0xaae8);
    ASSERT_EQ(cpu.getHL(), 0x559e);       // BC decremented to 0
    ASSERT_EQ(memory.read(0xaad8), 0x53); // Data copied from HL to DE
    ASSERT_EQ(cpu.PC, 0x02);              // Data copied from HL to DE

    if (cpu.getBC() != 0)
    {
        ASSERT_EQ(cpu.PC, PC); // Ensure PC loops back to execute instruction again
    }
    else
    {
        ASSERT_EQ(cpu.PC, PC + 2); // Ensure PC advances normally
    }

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag));           // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));                // N is reset
    ASSERT_EQ(cpu.isFlagSet(z80::P), cpu.getBC() != 0); // P/V is reset because BC = 0
}
TEST_F(ExchangeBlockTransferandSearchGroupTest, LDD)
{
    // Initialize CPU registers
    cpu.H = 0x11;
    cpu.L = 0x11; // HL = 0x1111
    cpu.D = 0x22;
    cpu.E = 0x22; // DE = 0x2222
    cpu.B = 0x00;
    cpu.C = 0x07; // BC = 7
    cpu.F = 0xFF; // Set flags to a known state

    uint16_t initialHL = cpu.getHL();
    uint16_t initialDE = cpu.getDE();
    uint16_t initialBC = cpu.getBC();
    uint16_t initialPC = cpu.PC;

    // Memory setup
    memory.write(0x1111, 0x88); // Memory at HL = 0x88
    memory.write(0x2222, 0x66); // Memory at DE = 0x66

    // Write the LDD instruction at the program counter
    memory.write(cpu.PC, 0xED);     // First byte of LDD opcode
    memory.write(cpu.PC + 1, 0xA8); // Second byte of LDD opcode

    // Execute the instruction
    cpu.execute(memory.read(cpu.PC));

    // Verify results
    ASSERT_EQ(cpu.getHL(), initialHL - 1); // HL decremented
    ASSERT_EQ(cpu.getDE(), initialDE - 1); // DE decremented
    ASSERT_EQ(cpu.getBC(), initialBC - 1); // BC decremented
    ASSERT_EQ(memory.read(0x2222), 0x88);  // Data copied from HL to DE
    ASSERT_EQ(memory.read(0x1111), 0x88);  // Memory at HL remains unchanged
    ASSERT_EQ(cpu.PC, initialPC + 2);      // PC advances to the next instruction

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag));           // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));                // N is reset
    ASSERT_EQ(cpu.isFlagSet(z80::P), cpu.getBC() != 0); // P/V set if BC != 0
}
TEST_F(ExchangeBlockTransferandSearchGroupTest, LDDR)
{
    // Initialize CPU registers
    cpu.H = 0x11;
    cpu.L = 0x14; // HL = 0x1114
    cpu.D = 0x22;
    cpu.E = 0x25; // DE = 0x2225
    cpu.B = 0x00;
    cpu.C = 0x03; // BC = 3
    cpu.F = 0xFF; // Initial flags (arbitrary)
    uint16_t PC = cpu.PC;

    uint16_t HL = cpu.getHL();
    uint16_t DE = cpu.getDE();

    // Memory setup
    memory.write(0x1114, 0xA5); // Memory at HL
    memory.write(0x1113, 0x36); // Memory at HL - 1
    memory.write(0x1112, 0x88); // Memory at HL - 2

    memory.write(0x2225, 0xC5); // Memory at DE
    memory.write(0x2224, 0x59); // Memory at DE - 1
    memory.write(0x2223, 0x66); // Memory at DE - 2

    // Write the LDDR instruction at the program counter
    memory.write(cpu.PC, 0xED);     // First byte of LDDR opcode
    memory.write(cpu.PC + 1, 0xB8); // Second byte of LDDR opcode

    // Execute the instruction until BC = 0
    uint8_t count = 0;
    while (cpu.getBC() != 0)
    {
        cpu.execute(memory.read(cpu.PC));
        count++;
    }

    // Verify results
    ASSERT_EQ(cpu.getHL(), HL - count); // HL decremented by the byte count
    ASSERT_EQ(cpu.getDE(), DE - count); // DE decremented by the byte count
    ASSERT_EQ(cpu.getBC(), 0x0000);     // BC decremented to 0

    // Verify memory after transfers
    ASSERT_EQ(memory.read(0x2225), 0xA5); // Data from HL transferred to DE
    ASSERT_EQ(memory.read(0x2224), 0x36); // Data from HL-1 transferred to DE-1
    ASSERT_EQ(memory.read(0x2223), 0x88); // Data from HL-2 transferred to DE-2

    // Program Counter validation
    ASSERT_EQ(cpu.PC, cpu.getBC() == 0 ? PC + 2 : PC); // PC behavior on BC = 0

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::P));      // P/V is reset when BC = 0
}
TEST_F(ExchangeBlockTransferandSearchGroupTest, CPI)
{
    // Initialize CPU registers
    cpu.A = 0x20; // Accumulator
    cpu.H = 0x11;
    cpu.L = 0x11; // HL = 0x1111
    cpu.B = 0x00;
    cpu.C = 0x01; // BC = 1
    cpu.F = 0xFF; // Clear flags

    uint16_t HL = cpu.getHL();

    // Memory setup
    memory.write(0x1111, 0x3B); // Memory at HL contains the same value as A

    // Write the CPI instruction at the program counter
    memory.write(cpu.PC, 0xED);     // First byte of CPI opcode
    memory.write(cpu.PC + 1, 0xA1); // Second byte of CPI opcode

    // Execute the instruction
    cpu.execute(memory.read(cpu.PC));

    // Verify results
    ASSERT_EQ(cpu.getHL(), HL + 1);   // HL incremented by 1
    ASSERT_EQ(cpu.getBC(), 0x0000);   // BC decremented to 0
    ASSERT_EQ(memory.read(HL), 0x3B); // Memory unchanged

    // Verify flags
    ASSERT_TRUE(cpu.isFlagSet(z80::S));
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::P)); // P/V is reset because BC == 0
    ASSERT_TRUE(cpu.isFlagSet(z80::N));  // N is set because this is a subtraction instruction

    // Verify the program counter advanced correctly
    ASSERT_EQ(cpu.PC, 0x0002); // PC advanced by 2 bytes
}
TEST_F(ExchangeBlockTransferandSearchGroupTest, CPIR)
{
    // Initialize CPU registers
    cpu.H = 0x11;
    cpu.L = 0x11; // HL = 0x1111
    cpu.B = 0x00;
    cpu.C = 0x07; // BC = 7
    cpu.A = 0xF3; // Accumulator = 0xF3
    cpu.F = 0x00; // Clear flags

    uint16_t PC = cpu.PC; // Store the initial program counter
    uint16_t HL = cpu.getHL();
    uint16_t BC = cpu.getBC();

    // Memory setup
    memory.write(0x1111, 0x3B); // Memory at HL
    memory.write(0x1112, 0x2A); // Memory at HL + 1
    memory.write(0x1113, 0xF3); // Memory at HL + 2
    memory.write(0x1114, 0x55); // Memory at HL + 3

    // Write the CPIR instruction at the program counter
    memory.write(cpu.PC, 0xED);     // First byte of CPIR opcode
    memory.write(cpu.PC + 1, 0xB1); // Second byte of CPIR opcode
    int count = 0;
    // Execute the CPIR instruction
    while (count < 3)
    {
        cpu.execute(memory.read(cpu.PC));
        count++;
        if (cpu.getBC() == 0)
        {
            break;
        }
    }

    // Verify results after execution
    ASSERT_EQ(cpu.getHL(), HL + 3);
    ASSERT_EQ(cpu.getBC(), BC - 3); // BC should decrement by 3
    ASSERT_EQ(cpu.A, 0xF3);         // A remains unchanged
    ASSERT_EQ(cpu.PC, PC + 2);      // PC advances normally after the instruction ends

    // Verify flags
    ASSERT_TRUE(cpu.isFlagSet(z80::Z));       // Z is set because A matches (HL) at 0x1113
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // S is not set as the result isn't negative
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset (no borrow from bit 4)
    ASSERT_TRUE(cpu.isFlagSet(z80::P));       // P/V is set because BC != 0 after decrement
    ASSERT_TRUE(cpu.isFlagSet(z80::N));       // N is set

    // Additional verification
    ASSERT_EQ(memory.read(0x1111), 0x3B); // Memory remains unchanged
    ASSERT_EQ(memory.read(0x1113), 0xF3); // Memory at match remains unchanged
}
TEST_F(ExchangeBlockTransferandSearchGroupTest, CPD)
{
    // Initialize CPU registers
    cpu.H = 0x11;
    cpu.L = 0x13; // HL = 0x1113
    cpu.B = 0x00;
    cpu.C = 0x03; // BC = 3
    cpu.A = 0x88; // Accumulator = 0x88
    cpu.F = 0x00; // Clear flags

    uint16_t PC = cpu.PC; // Store the initial program counter
    uint16_t HL = cpu.getHL();
    uint16_t BC = cpu.getBC();

    memory.write(0x1113, 0x88); // Memory at HL

    // Write the CPD instruction at the program counter
    memory.write(cpu.PC, 0xED);     // First byte of CPD opcode
    memory.write(cpu.PC + 1, 0xA9); // Second byte of CPD opcode
    int count = 0;

    cpu.execute(memory.read(cpu.PC));

    // Verify results after execution
    ASSERT_EQ(cpu.getHL(), HL - 1); // HL decrements until match
    ASSERT_EQ(cpu.getBC(), BC - 1); // BC decrements twice
    ASSERT_EQ(cpu.A, 0x88);         // A remains unchanged
    ASSERT_EQ(cpu.PC, PC + 2);      // PC advances normally after the instruction ends

    // Verify flags
    ASSERT_TRUE(cpu.isFlagSet(z80::Z));       // Z is set because A matches (HL) at 0x1112
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // S is not set as the result isn't negative
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset (no borrow from bit 4)
    ASSERT_TRUE(cpu.isFlagSet(z80::P));       // P/V is set because BC != 0 after first decrement
    ASSERT_TRUE(cpu.isFlagSet(z80::N));       // N is set

    // Additional verification
    ASSERT_EQ(memory.read(0x1113), 0x88); // Memory remains unchanged
                                          // Memory at match remains unchanged
}
TEST_F(ExchangeBlockTransferandSearchGroupTest, CPDR)
{
    // Initialize CPU registers
    cpu.H = 0x11;
    cpu.L = 0x18; // HL = 0x1118
    cpu.B = 0x00;
    cpu.C = 0x07; // BC = 7
    cpu.A = 0xF3; // Accumulator = 0xF3
    cpu.F = 0x00; // Clear flags

    uint16_t PC = cpu.PC; // Store the initial program counter
    uint16_t HL = cpu.getHL();
    uint16_t BC = cpu.getBC();

    // Memory setup
    memory.write(0x1118, 0x52); // Memory at HL
    memory.write(0x1117, 0x00); // Memory at HL - 1
    memory.write(0x1116, 0xF3); // Memory at HL - 2

    // Write the CPDR instruction at the program counter
    memory.write(cpu.PC, 0xED);     // First byte of CPDR opcode
    memory.write(cpu.PC + 1, 0xB9); // Second byte of CPDR opcode
    int count = 0;

    // Execute the CPDR instruction
    while (count < 3)
    {
        cpu.execute(memory.read(cpu.PC));
        count++;
        if (cpu.getBC() == 0)
        {
            break;
        }
    }

    // Verify results after execution
    ASSERT_EQ(cpu.getHL(), 0x1115); // HL decrements twice to point to the matching address (HL = 0x1116)
    ASSERT_EQ(cpu.getBC(), 0x0004); // BC decrements twice (BC = 5)
    ASSERT_EQ(cpu.A, 0xF3);         // A remains unchanged
    ASSERT_EQ(cpu.PC, PC + 2);      // PC advances normally after the instruction ends

    // Verify flags
    ASSERT_TRUE(cpu.isFlagSet(z80::Z));       // Z is set because A matches (HL) at 0x1116
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // S is not set as the result isn't negative
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset (no borrow from bit 4)
    ASSERT_TRUE(cpu.isFlagSet(z80::P));       // P/V is set because BC != 0 after the decrement
    ASSERT_TRUE(cpu.isFlagSet(z80::N));       // N is set

    // Additional verification
    ASSERT_EQ(memory.read(0x1118), 0x52); // Memory remains unchanged
    ASSERT_EQ(memory.read(0x1116), 0xF3); // Memory at match remains unchanged
}

class EightBitArithmeticGroupTest : public ::testing::Test
{
protected:
    Memory memory;
    Bus bus;
    z80 cpu;

    EightBitArithmeticGroupTest() : memory(0x10000), bus(memory), cpu()
    {
        cpu.reset(&bus);
    }
};

TEST_F(EightBitArithmeticGroupTest, ADD_A_R)
{
    // Initialize CPU registers
    cpu.A = 0x7F; // Accumulator = 0x44
    cpu.B = 0x11; // Register B = 0x11
    cpu.F = 0x00; // Clear flags

    uint8_t expectedResult = 0x7F + 0x11;

    // Write the ADD A, B instruction at the program counter
    memory.write(cpu.PC, 0x80); // OpCode for ADD A, B

    // Execute the ADD A, B instruction
    cpu.execute(memory.read(cpu.PC));

    // Verify results after execution
    ASSERT_EQ(cpu.A, expectedResult); // Accumulator contains the sum
    ASSERT_EQ(cpu.PC, 0x0001);        // PC advances by 1

    // Verify flags
    ASSERT_TRUE(cpu.isFlagSet(z80::S));       // S is set (result is  negative)
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Z is reset (result is not 0)
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag));  // H is SET
    ASSERT_TRUE(cpu.isFlagSet(z80::P));       // P/V is set overflow
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // C is reset (no carry from bit 7)
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset

    // Additional verification
    ASSERT_EQ(cpu.B, 0x11); // Register B remains unchanged
}
TEST_F(EightBitArithmeticGroupTest, ADD_A_n)
{
    // Initialize the CPU state for the test
    cpu.A = 0x23; // Accumulator = 0x23
    cpu.F = 0x00; // Clear all flags

    uint8_t operand = 0x33; // Operand n = 0x33

    memory.write(cpu.PC, 0xC6);
    memory.write(cpu.PC + 1, operand); // Write the operand n (0x33) to memory

    // Execute the ADD A, n instruction
    cpu.execute(memory.read(cpu.PC));

    // Verify the result after the instruction is executed
    ASSERT_EQ(cpu.A, 0x23 + operand); // A should be updated with 0x56 (23h + 33h = 56h)

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // S flag should be reset (no negative result)
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Z flag should be reset (result is not 0)
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H flag should be reset (no carry from bit 3)
    ASSERT_FALSE(cpu.isFlagSet(z80::P));      // P/V flag should be reset (no overflow)
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag should be reset (addition, not subtraction)
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // C flag should be reset (no carry from bit 7)

    // Test for overflow by adding a value that would overflow the accumulator
    cpu.A = 0xF0;   // Set A to 0x7F (127 in decimal)
    operand = 0x10; // Operand n = 0x10 (16 in decimal)

    memory.write(cpu.PC, 0xC6);
    memory.write(cpu.PC + 1, operand); // Update operand in memory
    cpu.execute(memory.read(cpu.PC));  // Execute the instruction

    // Verify the result after the overflow
    ASSERT_EQ(cpu.A, 0); // A should be 0x8F (overflowed value)

    ASSERT_TRUE(cpu.isFlagSet(z80::Z));
    // Check if the overflow flag (P/V) is set
    ASSERT_FALSE(cpu.isFlagSet(z80::P)); // P/V flag should be set because of overflow

    // Check for carry from bit 7 and if the C flag is set
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));
}
TEST_F(EightBitArithmeticGroupTest, ADD_A_HL)
{
    // Setup: Initialize CPU registers
    cpu.A = 0xA0; // Accumulator = 0xA0
    cpu.H = 0x23; // High byte of HL = 0x23
    cpu.L = 0x23; // Low byte of HL = 0x23 (HL = 0x2323)
    cpu.F = 0xFF; // Clear all flags

    // Memory setup
    memory.write(0x2323, 0x08); // Memory at address 0x2323 contains 0x08

    // Write ADD A, (HL) instruction to memory at the current PC
    memory.write(cpu.PC, 0x86); // Opcode for ADD A, (HL)

    // Execute the instruction
    cpu.execute(memory.read(cpu.PC));

    // Verify the results
    ASSERT_EQ(cpu.A, 0xA8);         // A should contain the result 0xA8
    ASSERT_EQ(cpu.getHL(), 0x2323); // HL remains unchanged
    ASSERT_EQ(cpu.PC, 1);           // PC increments by 1

    // Verify flags
    ASSERT_TRUE(cpu.isFlagSet(z80::S));       // S is not set; 0xA8 is positive
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Z is not set; result is not zero
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is not set; no carry from bit 3
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // C is not set; no carry from bit 7
    ASSERT_FALSE(cpu.isFlagSet(z80::P));      // P/V is not set; no overflow
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset for addition

    // Ensure memory at 0x2323 remains unchanged
    ASSERT_EQ(memory.read(0x2323), 0x08);
}

TEST_F(EightBitArithmeticGroupTest, ADD_A_IX_d)
{
    // Initialize CPU registers
    cpu.A = 0x11;    // Accumulator = 0x11
    cpu.IX = 0x1000; // IX register = 0x1000
    cpu.F = 0x00;    // Clear all flags

    // Memory setup
    uint8_t displacement = 0x05;               // Displacement = 0x05
    memory.write(0x1000 + displacement, 0x22); // Memory at address 0x1005 = 0x22

    // Write the ADD A, (IX + d) instruction
    memory.write(cpu.PC, 0xDD);             // First byte of the instruction prefix
    memory.write(cpu.PC + 1, 0x86);         // Opcode for ADD A, (IX + d)
    memory.write(cpu.PC + 2, displacement); // Displacement d = 0x05

    // Execute the instruction
    cpu.execute(memory.read(cpu.PC));

    // Verify results
    ASSERT_EQ(cpu.A, 0x33);    // 0x11 + 0x22 = 0x33
    ASSERT_EQ(cpu.PC, 0x0003); // PC increments by 3 bytes

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // No carry from bit 7
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // Result is not negative
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Result is not zero
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // No half carry
    ASSERT_FALSE(cpu.isFlagSet(z80::P));      // No overflow
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
}
TEST_F(EightBitArithmeticGroupTest, ADD_A_IY_d)
{
    // Initialize CPU registers
    cpu.A = 0x80;    // Accumulator = 0x11
    cpu.IY = 0x1000; // IX register = 0x1000
    cpu.F = 0x00;    // Clear all flags

    // Memory setup
    uint8_t displacement = 0x05;               // Displacement = 0x05
    memory.write(0x1000 + displacement, 0x22); // Memory at address 0x1005 = 0x22

    // Write the ADD A, (IX + d) instruction
    memory.write(cpu.PC, 0xFD);             // First byte of the instruction prefix
    memory.write(cpu.PC + 1, 0x86);         // Opcode for ADD A, (IX + d)
    memory.write(cpu.PC + 2, displacement); // Displacement d = 0x05

    // Execute the instruction
    cpu.execute(memory.read(cpu.PC));

    // Verify results
    ASSERT_EQ(cpu.A, 0xA2);    // 0x11 + 0x22 = 0x33
    ASSERT_EQ(cpu.PC, 0x0003); // PC increments by 3 bytes

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // No carry from bit 7
    ASSERT_TRUE(cpu.isFlagSet(z80::S));       // Result is not negative
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Result is not zero
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // No half carry
    ASSERT_FALSE(cpu.isFlagSet(z80::P));      // No overflow
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
}
TEST_F(EightBitArithmeticGroupTest, ADC_A_s)
{
    // Set up initial CPU state
    cpu.A = 0x20;             // Accumulator = 0x20
    cpu.B = 0x15;             // Register B = 0x15 (source register for test)
    cpu.setFlag(z80::C_flag); // Set the carry flag

    uint8_t opCode = 0x88; // ADC A, B (opcode: 1000 1000, source register = B)

    uint8_t expectedResult = 0x20 + 0x15 + 1; // A + B + Carry

    memory.write(cpu.PC, 0x88); // First byte of the instruction prefix

    cpu.execute(memory.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // No carry from bit 7
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // Result is not negative
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Result is not zero
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // No half carry
    ASSERT_FALSE(cpu.isFlagSet(z80::P));      // No overflow
    ASSERT_FALSE(cpu.isFlagSet(z80::N));

    // Additional verification: Ensure other registers are unaffected
    ASSERT_EQ(cpu.B, 0x15); // Register B remains unchanged
}
TEST_F(EightBitArithmeticGroupTest, ADC_A_N)
{
    // Set up initial CPU state
    cpu.A = 0x20;             // Accumulator = 0x20
    cpu.setFlag(z80::C_flag); // Set the carry flag

    uint8_t immediateValue = 0x15; // Immediate value = 0x15
    // Immediate value placed at the program counter
    uint8_t opCode = 0xCE; // ADC A, n (opcode: 1100 1110)

    uint8_t expectedResult = 0x20 + 0x15 + 1; // A + Immediate + Carry

    memory.write(cpu.PC, opCode);
    memory.write(cpu.PC + 1, immediateValue);

    cpu.execute(memory.read(cpu.PC));
    // Verify accumulator result
    ASSERT_EQ(cpu.A, 0x36);

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // No carry from bit 7
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // Result is not negative
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Result is not zero
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // No half carry
    ASSERT_FALSE(cpu.isFlagSet(z80::P));      // No overflow
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is always reset for ADC
}
TEST_F(EightBitArithmeticGroupTest, ADC_A_HL)
{
    // Set up initial CPU state
    cpu.A = 0x7F; // Accumulator = 0x7F (maximum positive value in signed 8-bit)
    cpu.H = 0x12; // HL = 0x1210 (arbitrary memory address)
    cpu.L = 0x10;
    cpu.setFlag(z80::C_flag); // Set the carry flag

    uint8_t memoryValue = 0x01;             // Memory at HL contains 0x01
    memory.write(cpu.getHL(), memoryValue); // Set memory value at address HL
    uint8_t opCode = 0x8E;                  // ADC A, (HL) (opcode: 1000 1110)

    uint8_t expectedResult = 0x7F + 0x01 + 1; // A + memoryValue + carry = 0x81

    // Execute the instruction
    cpu.execute(opCode);

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // Carry flag is set (carry out from bit 7)
    ASSERT_TRUE(cpu.isFlagSet(z80::S));       // Sign flag is set (result is negative, 0x81)
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Zero flag is reset (result is not zero)
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag));  // Half carry flag is reset (no carry from bit 3)
    ASSERT_TRUE(cpu.isFlagSet(z80::P));       // Overflow flag is set (from positive to negative overflow)
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag is reset (addition operation)
}

TEST_F(EightBitArithmeticGroupTest, ADC_A_IX_D)
{
    // Set up initial CPU state
    cpu.A = 0x7F;             // Accumulator = 0x7F (maximum positive value in signed 8-bit)
    cpu.IX = 0x1200;          // IX register points to base address 0x1200
    cpu.setFlag(z80::C_flag); // Set the carry flag

    int8_t displacement = 0x05;                        // Displacement = 5 (signed 8-bit value)
    uint16_t effectiveAddress = cpu.IX + displacement; // Address = 0x1205
    uint8_t memoryValue = 0x01;                        // Memory at 0x1205 contains 0x01
    memory.write(effectiveAddress, memoryValue);       // Write value to memory

    uint8_t opCode = 0xCE; // ADC A, (IX+d) opcode (1100 1110)

    uint8_t expectedResult = 0x7F + 0x01 + 1; // A + memoryValue + carry = 0x81

    uint8_t operand = 0x10; // Operand n = 0x10 (16 in decimal)

    memory.write(cpu.PC, 0xDD);
    memory.write(cpu.PC + 1, 0x8E);
    memory.write(cpu.PC + 2, displacement);
    cpu.execute(memory.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // Carry flag is set (carry out from bit 7)
    ASSERT_TRUE(cpu.isFlagSet(z80::S));       // Sign flag is set (result is negative, 0x81)
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Zero flag is reset (result is not zero)
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag));  // Half carry flag is reset (no carry from bit 3)
    ASSERT_TRUE(cpu.isFlagSet(z80::P));       // Overflow flag is set (from positive to negative overflow)
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag is reset (addition operation)

    // Additional verification
    ASSERT_EQ(memory.read(effectiveAddress), memoryValue); // Memory at (IX+d) remains unchanged
}
TEST_F(EightBitArithmeticGroupTest, ADC_A_IY_D)
{
    // Set up initial CPU state
    cpu.A = 0x7F;             // Accumulator = 0x7F (maximum positive value in signed 8-bit)
    cpu.IY = 0x1200;          // IX register points to base address 0x1200
    cpu.setFlag(z80::C_flag); // Set the carry flag

    int8_t displacement = 0x05;                        // Displacement = 5 (signed 8-bit value)
    uint16_t effectiveAddress = cpu.IY + displacement; // Address = 0x1205
    uint8_t memoryValue = 0x01;                        // Memory at 0x1205 contains 0x01
    memory.write(effectiveAddress, memoryValue);       // Write value to memory

    uint8_t opCode = 0xCE; // ADC A, (IX+d) opcode (1100 1110)

    uint8_t expectedResult = 0x7F + 0x01 + 1; // A + memoryValue + carry = 0x81

    uint8_t operand = 0x10; // Operand n = 0x10 (16 in decimal)

    memory.write(cpu.PC, 0xFD);
    memory.write(cpu.PC + 1, 0x8E);
    memory.write(cpu.PC + 2, displacement);
    cpu.execute(memory.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // Carry flag is set (carry out from bit 7)
    ASSERT_TRUE(cpu.isFlagSet(z80::S));       // Sign flag is set (result is negative, 0x81)
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Zero flag is reset (result is not zero)
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag));  // Half carry flag is reset (no carry from bit 3)
    ASSERT_TRUE(cpu.isFlagSet(z80::P));       // Overflow flag is set (from positive to negative overflow)
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag is reset (addition operation)

    // Additional verification
    ASSERT_EQ(memory.read(effectiveAddress), memoryValue); // Memory at (IX+d) remains unchanged
}
TEST_F(EightBitArithmeticGroupTest, SUB_A_S)
{
    // Set up initial CPU state
    cpu.A = 0x30; // Accumulator = 0x20
    cpu.B = 0x09; // Register B = 0x15 (source register for test)

    uint8_t opCode = 0x90; // ADC A, B (opcode: 1000 1000, source register = B)

    uint8_t expectedResult = 0x30 - 0x09;

    memory.write(cpu.PC, opCode); // First byte of the instruction prefix

    cpu.execute(memory.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // No carry from bit 7
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // Result is not negative
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Result is not zero
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag));  // No half carry
    ASSERT_FALSE(cpu.isFlagSet(z80::P));      // No overflow
    ASSERT_TRUE(cpu.isFlagSet(z80::N));
}
TEST_F(EightBitArithmeticGroupTest, SUB_A_S_Overflow)
{
    // Set up initial CPU state
    cpu.A = 0x00; // Accumulator = 0x20
    cpu.B = 0x80; // Register B = 0x15 (source register for test)

    uint8_t opCode = 0x90; // ADC A, B (opcode: 1000 1000, source register = B)

    uint8_t expectedResult = 0x00 - 0x80;

    memory.write(cpu.PC, opCode); // First byte of the instruction prefix

    cpu.execute(memory.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));  // No carry from bit 7
    ASSERT_TRUE(cpu.isFlagSet(z80::S));       // Result is not negative
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Result is not zero
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // No half carry
    ASSERT_TRUE(cpu.isFlagSet(z80::P));       // No overflow
    ASSERT_TRUE(cpu.isFlagSet(z80::N));
}
TEST_F(EightBitArithmeticGroupTest, SUB_A_N)
{
    // Set up initial CPU state
    cpu.A = 0x00; // Accumulator = 0x20
                  // Register B = 0x15 (source register for test)

    uint8_t opCode = 0xD6; // ADC A, B (opcode: 1000 1000, source register = B)

    uint8_t expectedResult = 0x00 - 0x80;

    memory.write(cpu.PC, opCode);
    memory.write(cpu.PC + 1, 0x80); // First byte of the instruction prefix

    cpu.execute(memory.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));  // No carry from bit 7
    ASSERT_TRUE(cpu.isFlagSet(z80::S));       // Result is not negative
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Result is not zero
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // No half carry
    ASSERT_TRUE(cpu.isFlagSet(z80::P));       // No overflow
    ASSERT_TRUE(cpu.isFlagSet(z80::N));
}
TEST_F(EightBitArithmeticGroupTest, SUB_A_HL)
{
    // Set up initial CPU state
    cpu.A = 0x00; // Accumulator = 0x20
    cpu.H = 0x11;
    cpu.L = 0x11;

    uint8_t opCode = 0x96; // ADC A, B (opcode: 1000 1000, source register = B)

    uint8_t expectedResult = 0x00 - 0x80;

    memory.write(cpu.PC, opCode);
    memory.write(cpu.getHL(), 0x80); // First byte of the instruction prefix

    cpu.execute(memory.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));  // No carry from bit 7
    ASSERT_TRUE(cpu.isFlagSet(z80::S));       // Result is not negative
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Result is not zero
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // No half carry
    ASSERT_TRUE(cpu.isFlagSet(z80::P));       // No overflow
    ASSERT_TRUE(cpu.isFlagSet(z80::N));
}
TEST_F(EightBitArithmeticGroupTest, SUB_A_IX_D)
{
    // Set up initial CPU state
    cpu.A = 0x00; // Accumulator = 0x20
    cpu.IX = 0x1111;

    uint8_t opCode = 0x96; // ADC A, B (opcode: 1000 1000, source register = B)

    uint8_t expectedResult = 0x00 - 0x80;

    int8_t d = 0x05;

    memory.write(cpu.PC, 0xDD);
    memory.write(cpu.PC + 1, 0x96);
    memory.write(cpu.PC + 2, 0x05);
    memory.write(cpu.IX + d, 0x80); // First byte of the instruction prefix

    cpu.execute(memory.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));  // No carry from bit 7
    ASSERT_TRUE(cpu.isFlagSet(z80::S));       // Result is not negative
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Result is not zero
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // No half carry
    ASSERT_TRUE(cpu.isFlagSet(z80::P));       // No overflow
    ASSERT_TRUE(cpu.isFlagSet(z80::N));
}
TEST_F(EightBitArithmeticGroupTest, SUB_A_IY_D)
{
    // Set up initial CPU state
    cpu.A = 0x00; // Accumulator = 0x20
    cpu.IY = 0x1111;

    uint8_t opCode = 0x96; // ADC A, B (opcode: 1000 1000, source register = B)

    uint8_t expectedResult = 0x00 - 0x80;

    int8_t d = 0x05;

    memory.write(cpu.PC, 0xFD);
    memory.write(cpu.PC + 1, 0x96);
    memory.write(cpu.PC + 2, 0x05);
    memory.write(cpu.IY + d, 0x80); // First byte of the instruction prefix

    cpu.execute(memory.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));  // No carry from bit 7
    ASSERT_TRUE(cpu.isFlagSet(z80::S));       // Result is not negative
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Result is not zero
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // No half carry
    ASSERT_TRUE(cpu.isFlagSet(z80::P));       // No overflow
    ASSERT_TRUE(cpu.isFlagSet(z80::N));
}
TEST_F(EightBitArithmeticGroupTest, SBC_A_S)
{
    // Set up initial CPU state
    cpu.A = 0x20;             // Accumulator = 0x20
    cpu.H = 0x15;             // Register B = 0x15 (source register for test)
    cpu.setFlag(z80::C_flag); // Set the carry flag

    uint8_t opCode = 0x9C; // ADC A, B (opcode: 1000 1000, source register = B)

    uint8_t expectedResult = 0x20 - 0x15 - 1; // A + B + Carry

    memory.write(cpu.PC, opCode); // First byte of the instruction prefix

    cpu.execute(memory.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // No carry from bit 7
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // Result is not negative
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Result is not zero
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::P)); // No overflow
    ASSERT_TRUE(cpu.isFlagSet(z80::N));
}
TEST_F(EightBitArithmeticGroupTest, SBC_A_HL)
{
    // Set up initial CPU state
    cpu.A = 0x20; // Accumulator = 0x20
    cpu.H = 0x11;
    cpu.L = 0x11;
    cpu.setFlag(z80::C_flag); // Set the carry flag

    uint8_t opCode = 0x9E;

    uint8_t expectedResult = 0x20 - 0x15 - 1;

    memory.write(cpu.PC, opCode);
    memory.write(cpu.getHL(), 0x15);

    cpu.execute(memory.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // No carry from bit 7
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // Result is not negative
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Result is not zero
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::P)); // No overflow
    ASSERT_TRUE(cpu.isFlagSet(z80::N));
}
TEST_F(EightBitArithmeticGroupTest, SBC_A_IX_D)
{
    // Set up initial CPU state
    cpu.A = 0x20; // Accumulator = 0x20
    cpu.IX = 0x1234;
    cpu.setFlag(z80::C_flag); // Set the carry flag

    int8_t d = 0x05;

    uint8_t opCode = 0x9E;

    uint8_t expectedResult = 0x20 - 0x15 - 1;

    memory.write(cpu.PC, 0xDD);
    memory.write(cpu.PC + 1, opCode);
    memory.write(cpu.PC + 2, d);
    memory.write(cpu.IX + d, 0x15);

    cpu.execute(memory.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // No carry from bit 7
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // Result is not negative
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Result is not zero
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::P)); // No overflow
    ASSERT_TRUE(cpu.isFlagSet(z80::N));
}
TEST_F(EightBitArithmeticGroupTest, SBC_A_IY_D)
{
    // Set up initial CPU state
    cpu.A = 0x20; // Accumulator = 0x20
    cpu.IY = 0x1234;
    cpu.setFlag(z80::C_flag); // Set the carry flag

    int8_t d = 0x05;

    uint8_t opCode = 0x9E;

    uint8_t expectedResult = 0x20 - 0x15 - 1;

    memory.write(cpu.PC, 0xFD);
    memory.write(cpu.PC + 1, opCode);
    memory.write(cpu.PC + 2, d);
    memory.write(cpu.IY + d, 0x15);

    cpu.execute(memory.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // No carry from bit 7
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // Result is not negative
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Result is not zero
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::P)); // No overflow
    ASSERT_TRUE(cpu.isFlagSet(z80::N));
}
TEST_F(EightBitArithmeticGroupTest, AND_A_R)
{
    // Set up initial CPU state
    cpu.A = 0xAA;          // Accumulator = 0xAA (binary: 10101010)
    cpu.B = 0x55;          // Register B = 0x55 (binary: 01010101)
    uint8_t opCode = 0xA0; // AND A, B (opcode: 10100000, source register = B)

    uint8_t expectedResult = 0xAA & 0x55; // Result = 0x00 (binary: 00000000)

    // Execute the instruction
    cpu.execute(opCode);

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    // Verify flags
    ASSERT_TRUE(cpu.isFlagSet(z80::Z));       // Zero flag is set (result is 0)
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // Sign flag is reset (no negative result)
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag));  // Half carry flag is always set in AND
    ASSERT_TRUE(cpu.isFlagSet(z80::P));       // Parity flag is set (even parity: 0 bits set)
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // Carry flag is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag is reset

    ASSERT_EQ(cpu.B, 0x55); // Register B remains unchanged
}
TEST_F(EightBitArithmeticGroupTest, AND_A_N)
{
    // Set up initial CPU state
    cpu.A = 0xF0;                  // Accumulator = 0xF0 (binary: 11110000)
    uint8_t immediateValue = 0x0F; // Immediate value = 0x0F (binary: 00001111)
    uint8_t opCode = 0xE6;         // AND A, n (opcode: 11100110)

    uint8_t expectedResult = 0xF0 & 0x0F; // Expected result = 0x00 (binary: 00000000)

    // Write the immediate value to memory at the PC
    memory.write(cpu.PC, opCode);
    memory.write(cpu.PC + 1, immediateValue);

    // Execute the instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    // Verify flags
    ASSERT_TRUE(cpu.isFlagSet(z80::Z));       // Zero flag is set (result is 0)
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // Sign flag is reset (result is not negative)
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag));  // Half carry flag is always set in AND
    ASSERT_TRUE(cpu.isFlagSet(z80::P));       // Parity flag is set (even parity: 0 bits set)
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // Carry flag is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag is reset
}
TEST_F(EightBitArithmeticGroupTest, AND_A_HL)
{
    // Set up initial CPU state
    cpu.A = 0x03; // Accumulator = 0xF3 (binary: 11110011)
    cpu.H = 0x12;
    cpu.L = 0x34;                      // HL register pair points to address 0x1234
    uint8_t memoryValue = 0x01;        // Memory at address 0x1234 = 0x0C (binary: 00001100)
    memory.write(0x1234, memoryValue); // Write value to memory at HL address

    uint8_t opCode = 0xA6;                // AND A, (HL) (opcode: 10100110)
    uint8_t expectedResult = 0x03 & 0x01; // Expected result = 0x00 (binary: 00000000)

    // Write the opcode to memory at the PC
    memory.write(cpu.PC, opCode);

    // Execute the instruction
    cpu.execute(memory.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));     // Zero flag is set (result is 0)
    ASSERT_FALSE(cpu.isFlagSet(z80::S));     // Sign flag is reset (result is not negative)
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag)); // Half carry flag is always set in AND
    ASSERT_FALSE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // Carry flag is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag is reset

    // Additional verification
    ASSERT_EQ(memory.read(cpu.getHL()), memoryValue); // Memory at HL remains unchanged
    ASSERT_EQ(cpu.getHL(), 0x1234);                   // HL register pair remains unchanged
}
TEST_F(EightBitArithmeticGroupTest, AND_A_IX_D)
{
    // Set up initial CPU state
    cpu.A = 0x03;                             // Accumulator = 0xF3 (binary: 11110011)
    cpu.IX = 0x1234;                          // HL register pair points to address 0x1234
    uint8_t memoryValue = 0x01;               // Memory at address 0x1234 = 0x0C (binary: 00001100)
    memory.write(0x1234 + 0x05, memoryValue); // Write value to memory at HL address

    uint8_t opCode = 0xA6;                // AND A, (HL) (opcode: 10100110)
    uint8_t expectedResult = 0x03 & 0x01; // Expected result = 0x00 (binary: 00000000)

    // Write the opcode to memory at the PC
    memory.write(cpu.PC, 0xDD);
    memory.write(cpu.PC + 1, opCode);
    memory.write(cpu.PC + 2, 0x05);

    // Execute the instruction
    cpu.execute(memory.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));     // Zero flag is set (result is 0)
    ASSERT_FALSE(cpu.isFlagSet(z80::S));     // Sign flag is reset (result is not negative)
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag)); // Half carry flag is always set in AND
    ASSERT_FALSE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // Carry flag is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag is reset
}
TEST_F(EightBitArithmeticGroupTest, AND_A_IY_D)
{
    // Set up initial CPU state
    cpu.A = 0x03;                             // Accumulator = 0xF3 (binary: 11110011)
    cpu.IY = 0x1234;                          // HL register pair points to address 0x1234
    uint8_t memoryValue = 0x01;               // Memory at address 0x1234 = 0x0C (binary: 00001100)
    memory.write(0x1234 + 0x05, memoryValue); // Write value to memory at HL address

    uint8_t opCode = 0xA6;                // AND A, (HL) (opcode: 10100110)
    uint8_t expectedResult = 0x03 & 0x01; // Expected result = 0x00 (binary: 00000000)

    // Write the opcode to memory at the PC
    memory.write(cpu.PC, 0xFD);
    memory.write(cpu.PC + 1, opCode);
    memory.write(cpu.PC + 2, 0x05);

    // Execute the instruction
    cpu.execute(memory.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));     // Zero flag is set (result is 0)
    ASSERT_FALSE(cpu.isFlagSet(z80::S));     // Sign flag is reset (result is not negative)
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag)); // Half carry flag is always set in AND
    ASSERT_FALSE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // Carry flag is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag is reset
}

TEST_F(EightBitArithmeticGroupTest, OR_A_R)
{
    // Set up initial CPU state
    cpu.A = 0xAA;          // Accumulator = 0xAA (binary: 10101010)
    cpu.E = 0x55;          // Register B = 0x55 (binary: 01010101)
    uint8_t opCode = 0xB3; // AND A, B (opcode: 10100000, source register = B)

    uint8_t expectedResult = 0xAA | 0x55; // Result = 0x00 (binary: 00000000)

    // Execute the instruction
    cpu.execute(opCode);

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Zero flag is set (result is 0)
    ASSERT_TRUE(cpu.isFlagSet(z80::S));       // Sign flag is reset (no negative result)
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // Half carry flag is always set in AND
    ASSERT_TRUE(cpu.isFlagSet(z80::P));       // Parity flag is set (even parity: 0 bits set)
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // Carry flag is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag is reset
}
TEST_F(EightBitArithmeticGroupTest, OR_A_N)
{
    // Set up initial CPU state
    cpu.A = 0xF0;                  // Accumulator = 0xF0 (binary: 11110000)
    uint8_t immediateValue = 0x0F; // Immediate value = 0x0F (binary: 00001111)
    uint8_t opCode = 0xF6;         // AND A, n (opcode: 11100110)

    uint8_t expectedResult = 0xF0 | 0x0F; // Expected result = 0x00 (binary: 00000000)

    // Write the immediate value to memory at the PC
    memory.write(cpu.PC, opCode);
    memory.write(cpu.PC + 1, immediateValue);

    // Execute the instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::Z)); // Zero flag is set (result is 0)
    ASSERT_TRUE(cpu.isFlagSet(z80::S));  // Sign flag is reset (result is not negative)
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag));
    ASSERT_TRUE(cpu.isFlagSet(z80::P));       // Parity flag is set (even parity: 0 bits set)
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // Carry flag is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag is reset
}
TEST_F(EightBitArithmeticGroupTest, OR_A_HL)
{
    // Set up initial CPU state
    cpu.A = 0x03; // Accumulator = 0xF3 (binary: 11110011)
    cpu.H = 0x12;
    cpu.L = 0x34;                      // HL register pair points to address 0x1234
    uint8_t memoryValue = 0x01;        // Memory at address 0x1234 = 0x0C (binary: 00001100)
    memory.write(0x1234, memoryValue); // Write value to memory at HL address

    uint8_t opCode = 0xB6;                // AND A, (HL) (opcode: 10100110)
    uint8_t expectedResult = 0x03 | 0x01; // Expected result = 0x00 (binary: 00000000)

    // Write the opcode to memory at the PC
    memory.write(cpu.PC, opCode);

    // Execute the instruction
    cpu.execute(memory.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::Z)); // Zero flag is set (result is 0)
    ASSERT_FALSE(cpu.isFlagSet(z80::S)); // Sign flag is reset (result is not negative)
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag));
    ASSERT_TRUE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // Carry flag is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag is reset
}
TEST_F(EightBitArithmeticGroupTest, OR_A_IX_D)
{
    // Set up initial CPU state
    cpu.A = 0x03;                             // Accumulator = 0xF3 (binary: 11110011)
    cpu.IX = 0x1234;                          // HL register pair points to address 0x1234
    uint8_t memoryValue = 0x01;               // Memory at address 0x1234 = 0x0C (binary: 00001100)
    memory.write(0x1234 + 0x05, memoryValue); // Write value to memory at HL address

    uint8_t opCode = 0xB6;                // AND A, (HL) (opcode: 10100110)
    uint8_t expectedResult = 0x03 | 0x01; // Expected result = 0x00 (binary: 00000000)

    // Write the opcode to memory at the PC
    memory.write(cpu.PC, 0xDD);
    memory.write(cpu.PC + 1, opCode);
    memory.write(cpu.PC + 2, 0x05);

    // Execute the instruction
    cpu.execute(memory.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Zero flag is set (result is 0)
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // Sign flag is reset (result is not negative)
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // Half carry flag is always set in AND
    ASSERT_TRUE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // Carry flag is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag is reset
}
TEST_F(EightBitArithmeticGroupTest, OR_A_IY_D)
{
    // Set up initial CPU state
    cpu.A = 0x03;                             // Accumulator = 0xF3 (binary: 11110011)
    cpu.IY = 0x1234;                          // HL register pair points to address 0x1234
    uint8_t memoryValue = 0x01;               // Memory at address 0x1234 = 0x0C (binary: 00001100)
    memory.write(0x1234 + 0x05, memoryValue); // Write value to memory at HL address

    uint8_t opCode = 0xB6;                // AND A, (HL) (opcode: 10100110)
    uint8_t expectedResult = 0x03 | 0x01; // Expected result = 0x00 (binary: 00000000)

    // Write the opcode to memory at the PC
    memory.write(cpu.PC, 0xFD);
    memory.write(cpu.PC + 1, opCode);
    memory.write(cpu.PC + 2, 0x05);

    // Execute the instruction
    cpu.execute(memory.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Zero flag is set (result is 0)
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // Sign flag is reset (result is not negative)
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // Half carry flag is always set in AND
    ASSERT_TRUE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // Carry flag is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag is reset
}
TEST_F(EightBitArithmeticGroupTest, XOR_A_R)
{
    // Set up initial CPU state
    cpu.A = 0xAA;          // Accumulator = 0xAA (binary: 10101010)
    cpu.L = 0x55;          // Register B = 0x55 (binary: 01010101)
    uint8_t opCode = 0xAD; // AND A, B (opcode: 10100000, source register = B)

    uint8_t expectedResult = 0xAA ^ 0x55;

    // Execute the instruction
    cpu.execute(opCode);

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Zero flag is set (result is 0)
    ASSERT_TRUE(cpu.isFlagSet(z80::S));       // Sign flag is reset (no negative result)
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // Half carry flag is always set in AND
    ASSERT_TRUE(cpu.isFlagSet(z80::P));       // Parity flag is set (even parity: 0 bits set)
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // Carry flag is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag is reset
}
TEST_F(EightBitArithmeticGroupTest, XOR_A_N)
{
    // Set up initial CPU state
    cpu.A = 0xF0;                  // Accumulator = 0xF0 (binary: 11110000)
    uint8_t immediateValue = 0x06; // Immediate value = 0x0F (binary: 00001111)
    uint8_t opCode = 0xEE;         // AND A, n (opcode: 11100110)

    uint8_t expectedResult = 0xF0 ^ 0x06; // Expected result = 0x00 (binary: 00000000)

    // Write the immediate value to memory at the PC
    memory.write(cpu.PC, opCode);
    memory.write(cpu.PC + 1, immediateValue);

    // Execute the instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));
    ASSERT_TRUE(cpu.isFlagSet(z80::S)); // Sign flag is reset (result is not negative)
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag));
    ASSERT_TRUE(cpu.isFlagSet(z80::P));       // Parity flag is set (even parity: 0 bits set)
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // Carry flag is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag is reset
}
TEST_F(EightBitArithmeticGroupTest, XOR_A_HL)
{
    // Set up initial CPU state
    cpu.A = 0x03; // Accumulator = 0xF3 (binary: 11110011)
    cpu.H = 0x12;
    cpu.L = 0x34;                      // HL register pair points to address 0x1234
    uint8_t memoryValue = 0x01;        // Memory at address 0x1234 = 0x0C (binary: 00001100)
    memory.write(0x1234, memoryValue); // Write value to memory at HL address

    uint8_t opCode = 0xAE;                // AND A, (HL) (opcode: 10100110)
    uint8_t expectedResult = 0x03 ^ 0x01; // Expected result = 0x00 (binary: 00000000)

    // Write the opcode to memory at the PC
    memory.write(cpu.PC, opCode);

    // Execute the instruction
    cpu.execute(memory.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::Z)); // Zero flag is set (result is 0)
    ASSERT_FALSE(cpu.isFlagSet(z80::S)); // Sign flag is reset (result is not negative)
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // Carry flag is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag is reset
}
TEST_F(EightBitArithmeticGroupTest, XOR_A_IX_D)
{
    // Set up initial CPU state
    cpu.A = 0x03;                             // Accumulator = 0xF3 (binary: 11110011)
    cpu.IX = 0x1234;                          // HL register pair points to address 0x1234
    uint8_t memoryValue = 0x01;               // Memory at address 0x1234 = 0x0C (binary: 00001100)
    memory.write(0x1234 + 0x05, memoryValue); // Write value to memory at HL address

    uint8_t opCode = 0xAE;                // AND A, (HL) (opcode: 10100110)
    uint8_t expectedResult = 0x03 ^ 0x01; // Expected result = 0x00 (binary: 00000000)

    // Write the opcode to memory at the PC
    memory.write(cpu.PC, 0xDD);
    memory.write(cpu.PC + 1, opCode);
    memory.write(cpu.PC + 2, 0x05);

    // Execute the instruction
    cpu.execute(memory.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Zero flag is set (result is 0)
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // Sign flag is reset (result is not negative)
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // Half carry flag is always set in AND
    ASSERT_FALSE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // Carry flag is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag is reset
}
TEST_F(EightBitArithmeticGroupTest, XOR_A_IY_D)
{
    // Set up initial CPU state
    cpu.A = 0x03;                             // Accumulator = 0xF3 (binary: 11110011)
    cpu.IY = 0x1234;                          // HL register pair points to address 0x1234
    uint8_t memoryValue = 0x01;               // Memory at address 0x1234 = 0x0C (binary: 00001100)
    memory.write(0x1234 + 0x05, memoryValue); // Write value to memory at HL address

    uint8_t opCode = 0xAE;                // AND A, (HL) (opcode: 10100110)
    uint8_t expectedResult = 0x03 ^ 0x01; // Expected result = 0x00 (binary: 00000000)

    // Write the opcode to memory at the PC
    memory.write(cpu.PC, 0xFD);
    memory.write(cpu.PC + 1, opCode);
    memory.write(cpu.PC + 2, 0x05);

    // Execute the instruction
    cpu.execute(memory.read(cpu.PC));

    // Verify accumulator result
    ASSERT_EQ(cpu.A, expectedResult);

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));
    ASSERT_FALSE(cpu.isFlagSet(z80::S));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::N));
}
TEST_F(EightBitArithmeticGroupTest, CP_R)
{
    // Set up initial CPU state
    cpu.A = 0xF5; // Accumulator = 0x8F (-113 in signed 8-bit)
    cpu.H = 0x0F; // Source register B = 0x80 (-128 in signed 8-bit)

    uint8_t opCode = 0xBC; // CP A, B (opcode: 1011 1000)

    // Execute the instruction
    cpu.execute(opCode);

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Result is not zero
    ASSERT_TRUE(cpu.isFlagSet(z80::S));       // Result is positive (0x0F = 15)
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag));  // No half-borrow (no borrow from bit 4)
    ASSERT_FALSE(cpu.isFlagSet(z80::P));      // Overflow occurred
    ASSERT_TRUE(cpu.isFlagSet(z80::N));       // Subtraction operation
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // No borrow (A >= B)

    cpu.A = 0x01; // Accumulator = 0x8F (-113 in signed 8-bit)
    cpu.H = 0x01; // Source register B = 0x80 (-128 in signed 8-bit)

    uint8_t opCode1 = 0xBC; // CP A, B (opcode: 1011 1000)

    // Execute the instruction
    cpu.execute(opCode1);

    // Verify flags
    ASSERT_TRUE(cpu.isFlagSet(z80::Z));       // Result is not zero
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // Result is positive (0x0F = 15)
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // No half-borrow (no borrow from bit 4)
    ASSERT_FALSE(cpu.isFlagSet(z80::P));      // Overflow occurred
    ASSERT_TRUE(cpu.isFlagSet(z80::N));       // Subtraction operation
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // No borrow (A >= B)
}
TEST_F(EightBitArithmeticGroupTest, CP_N)
{
    // Set up initial CPU state
    cpu.A = 0x81;                  // Accumulator = 0x80 (signed: -128, unsigned: 128)
    uint8_t immediateValue = 0x7F; // Immediate value to compare = 0x01
    uint8_t opCode = 0xFE;         // CP A, n (opcode: 11111110)

    // Write the immediate value to memory at the program counter
    memory.write(cpu.PC, opCode);
    memory.write(cpu.PC + 1, immediateValue);

    // Execute the instruction
    cpu.execute(memory.read(cpu.PC));

    // Verify that the accumulator remains unchanged
    ASSERT_EQ(cpu.A, 0x81);

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::Z)); // Zero flag reset (0x80 != 0x01)
    ASSERT_FALSE(cpu.isFlagSet(z80::S));
    ASSERT_TRUE(cpu.isFlagSet(z80::P));
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag));  // Half borrow set (borrow from bit 4)
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // Carry flag set (borrow required)
    ASSERT_TRUE(cpu.isFlagSet(z80::N));       // N flag set (subtraction operation)
}
TEST_F(EightBitArithmeticGroupTest, CP_HL)
{
    // Set up initial CPU state
    cpu.A = 0x06; // Accumulator = 0x80 (signed: -128, unsigned: 128)
    cpu.H = 0x12;
    cpu.L = 0x34;                      // HL register points to memory location 0x1234
    uint8_t memoryValue = 0x0C;        // Value at memory[HL] = 0x7F (signed: 127, unsigned: 127)
    memory.write(0x1234, memoryValue); // Write value to memory

    uint8_t opCode = 0xBE; // CP A, (HL) opcode (10111110)

    // Expected results
    uint8_t expectedResult = cpu.A - memoryValue; // Expected result of subtraction (does not update A)

    // Execute the instruction
    cpu.execute(opCode);

    // Verify accumulator remains unchanged
    ASSERT_EQ(cpu.A, 0x06);

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));     // Zero flag reset (0x80 != 0x7F)
    ASSERT_TRUE(cpu.isFlagSet(z80::S));      // Sign flag set (result is negative)
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag)); // Half borrow reset (no borrow from bit 4)
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag)); // Carry flag set (borrow occurs: 0x80 < 0x7F unsigned)
    ASSERT_FALSE(cpu.isFlagSet(z80::P));     // Overflow flag reset (no signed overflow)
    ASSERT_TRUE(cpu.isFlagSet(z80::N));      // N flag set (subtraction operation)

    // Additional verification
    ASSERT_EQ(memory.read(0x1234), memoryValue); // Memory value at HL remains unchanged
}
TEST_F(EightBitArithmeticGroupTest, INC_R)
{

    cpu.A = 0x06;
    uint8_t opCode = 0x3C;
    memory.write(cpu.PC, opCode);
    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(cpu.A, 0x06 + 1);

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Zero flag reset (0x80 != 0x7F)
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // Sign flag set (result is negative)
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // Half borrow reset (no borrow from bit 4)
    ASSERT_FALSE(cpu.isFlagSet(z80::P));      // Overflow flag reset (no signed overflow)
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag set (subtraction operation)
}
TEST_F(EightBitArithmeticGroupTest, INC_HL)
{

    cpu.H = 0x12;
    cpu.L = 0x34;
    uint8_t opCode = 0x34;
    memory.write(cpu.PC, opCode);
    memory.write(cpu.getHL(), 0x06);
    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(bus.read(cpu.getHL()), 0x06 + 1);

    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Zero flag reset (0x80 != 0x7F)
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // Sign flag set (result is negative)
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // Half borrow reset (no borrow from bit 4)
    ASSERT_FALSE(cpu.isFlagSet(z80::P));      // Overflow flag reset (no signed overflow)
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag set (subtraction operation)
}
TEST_F(EightBitArithmeticGroupTest, INC_IX_D)
{

    cpu.IX = 0x1234;
    int8_t d = 0x05;
    uint8_t opCode = 0x34;
    memory.write(cpu.PC, 0xDD);
    memory.write(cpu.PC + 1, opCode);
    bus.write(cpu.PC + 2, d);
    memory.write(cpu.IX + d, 0x06);
    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(bus.read(cpu.IX + d), 0x06 + 1);

    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Zero flag reset (0x80 != 0x7F)
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // Sign flag set (result is negative)
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // Half borrow reset (no borrow from bit 4)
    ASSERT_FALSE(cpu.isFlagSet(z80::P));      // Overflow flag reset (no signed overflow)
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag set (subtraction operation)
}
TEST_F(EightBitArithmeticGroupTest, INC_IY_D)
{

    cpu.IY = 0x1234;
    int8_t d = 0x05;
    uint8_t opCode = 0x34;
    memory.write(cpu.PC, 0xFD);
    memory.write(cpu.PC + 1, opCode);
    bus.write(cpu.PC + 2, d);
    memory.write(cpu.IY + d, 0x06);
    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(bus.read(cpu.IY + d), 0x06 + 1);

    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Zero flag reset (0x80 != 0x7F)
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // Sign flag set (result is negative)
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // Half borrow reset (no borrow from bit 4)
    ASSERT_FALSE(cpu.isFlagSet(z80::P));      // Overflow flag reset (no signed overflow)
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag set (subtraction operation)
}
TEST_F(EightBitArithmeticGroupTest, DEC_R)
{

    cpu.B = 0x80;
    uint8_t opCode = 0x05;
    memory.write(cpu.PC, opCode);
    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(cpu.B, 0x80 - 1);

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));     // Zero flag reset (0x80 != 0x7F)
    ASSERT_FALSE(cpu.isFlagSet(z80::S));     // Sign flag set (result is negative)
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag)); // Half borrow reset (no borrow from bit 4)
    ASSERT_TRUE(cpu.isFlagSet(z80::P));      // Overflow flag reset (no signed overflow)
    ASSERT_TRUE(cpu.isFlagSet(z80::N));      // N flag set (subtraction operation)
}
TEST_F(EightBitArithmeticGroupTest, DEC_HL)
{

    cpu.H = 0x12;
    cpu.L = 0x34;
    uint8_t opCode = 0x35;
    memory.write(cpu.PC, opCode);
    memory.write(cpu.getHL(), 0x0A);
    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(bus.read(cpu.getHL()), 0x09);

    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Zero flag reset (0x80 != 0x7F)
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // Sign flag set (result is negative)
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // Half borrow reset (no borrow from bit 4)
    ASSERT_FALSE(cpu.isFlagSet(z80::P));      // Overflow flag reset (no signed overflow)
    ASSERT_TRUE(cpu.isFlagSet(z80::N));       // N flag set (subtraction operation)
}
TEST_F(EightBitArithmeticGroupTest, DEC_IX_D)
{

    cpu.IX = 0x1234;
    int8_t d = 0x05;
    uint8_t opCode = 0x35;
    memory.write(cpu.PC, 0xDD);
    memory.write(cpu.PC + 1, opCode);
    bus.write(cpu.PC + 2, d);
    memory.write(cpu.IX + d, 0x06);
    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(bus.read(cpu.IX + d), 0x06 - 1);

    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Zero flag reset (0x80 != 0x7F)
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // Sign flag set (result is negative)
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // Half borrow reset (no borrow from bit 4)
    ASSERT_FALSE(cpu.isFlagSet(z80::P));      // Overflow flag reset (no signed overflow)
    ASSERT_TRUE(cpu.isFlagSet(z80::N));       // N flag set (subtraction operation)
}
TEST_F(EightBitArithmeticGroupTest, DEC_IY_D)
{

    cpu.IY = 0x1234;
    int8_t d = 0x05;
    uint8_t opCode = 0x35;
    memory.write(cpu.PC, 0xFD);
    memory.write(cpu.PC + 1, opCode);
    bus.write(cpu.PC + 2, d);
    memory.write(cpu.IY + d, 0x06);
    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(bus.read(cpu.IY + d), 0x06 - 1);

    ASSERT_FALSE(cpu.isFlagSet(z80::Z));      // Zero flag reset (0x80 != 0x7F)
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // Sign flag set (result is negative)
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // Half borrow reset (no borrow from bit 4)
    ASSERT_FALSE(cpu.isFlagSet(z80::P));      // Overflow flag reset (no signed overflow)
    ASSERT_TRUE(cpu.isFlagSet(z80::N));       // N flag set (subtraction operation)
}
