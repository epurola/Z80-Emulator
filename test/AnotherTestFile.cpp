#include <gtest/gtest.h>
#include "../main.cpp"
#include "../Bus.cpp"
#include "../Memory.cpp"
#include "../Instruction.cpp"

class GeneralPurposeGroupTest : public ::testing::Test
{
protected:
    Memory memory;
    Bus bus;
    z80 cpu;

    GeneralPurposeGroupTest() : memory(0x10000), bus(memory), cpu()
    {
        cpu.reset(&bus);
    }
};

TEST_F(GeneralPurposeGroupTest, DAA)
{

    // Example scenario: Adding 15 (BCD) and 27 (BCD) using binary arithmetic
    cpu.A = 0x15; // Set Accumulator to 15 in BCD (0001 0101)
    cpu.B = 0x27; // Set B register to 27 in BCD (0010 0111)

    // Write and execute the DAA instruction
    bus.write(cpu.PC, 0x80);
    bus.write(cpu.PC + 1, 0x27); // Opcode for DAA
    cpu.execute(bus.read(cpu.PC));
    ASSERT_EQ(cpu.A, 0x3C);
    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(cpu.A, 0x42); // After DAA, A should hold the correct BCD result (42)

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::S));
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));
    ASSERT_FALSE(cpu.isFlagSet(z80::N));
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag));
    ASSERT_TRUE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag));
}
TEST_F(GeneralPurposeGroupTest, DAASubtractionNegativeResult)
{
    // Example scenario: Subtracting 27 (BCD) from 15 (BCD) using binary arithmetic
    cpu.A = 0x15; // Set Accumulator to 15 in BCD (0001 0101)
    cpu.B = 0x27; // Set B register to 27 in BCD (0010 0111)
    // SUB A, B (A = A - B)
    bus.write(cpu.PC, 0x90); // Opcode for SUB B
    cpu.execute(bus.read(cpu.PC));
    ASSERT_EQ(cpu.A, 0xEE); // After subtraction, binary result is EE (-18 in signed form)
    // Write and execute the DAA instruction
    bus.write(cpu.PC + 1, 0x27); // Opcode for DAA
    cpu.execute(bus.read(cpu.PC + 1));
    ASSERT_EQ(cpu.A, 0x88); // After DAA, A should hold the BCD result for -12 (88 in BCD, tens complement)
    // Verify flags
    ASSERT_TRUE(cpu.isFlagSet(z80::S));      // Sign flag should be set for negative result
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));     // Zero flag should not be set
    ASSERT_TRUE(cpu.isFlagSet(z80::N));      // Subtraction flag should be set
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag)); // Carry flag should be set for borrow

    // Reverse the tens complement form and verify BCD correctness
    ASSERT_EQ(9 - (cpu.A >> 4), 1);       // Tens digit is 1
    ASSERT_EQ(9 - (cpu.A & 0x0F) + 1, 2); // Ones digit is 2

    // Verify additional flags
    ASSERT_TRUE(cpu.isFlagSet(z80::P));       // Even parity flag should be set
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // Half carry flag should not be set
}
TEST_F(GeneralPurposeGroupTest, CPL)
{
    // the contents of the Accumulator
    cpu.A = 0xB4; // Set Accumulator to 0xB4 (1011 0100 in binary)
    // Write and execute the CPL instruction
    bus.write(cpu.PC, 0x2F); // Opcode for CPL
    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(cpu.A, 0x4B); // After CPL, A should contain the inverted value (0100 1011)

    // Verify flags
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag)); // Half carry flag should be set

    ASSERT_TRUE(cpu.isFlagSet(z80::N)); // Subtract flag should be set
}
TEST_F(GeneralPurposeGroupTest, NEG)
{
    // Set initial conditions
    cpu.A = 0x14; // Accumulator starts at 0x14 (binary: 0001 0100)

    // Write the NEG opcode to memory (ED 44 is the extended opcode for NEG)
    bus.write(cpu.PC, 0xED);     // Prefix for extended opcodes
    bus.write(cpu.PC + 1, 0x44); // Opcode for NEG

    // Execute the NEG instruction
    cpu.execute(bus.read(cpu.PC));
    cpu.execute(bus.read(cpu.PC + 1));

    // Verify the result in the accumulator
    ASSERT_EQ(cpu.A, 0xEC); // -0x14 = 0xEC in two's complement

    // Verify flags
    ASSERT_TRUE(cpu.isFlagSet(z80::S));      // Result is negative
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));     // Result is not zero
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag)); // Borrow from bit 4 occurred
    ASSERT_FALSE(cpu.isFlagSet(z80::P));     // P/V is not set unless result was 0x80
    ASSERT_TRUE(cpu.isFlagSet(z80::N));      // Subtraction flag is always set
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag)); // Carry is set since A was not 0 before NEG

    // Special case: A = 0x80 before NEG
    cpu.A = 0x80;
    bus.write(cpu.PC, 0xED); // Prefix for extended opcodes
    bus.write(cpu.PC + 1, 0x44);
    cpu.execute(bus.read(cpu.PC));
    cpu.execute(bus.read(cpu.PC + 1));
    ASSERT_EQ(cpu.A, 0x80);                  // Two's complement of 0x80 is itself
    ASSERT_TRUE(cpu.isFlagSet(z80::P));      // P/V flag set because A was 0x80
    ASSERT_TRUE(cpu.isFlagSet(z80::S));      // Result is negative
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));     // Result is not zero
    ASSERT_TRUE(cpu.isFlagSet(z80::N));      // Subtraction flag is set
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag)); // Carry is set because A was not 0 before NEG

    // Special case: A = 0x00 before NEG
    cpu.A = 0x00;
    bus.write(cpu.PC, 0xED); // Prefix for extended opcodes
    bus.write(cpu.PC + 1, 0x44);
    cpu.execute(bus.read(cpu.PC));
    cpu.execute(bus.read(cpu.PC + 1));
    ASSERT_EQ(cpu.A, 0x00);                   // 0 - 0 = 0
    ASSERT_FALSE(cpu.isFlagSet(z80::S));      // Result is not negative
    ASSERT_TRUE(cpu.isFlagSet(z80::Z));       // Result is zero
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // No borrow from bit 4
    ASSERT_FALSE(cpu.isFlagSet(z80::P));      // P/V is not set
    ASSERT_TRUE(cpu.isFlagSet(z80::N));       // Subtraction flag is set
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // Carry is not set since A was 0
}
TEST_F(GeneralPurposeGroupTest, CCF)
{
    // Initialize the Carry flag (CY) to 1
    cpu.setFlag(z80::C_flag); // Set the Carry flag (CY = 1)

    // Set up the bus to execute the CCF instruction (opcode 0x3F)
    bus.write(cpu.PC, 0x3F);       // Opcode for CCF
    cpu.execute(bus.read(cpu.PC)); // Execute the instruction

    // Verify that the Carry flag is inverted (CY was 1, now should be 0)
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // CY should be reset (CY = 0)
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N should be reset

    // If CY was 0 initially, we should verify that CY is set
    cpu.clearFlag(z80::C_flag);    // Now set Carry flag to 0
    bus.write(cpu.PC, 0x3F);       // Execute CCF instruction again
    cpu.execute(bus.read(cpu.PC)); // Invert the Carry flag

    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag)); // CY should be set (CY = 1)
}
TEST_F(GeneralPurposeGroupTest, SCF)
{
    // Initialize the Carry flag (CY) to 1
    cpu.clearFlag(z80::C_flag); // Set the Carry flag (CY = 1)

    // Set up the bus to execute the CCF instruction (opcode 0x3F)
    bus.write(cpu.PC, 0x37);       // Opcode for CCF
    cpu.execute(bus.read(cpu.PC)); // Execute the instruction

    // Verify that the Carry flag is inverted (CY was 1, now should be 0)
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));  // CY should be reset (CY = 0)
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N should be reset
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // N should be reset
}
TEST_F(GeneralPurposeGroupTest, IM0)
{

    bus.write(cpu.PC, 0xED);
    bus.write(cpu.PC + 1, 0x46);
    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(cpu.interruptMode, InterruptMode::Mode0);
}
TEST_F(GeneralPurposeGroupTest, IM1)
{

    bus.write(cpu.PC, 0xED);
    bus.write(cpu.PC + 1, 0x56);
    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(cpu.interruptMode, InterruptMode::Mode1);
}
TEST_F(GeneralPurposeGroupTest, IM2)
{

    bus.write(cpu.PC, 0xED);
    bus.write(cpu.PC + 1, 0x5E);
    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(cpu.interruptMode, InterruptMode::Mode2);
}
TEST_F(GeneralPurposeGroupTest, NOP)
{

    bus.write(cpu.PC, 0x00);

    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(cpu.A, 0xff);
}
TEST_F(GeneralPurposeGroupTest, LoadROMTest)
{
    // Prepare a simple test ROM with known values.
    const std::string testROMPath = "C:\\Users\\eelip\\Downloads\\48.rom";

    // Load the ROM into memory
    bus.loadROM(testROMPath);
    std::cout << "0x" << std::hex << bus.read(0x3FFE) << ": 0x" << bus.read(0x3FFE) << "\n";

    for (int i = 0; i < 10000; i++)
    {
        uint8_t opCode = bus.read(cpu.PC);
        uint16_t PCbefore = cpu.PC;
        cpu.execute(bus.read(cpu.PC));
    }
    bool videoMemoryPopulated = false;
    if (bus.read(0x3ff0) == 0x3c)
    {
        videoMemoryPopulated = true;
    }

    if (bus.read(0x3ff0 - 1) == 0x42)
    {
        videoMemoryPopulated = true;
    }

    if (bus.read(0x3ffE) == 0x99)
    {
        videoMemoryPopulated = true;
    }

    std::cout << "Dumping video memory contents (0x4000 to 0x57FF):\n";
    for (uint16_t addr = 0x4000; addr <= 0x57FF; ++addr)
    {
        uint8_t value = bus.read(addr);
        std::cout << "0x" << std::hex << bus.read(0x3FFE) << ": 0x" << (int)value << "\n";
    }

    // Assert that video memory contains non-zero values.
    ASSERT_TRUE(videoMemoryPopulated) << "Video memory should contain non-zero values after ROM execution.";
}

class SixteenBitArithmetictest : public ::testing::Test
{
protected:
    Memory memory;
    Bus bus;
    z80 cpu;

    SixteenBitArithmetictest() : memory(0x10000), bus(memory), cpu()
    {
        cpu.reset(&bus);
    }
};

TEST_F(SixteenBitArithmetictest, ADD_HL_SS)
{
    // Set initial values for HL and DE registers
    cpu.H = 0x42; // HL = 0x4242
    cpu.L = 0x42;
    cpu.D = 0x11; // DE = 0x1111
    cpu.E = 0x11;

    memory.write(cpu.PC, 0x19); // Opcode for ADD HL, DE

    // Execute the ADD HL, DE instruction
    cpu.execute(memory.read(cpu.PC));

    // Verify the result in HL
    ASSERT_EQ(cpu.getHL(), 0x5353); // Expected result = 0x4242 + 0x1111 = 0x5353

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // No carry from bit 15 (sum is within 16 bits)
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is set if there's a carry from bit 11 (0x424 + 0x111 > 0xFFF)

    // Verify the program counter (increment by 2 for the instruction)
    ASSERT_EQ(cpu.PC, 1); // Increment PC by 2 after the instruction
}

TEST_F(SixteenBitArithmetictest, ADD_HL_SS2)
{
    // Set initial values for HL and DE registers
    cpu.H = 0xEE; // HL = 0x4242
    cpu.L = 0x00;
    cpu.D = 0x20; // DE = 0x1111
    cpu.E = 0x00;

    memory.write(cpu.PC, 0x19); // Opcode for ADD HL, DE

    // Execute the ADD HL, DE instruction
    cpu.execute(memory.read(cpu.PC));

    // Verify the result in HL
    ASSERT_EQ(cpu.getHL(), 0x0E00); // Expected result = 0x4242 + 0x1111 = 0x5353

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag is reset
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));  // No carry from bit 15 (sum is within 16 bits)
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is set if there's a carry from bit 11 (0x424 + 0x111 > 0xFFF)

    // Verify the program counter (increment by 2 for the instruction)
    ASSERT_EQ(cpu.PC, 1); // Increment PC by 2 after the instruction
}
TEST_F(SixteenBitArithmetictest, ADD_HL_SS3)
{
    // Set initial values for HL and DE registers
    cpu.H = 0x0F; // HL = 0x4242
    cpu.L = 0xFF;
    cpu.D = 0x00; // DE = 0x1111
    cpu.E = 0x01;

    memory.write(cpu.PC, 0x19); // Opcode for ADD HL, DE

    // Execute the ADD HL, DE instruction
    cpu.execute(memory.read(cpu.PC));

    // Verify the result in HL
    ASSERT_EQ(cpu.getHL(), 0x1000); // Expected result = 0x4242 + 0x1111 = 0x5353

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // No carry from bit 15 (sum is within 16 bits)
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag));  // H is set if there's a carry from bit 11 (0x424 + 0x111 > 0xFFF)

    // Verify the program counter (increment by 2 for the instruction)
    ASSERT_EQ(cpu.PC, 1); // Increment PC by 2 after the instruction
}
TEST_F(SixteenBitArithmetictest, ADC_HL_SP_WithCarry)
{
    // Initialize CPU registers
    cpu.H = 0xEE; // HL = 0xF0FF
    cpu.L = 0x00;
    cpu.SP = 0x2000;

    // Set the carry flag
    cpu.setFlag(z80::C_flag);

    // Memory setup
    memory.write(cpu.PC, 0xED);     // First byte of ADC HL, ss opcode
    memory.write(cpu.PC + 1, 0x7A); // Opcode for ADC HL, SP

    uint16_t initialHL = cpu.getHL();
    uint16_t initialSP = cpu.SP;

    // Execute the ADC HL, SP instruction
    cpu.execute(memory.read(cpu.PC));

    // Verify the results
    ASSERT_EQ(cpu.getHL(), 0x0E01); // HL should contain 0x10101 (F0FF + 0101 + 1)

    // Verify carry flag
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag)); // Carry from bit 15 in this case

    // Verify half-carry flag
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // Half-carry should be set (carry from bit 11)

    // Verify overflow flag (P/V)
    ASSERT_FALSE(cpu.isFlagSet(z80::P)); // No signed overflow in this case

    // Verify Zero and Sign flags
    ASSERT_FALSE(cpu.isFlagSet(z80::Z)); // Result is not zero
    ASSERT_FALSE(cpu.isFlagSet(z80::S)); // Sign is negative (bit 15 is 1)

    // Verify N flag
    ASSERT_FALSE(cpu.isFlagSet(z80::N)); // N is reset for addition

    // Ensure the program counter advanced correctly
    ASSERT_EQ(cpu.PC, 2);
}
TEST_F(SixteenBitArithmetictest, ADC_HL_SP_WithCarry2)
{
    // Initialize CPU registers
    cpu.H = 0x00; // HL = 0xF0FF
    cpu.L = 0x00;
    cpu.SP = 0x0000;
    // Set the carry flag
    cpu.setFlag(z80::C_flag);
    // Memory setup
    memory.write(cpu.PC, 0xED);     // First byte of ADC HL, ss opcode
    memory.write(cpu.PC + 1, 0x7A); // Opcode for ADC HL, SP

    uint16_t initialHL = cpu.getHL();
    uint16_t initialSP = cpu.SP;
    // Execute the ADC HL, SP instruction
    cpu.execute(memory.read(cpu.PC));
    // Verify the results
    ASSERT_EQ(cpu.getHL(), 0x0001); // HL should contain 0x10101 (F0FF + 0101 + 1)

    ASSERT_EQ(cpu.PC, 2);
}
TEST_F(SixteenBitArithmetictest, SBC_HL_SP_WithCarry)
{
    // Initialize CPU registers
    cpu.H = 0x99; // HL = 0xF0FF
    cpu.L = 0x99;
    cpu.SP = 0x1111;

    // Set the carry flag
    cpu.setFlag(z80::C_flag);

    // Memory setup
    memory.write(cpu.PC, 0xED);
    memory.write(cpu.PC + 1, 0x72);

    uint16_t initialHL = cpu.getHL();
    uint16_t initialSP = cpu.SP;

    // Execute the ADC HL, SP instruction
    cpu.execute(memory.read(cpu.PC));

    // Verify the results
    ASSERT_EQ(cpu.getHL(), 0x8887);

    // Verify carry flag
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // Carry from bit 15 in this case

    // Verify half-carry flag
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // Half-carry should be set (carry from bit 11)

    // Verify overflow flag (P/V)
    ASSERT_FALSE(cpu.isFlagSet(z80::P)); // No signed overflow in this case

    // Verify Zero and Sign flags
    ASSERT_FALSE(cpu.isFlagSet(z80::Z)); // Result is not zero
    ASSERT_TRUE(cpu.isFlagSet(z80::S));  // Sign is negative (bit 15 is 1)

    // Verify N flag
    ASSERT_TRUE(cpu.isFlagSet(z80::N));

    // Ensure the program counter advanced correctly
    ASSERT_EQ(cpu.PC, 2);
}
TEST_F(SixteenBitArithmetictest, ADD_IX_PP)
{
    // Set initial values for HL and DE registers
    cpu.B = 0x0F; // HL = 0x4242
    cpu.C = 0xFF;
    cpu.IX = 0x0001;

    memory.write(cpu.PC, 0xDD);
    memory.write(cpu.PC + 1, 0x09);

    // Execute the ADD HL, DE instruction
    cpu.execute(memory.read(cpu.PC));

    // Verify the result in HL
    ASSERT_EQ(cpu.IX, 0x1000); // Expected result = 0x4242 + 0x1111 = 0x5353

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // No carry from bit 15 (sum is within 16 bits)
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag));  // H is set if there's a carry from bit 11 (0x424 + 0x111 > 0xFFF)

    // Verify the program counter (increment by 2 for the instruction)
    ASSERT_EQ(cpu.PC, 2); // Increment PC by 2 after the instruction
}
TEST_F(SixteenBitArithmetictest, ADD_IY_RR)
{

    cpu.IY = 0x0001;

    bus.write(cpu.PC, 0xFD);
    bus.write(cpu.PC + 1, 0x09);

    // Execute the ADD HL, DE instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify the result in HL
    ASSERT_EQ(cpu.IY, 0x0002); // Expected result = 0x4242 + 0x1111 = 0x5353

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N flag is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag)); // No carry from bit 15 (sum is within 16 bits)
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is set if there's a carry from bit 11 (0x424 + 0x111 > 0xFFF)

    // Verify the program counter (increment by 2 for the instruction)
    ASSERT_EQ(cpu.PC, 2); // Increment PC by 2 after the instruction
}
TEST_F(SixteenBitArithmetictest, INC_HL)
{
    // Initialize the CPU registers
    cpu.H = 0x10; // Set HL register to 0x1000
    cpu.L = 0x00;

    uint16_t initialHL = cpu.getHL(); // Store the initial HL value

    // Execute the INC HL instruction
    cpu.execute(0x23); // The opcode for INC HL is 0x34

    ASSERT_EQ(cpu.getHL(), initialHL + 1);

    // Initialize the CPU registers
    cpu.D = 0x10; // Set HL register to 0x1000
    cpu.E = 0x00;

    uint16_t initialDE = cpu.getDE(); // Store the initial HL value

    // Execute the INC HL instruction
    cpu.execute(0x13); // The opcode for INC HL is 0x34

    ASSERT_EQ(cpu.getDE(), initialDE + 1);
}
TEST_F(SixteenBitArithmetictest, INC_IX)
{
    // Initialize the CPU registers
    cpu.IX = 0x1000;

    uint16_t initialHL = cpu.IX;
    bus.write(cpu.PC, 0xDD);
    bus.write(cpu.PC + 1, 0x23);
    cpu.execute(bus.read(cpu.PC)); // The opcode for INC HL is 0x34

    ASSERT_EQ(cpu.IX, initialHL + 1);
}
TEST_F(SixteenBitArithmetictest, INC_IY)
{
    // Initialize the CPU registers
    cpu.IY = 0x1000;

    uint16_t initialHL = cpu.IY; // Store the initial HL value

    bus.write(cpu.PC, 0xFD);
    bus.write(cpu.PC + 1, 0x23);
    cpu.execute(bus.read(cpu.PC)); // The opcode for INC HL is 0x34

    ASSERT_EQ(cpu.IY, initialHL + 1);
}
TEST_F(SixteenBitArithmetictest, DEC_HL)
{
    // Initialize the CPU registers
    cpu.H = 0x10; // Set HL register to 0x1000
    cpu.L = 0x00;

    uint16_t initialHL = cpu.getHL(); // Store the initial HL value

    // Execute the INC HL instruction
    cpu.execute(0x2B); // The opcode for INC HL is 0x34

    ASSERT_EQ(cpu.getHL(), initialHL - 1);

    // Initialize the CPU registers
    cpu.D = 0x10; // Set HL register to 0x1000
    cpu.E = 0x00;

    uint16_t initialDE = cpu.getDE(); // Store the initial HL value

    // Execute the INC HL instruction
    cpu.execute(0x1B); // The opcode for INC HL is 0x34

    ASSERT_EQ(cpu.getDE(), initialDE - 1);
}
TEST_F(SixteenBitArithmetictest, DEC_IX)
{
    // Initialize the CPU registers
    cpu.IX = 0x1000;

    uint16_t initialHL = cpu.IX;
    bus.write(cpu.PC, 0xDD);
    bus.write(cpu.PC + 1, 0x2B);
    cpu.execute(bus.read(cpu.PC)); // The opcode for INC HL is 0x34

    ASSERT_EQ(cpu.IX, initialHL - 1);
}
TEST_F(SixteenBitArithmetictest, DEC_IY)
{
    // Initialize the CPU registers
    cpu.IY = 0x1000;

    uint16_t initialHL = cpu.IY; // Store the initial HL value

    bus.write(cpu.PC, 0xFD);
    bus.write(cpu.PC + 1, 0x2B);
    cpu.execute(bus.read(cpu.PC)); // The opcode for INC HL is 0x34

    ASSERT_EQ(cpu.IY, initialHL - 1);
}
class RotateAndShiftGroup : public ::testing::Test
{
protected:
    Memory memory;
    Bus bus;
    z80 cpu;

    RotateAndShiftGroup() : memory(0x10000), bus(memory), cpu()
    {
        cpu.reset(&bus);
    }
};

TEST_F(RotateAndShiftGroup, RLCA)
{
    // Initialize the CPU registers
    cpu.A = 0b10001000; // Set A to 0b11010010

    uint8_t initialA = cpu.A;

    // Write the RLCA instruction to memory at PC
    bus.write(cpu.PC, 0x07); // RLCA opcode

    // Execute the RLCA instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify that A was rotated left circularly
    ASSERT_EQ(cpu.A, 0b00010001); // Bit 7 is moved to bit 0, other bits shifted left

    // Verify that the Carry flag was updated correctly
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag)); // Carry should be set because bit 7 of initial A was 1

    // Verify that other flags are unaffected
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
    ASSERT_EQ(cpu.A, 0b00010001);
}
TEST_F(RotateAndShiftGroup, RLA)
{
    cpu.A = 0b01110110;
    bus.write(cpu.PC, 0x17); // RLA opcode
    // Execute the RLCA instruction
    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(cpu.A, 0b11101101);
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
}
TEST_F(RotateAndShiftGroup, RRCA)
{
    cpu.A = 0b00010001;
    bus.write(cpu.PC, 0x0F); // RLA opcode
    // Execute the RLCA instruction
    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(cpu.A, 0b10001000);
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
}
TEST_F(RotateAndShiftGroup, RRA)
{
    cpu.A = 0b11100001;
    cpu.clearFlag(z80::C_flag);
    bus.write(cpu.PC, 0x1F); // RLA opcode
    // Execute the RLCA instruction
    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(cpu.A, 0b01110000);
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
}
TEST_F(RotateAndShiftGroup, RLC_R)
{
    cpu.C = 0b10001000;
    cpu.clearFlag(z80::C_flag);
    bus.write(cpu.PC, 0xCB);
    bus.write(cpu.PC + 1, 0x01);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(cpu.C, 0b00010001);
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));
    ASSERT_TRUE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
}
TEST_F(RotateAndShiftGroup, RLC_HL)
{
    uint8_t value = 0b10001000;
    cpu.H = 0x10;
    cpu.L = 0x10;
    cpu.clearFlag(z80::C_flag);

    bus.write(cpu.getHL(), value);
    bus.write(cpu.PC, 0xCB);
    bus.write(cpu.PC + 1, 0x06);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(bus.read(cpu.getHL()), 0b00010001);
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));
    ASSERT_TRUE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
}

TEST_F(RotateAndShiftGroup, RLC_IX_D)
{
    uint8_t value = 0b10001000;
    uint16_t address = 0x0191 + 0x10;
    cpu.IX = 0x0191;
    cpu.clearFlag(z80::C_flag);

    bus.write(address, value);
    bus.write(cpu.PC, 0xDD);
    bus.write(cpu.PC + 1, 0xCB);
    bus.write(cpu.PC + 2, 0x10);
    bus.write(cpu.PC + 3, 0x01);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(bus.read(address), 0b00010001);
    ASSERT_EQ(cpu.C, 0b00010001);
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));
    ASSERT_TRUE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
}
TEST_F(RotateAndShiftGroup, RLC_IY_D)
{
    uint8_t value = 0b10001000;
    uint16_t address = 0x0191 + 0x10;
    cpu.IY = 0x0191;
    cpu.clearFlag(z80::C_flag);

    bus.write(address, value);
    bus.write(cpu.PC, 0xFD);
    bus.write(cpu.PC + 1, 0xCB);
    bus.write(cpu.PC + 2, 0x10);
    bus.write(cpu.PC + 3, 0x07);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(bus.read(address), 0b00010001);
    ASSERT_EQ(cpu.A, 0b00010001);
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));
    ASSERT_TRUE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
}
TEST_F(RotateAndShiftGroup, RL_R)
{
    cpu.B = 0b10001111;
    cpu.clearFlag(z80::C_flag);
    bus.write(cpu.PC, 0xCB);
    bus.write(cpu.PC + 1, 0x10);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(cpu.B, 0b00011110);
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));
    ASSERT_TRUE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
}
TEST_F(RotateAndShiftGroup, RL_HL)
{
    cpu.H = 0x01;
    cpu.L = 0x91;
    cpu.clearFlag(z80::C_flag);
    bus.write(cpu.getHL(), 0b10001111);
    bus.write(cpu.PC, 0xCB);
    bus.write(cpu.PC + 1, 0x16);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(bus.read(cpu.getHL()), 0b00011110);
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));
    ASSERT_TRUE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
}
TEST_F(RotateAndShiftGroup, RL_IX_D)
{
    cpu.IX = 0x0191;
    int8_t d = 0x10;
    cpu.clearFlag(z80::C_flag);
    bus.write(cpu.IX + d, 0b10001111);
    bus.write(cpu.PC, 0xDD);
    bus.write(cpu.PC + 1, 0xCB);
    bus.write(cpu.PC + 2, d);
    bus.write(cpu.PC + 3, 0x16);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(bus.read(cpu.IX + d), 0b00011110);
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));
    ASSERT_TRUE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
}
TEST_F(RotateAndShiftGroup, RL_IY_D)
{
    cpu.IY = 0x0191;
    int8_t d = 0x10;
    cpu.clearFlag(z80::C_flag);
    bus.write(cpu.IY + d, 0b10001111);

    bus.write(cpu.PC, 0xFD);
    bus.write(cpu.PC + 1, 0xCB);
    bus.write(cpu.PC + 2, d);
    bus.write(cpu.PC + 3, 0x12);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(bus.read(cpu.IY + d), 0b00011110);
    ASSERT_EQ(cpu.D, 0b00011110);
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));
    ASSERT_TRUE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
}
TEST_F(RotateAndShiftGroup, RRC_R)
{
    cpu.A = 0b00010001;
    int8_t d = 0x10;

    bus.write(cpu.PC, 0xCB);
    bus.write(cpu.PC + 1, 0x0F);

    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(cpu.A, 0b10001000);
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));
    ASSERT_TRUE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
}
TEST_F(RotateAndShiftGroup, RRC_HL)
{
    uint8_t value = 0b00110001;
    int8_t d = 0x10;
    cpu.H = 0x10;
    cpu.L = 0x10;
    cpu.F = 0x00;
    bus.write(cpu.getHL(), value);
    bus.write(cpu.PC, 0xCB);
    bus.write(cpu.PC + 1, 0x0E);

    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(bus.read(cpu.getHL()), 0b10011000);
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::P));
    ASSERT_TRUE(cpu.isFlagSet(z80::S));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
}
TEST_F(RotateAndShiftGroup, RRC_IX_D)
{
    uint8_t value = 0b00110001;
    int8_t d = 0x10;

    cpu.F = 0x00;
    bus.write(cpu.IX + d, value);
    bus.write(cpu.PC, 0xDD);
    bus.write(cpu.PC + 1, 0xCB);
    bus.write(cpu.PC + 2, d);
    bus.write(cpu.PC + 3, 0x0E);

    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(bus.read(cpu.IX + d), 0b10011000);
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::P));
    ASSERT_TRUE(cpu.isFlagSet(z80::S));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
}
TEST_F(RotateAndShiftGroup, RRC_IY_D)
{
    uint8_t value = 0b00110001;
    int8_t d = 0x10;

    cpu.F = 0x00;
    bus.write(cpu.IY + d, value);
    bus.write(cpu.PC, 0xFD);
    bus.write(cpu.PC + 1, 0xCB);
    bus.write(cpu.PC + 2, d);
    bus.write(cpu.PC + 3, 0x08);

    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(bus.read(cpu.IY + d), 0b10011000);
    ASSERT_EQ(cpu.B, 0b10011000);
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::P));
    ASSERT_TRUE(cpu.isFlagSet(z80::S));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
}

TEST_F(RotateAndShiftGroup, RR_R)
{
    cpu.A = 0b11011101;
    cpu.clearFlag(z80::C_flag);
    bus.write(cpu.PC, 0xCB);
    bus.write(cpu.PC + 1, 0x1F);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(cpu.A, 0b01101110);
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::S));
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));
}
TEST_F(RotateAndShiftGroup, RR_HL)
{
    cpu.H = 0x01;
    cpu.L = 0x91;

    bus.write(cpu.getHL(), 0b11011101);
    bus.write(cpu.PC, 0xCB);
    bus.write(cpu.PC + 1, 0x1E);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(bus.read(cpu.getHL()), 0b11101110);
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag));
    ASSERT_TRUE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
    ASSERT_TRUE(cpu.isFlagSet(z80::S));
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));
}
TEST_F(RotateAndShiftGroup, RR_IX_D)
{
    cpu.IX = 0x0191;
    int8_t d = 0x10;
    cpu.clearFlag(z80::C_flag);
    bus.write(cpu.IX + d, 0b11011101);
    bus.write(cpu.PC, 0xDD);
    bus.write(cpu.PC + 1, 0xCB);
    bus.write(cpu.PC + 2, d);
    bus.write(cpu.PC + 3, 0x1E);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(bus.read(cpu.IX + d), 0b01101110);
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag)); // toDo Might not be right
    ASSERT_FALSE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
}
TEST_F(RotateAndShiftGroup, RR_IY_D)
{
    cpu.IX = 0x0191;
    int8_t d = 0x10;
    cpu.clearFlag(z80::C_flag);
    bus.write(cpu.IY + d, 0b11011101);
    bus.write(cpu.PC, 0xFD);
    bus.write(cpu.PC + 1, 0xCB);
    bus.write(cpu.PC + 2, d);
    bus.write(cpu.PC + 3, 0x1E);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(bus.read(cpu.IY + d), 0b01101110);
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag)); // toDo Might not be right
    ASSERT_FALSE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
}
TEST_F(RotateAndShiftGroup, SLA_R)
{
    cpu.L = 0b10110001;
    cpu.F = 0x00;
    cpu.clearFlag(z80::C_flag);
    bus.write(cpu.PC, 0xCB);
    bus.write(cpu.PC + 1, 0x25);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(cpu.L, 0b01100010);
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::S));
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));
}
TEST_F(RotateAndShiftGroup, SLA_HL)
{
    cpu.H = 0x01;
    cpu.L = 0x91;

    bus.write(cpu.getHL(), 0b01011001);
    bus.write(cpu.PC, 0xCB);
    bus.write(cpu.PC + 1, 0x26);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(bus.read(cpu.getHL()), 0b10110010);
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag));
    ASSERT_TRUE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
    ASSERT_TRUE(cpu.isFlagSet(z80::S));
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));
}
TEST_F(RotateAndShiftGroup, SLA_IX_D)
{
    cpu.IX = 0x0991;

    uint8_t d = 0x10;

    bus.write(cpu.IX + d, 0b01011001);
    bus.write(cpu.PC, 0xDD);
    bus.write(cpu.PC + 1, 0xCB);
    bus.write(cpu.PC + 2, d);
    bus.write(cpu.PC + 3, 0x26);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(bus.read(cpu.IX + d), 0b10110010);
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag));
    ASSERT_TRUE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
    ASSERT_TRUE(cpu.isFlagSet(z80::S));
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));
}
TEST_F(RotateAndShiftGroup, SLA_IY_D)
{
    cpu.IY = 0x0991;

    uint8_t d = 0x10;

    bus.write(cpu.IY + d, 0b01011001);
    bus.write(cpu.PC, 0xFD);
    bus.write(cpu.PC + 1, 0xCB);
    bus.write(cpu.PC + 2, d);
    bus.write(cpu.PC + 3, 0x26);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(bus.read(cpu.IY + d), 0b10110010);
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag));
    ASSERT_TRUE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
    ASSERT_TRUE(cpu.isFlagSet(z80::S));
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));
}

TEST_F(RotateAndShiftGroup, SRA_R)
{
    cpu.A = 0b10111000;
    cpu.F = 0x00;
    bus.write(cpu.PC, 0xCB);
    bus.write(cpu.PC + 1, 0x2F);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(cpu.A, 0b11011100);
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
    ASSERT_TRUE(cpu.isFlagSet(z80::S));
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));
}
TEST_F(RotateAndShiftGroup, SRA_HL)
{
    cpu.H = 0x10;
    cpu.L = 0x10;
    cpu.F = 0x00;
    bus.write(cpu.getHL(), 0b10111000);
    bus.write(cpu.PC, 0xCB);
    bus.write(cpu.PC + 1, 0x2E);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(bus.read(cpu.getHL()), 0b11011100);
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
    ASSERT_TRUE(cpu.isFlagSet(z80::S));
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));
}
TEST_F(RotateAndShiftGroup, SRA_IX_D)
{
    cpu.IY = 0x0991;

    uint8_t d = 0x10;

    bus.write(cpu.IX + d, 0b10111000);
    bus.write(cpu.PC, 0xDD);
    bus.write(cpu.PC + 1, 0xCB);
    bus.write(cpu.PC + 2, d);
    bus.write(cpu.PC + 3, 0x2E);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(bus.read(cpu.IX + d), 0b11011100);
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
    ASSERT_TRUE(cpu.isFlagSet(z80::S));
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));
}
TEST_F(RotateAndShiftGroup, SRA_IY_D)
{
    cpu.IY = 0x0991;

    uint8_t d = 0x10;

    bus.write(cpu.IY + d, 0b10111000);
    bus.write(cpu.PC, 0xFD);
    bus.write(cpu.PC + 1, 0xCB);
    bus.write(cpu.PC + 2, d);
    bus.write(cpu.PC + 3, 0x2E);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(bus.read(cpu.IY + d), 0b11011100);
    ASSERT_FALSE(cpu.isFlagSet(z80::C_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
    ASSERT_TRUE(cpu.isFlagSet(z80::S));
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));
}
TEST_F(RotateAndShiftGroup, SRL_R)
{
    cpu.D = 0b10110001;
    cpu.F = 0x00;
    bus.write(cpu.PC, 0xCB);
    bus.write(cpu.PC + 1, 0x3A);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(cpu.D, 0b01011000);
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::S));
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));
}
TEST_F(RotateAndShiftGroup, SRL_HL)
{
    cpu.H = 0x10;
    cpu.L = 0x10;
    cpu.F = 0x00;
    bus.write(cpu.getHL(), 0b01011001);
    bus.write(cpu.PC, 0xCB);
    bus.write(cpu.PC + 1, 0x3E);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(bus.read(cpu.getHL()), 0b00101100);
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::S));
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));
}
TEST_F(RotateAndShiftGroup, SRL_IX_D)
{
    cpu.IY = 0x0991;
    cpu.F = 0x00;

    int8_t d = 0x10;

    bus.write(cpu.IX + d, 0b01011001);
    bus.write(cpu.PC, 0xDD);
    bus.write(cpu.PC + 1, 0xCB);
    bus.write(cpu.PC + 2, d);
    bus.write(cpu.PC + 3, 0x3E);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(bus.read(cpu.IX + d), 0b00101100);
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::S));
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));
}
TEST_F(RotateAndShiftGroup, SRL_IY_D)
{
    cpu.IY = 0x0991;

    uint8_t d = 0x10;

    bus.write(cpu.IY + d, 0b01011001);
    bus.write(cpu.PC, 0xFD);
    bus.write(cpu.PC + 1, 0xCB);
    bus.write(cpu.PC + 2, d);
    bus.write(cpu.PC + 3, 0x3E);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(bus.read(cpu.IY + d), 0b00101100);
    ASSERT_TRUE(cpu.isFlagSet(z80::C_flag));
    ASSERT_FALSE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::S));
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));
}
TEST_F(RotateAndShiftGroup, RLD)
{
    cpu.A = 0b01111010;

    cpu.H = 0x10;
    cpu.L = 0x10;

    bus.write(cpu.getHL(), 0b00110001);
    bus.write(cpu.PC, 0xED);
    bus.write(cpu.PC + 1, 0x6F);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(bus.read(cpu.getHL()), 0b00011010);
    ASSERT_EQ(cpu.A, 0b01110011);

    ASSERT_FALSE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::S));
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));
}
TEST_F(RotateAndShiftGroup, RRD)
{
    cpu.A = 0b10000100;

    cpu.H = 0x10;
    cpu.L = 0x10;

    bus.write(cpu.getHL(), 0b00100000);
    bus.write(cpu.PC, 0xED);
    bus.write(cpu.PC + 1, 0x67);

    cpu.execute(bus.read(cpu.PC));
    // Verify that A was rotated left circularly
    ASSERT_EQ(bus.read(cpu.getHL()), 0b01000010);
    ASSERT_EQ(cpu.A, 0b10000000);

    ASSERT_FALSE(cpu.isFlagSet(z80::P));
    ASSERT_FALSE(cpu.isFlagSet(z80::H_flag)); // H is reset
    ASSERT_FALSE(cpu.isFlagSet(z80::N));      // N is reset
    ASSERT_TRUE(cpu.isFlagSet(z80::S));
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));
}

class BitGroup : public ::testing::Test
{
protected:
    Memory memory;
    Bus bus;
    z80 cpu;

    BitGroup() : memory(0x10000), bus(memory), cpu()
    {
        cpu.reset(&bus);
    }
};
TEST_F(BitGroup, BIT_b_r)
{
    // Test setup for BIT 2, B
    cpu.B = 0b00000100; // Set bit 2 in Register B
    cpu.F = 0x00;
    bus.write(cpu.PC, 0xCB);     // First byte of BIT instruction
    bus.write(cpu.PC + 1, 0x50); // Second byte: BIT 2, B

    cpu.execute(bus.read(cpu.PC));

    // Verify that Z flag is reset because bit 2 is 1
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag)); // H is always set
    ASSERT_FALSE(cpu.isFlagSet(z80::N));     // N is reset

    // Test when bit 2 in Register B is 0
    cpu.B = 0b11111011;          // Clear bit 2 in Register B
    bus.write(cpu.PC, 0xCB);     // First byte of BIT instruction
    bus.write(cpu.PC + 1, 0x50); // Second byte: BIT 2, B

    cpu.execute(bus.read(cpu.PC));

    // Verify that Z flag is set because bit 2 is 0
    ASSERT_TRUE(cpu.isFlagSet(z80::Z));
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag)); // H remains set
    ASSERT_FALSE(cpu.isFlagSet(z80::N));     // N remains reset
}
TEST_F(BitGroup, BIT_b_HL)
{
    // Set HL to point to memory location 0x4444
    cpu.H = 0x44;
    cpu.L = 0x44;
    cpu.F = 0x00;

    // Case 1: Test BIT 4, (HL) where bit 4 is set (1)
    bus.write(0x4444, 0b00010000); // Memory at 0x4444: bit 4 is 1
    bus.write(cpu.PC, 0xCB);       // First byte of BIT instruction
    bus.write(cpu.PC + 1, 0x66);   // Second byte: BIT 4, (HL)

    cpu.execute(bus.read(cpu.PC));

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));     // Z flag should be reset because bit 4 is 1
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag)); // H flag is always set
    ASSERT_FALSE(cpu.isFlagSet(z80::N));     // N flag is reset

    // Case 2: Test BIT 4, (HL) where bit 4 is clear (0)
    bus.write(0x4444, 0b11101111); // Memory at 0x4444: bit 4 is 0
    bus.write(cpu.PC, 0xCB);       // First byte of BIT instruction
    bus.write(cpu.PC + 1, 0x66);   // Second byte: BIT 4, (HL)

    cpu.execute(bus.read(cpu.PC));

    // Verify flags
    ASSERT_TRUE(cpu.isFlagSet(z80::Z));      // Z flag should be set because bit 4 is 0
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag)); // H flag remains set
    ASSERT_FALSE(cpu.isFlagSet(z80::N));     // N flag remains reset
}

TEST_F(BitGroup, BIT_b_IX_plus_d)
{
    // Set IX to point to base address 0x2000
    cpu.IX = 0x2000;
    // Set displacement d to 4
    int8_t displacement = 4;

    // Case 1: BIT 6, (IX + d) where bit 6 is set (1)
    bus.write(0x2000 + displacement, 0b01000000); // Memory at 0x2004: bit 6 is set
    bus.write(cpu.PC, 0xDD);                      // First byte for IX instructions
    bus.write(cpu.PC + 1, 0xCB);                  // Prefix for BIT instructions with displacement
    bus.write(cpu.PC + 2, displacement);          // Displacement
    bus.write(cpu.PC + 3, 0x76);                  // BIT 6, (IX + d) - 0x76 encodes BIT 6

    cpu.execute(bus.read(cpu.PC));

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));     // Z flag reset because bit 6 is 1
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag)); // H flag is always set
    ASSERT_FALSE(cpu.isFlagSet(z80::N));     // N flag is reset

    // Case 2: BIT 6, (IX + d) where bit 6 is clear (0)
    bus.write(0x2000 + displacement, 0b00000000); // Memory at 0x2004: bit 6 is clear
    bus.write(cpu.PC, 0xDD);                      // First byte for IX instructions
    bus.write(cpu.PC + 1, 0xCB);                  // Prefix for BIT instructions with displacement
    bus.write(cpu.PC + 2, displacement);          // Displacement
    bus.write(cpu.PC + 3, 0x76);                  // BIT 6, (IX + d) - 0x76 encodes BIT 6

    cpu.execute(bus.read(cpu.PC));

    // Verify flags
    ASSERT_TRUE(cpu.isFlagSet(z80::Z));      // Z flag set because bit 6 is 0
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag)); // H flag remains set
    ASSERT_FALSE(cpu.isFlagSet(z80::N));     // N flag remains reset
}
TEST_F(BitGroup, BIT_b_IY_plus_d)
{
    // Set IX to point to base address 0x2000
    cpu.IY = 0x2000;
    // Set displacement d to 4
    int8_t displacement = 4;

    // Case 1: BIT 6, (IX + d) where bit 6 is set (1)
    bus.write(0x2000 + displacement, 0b01000000); // Memory at 0x2004: bit 6 is set
    bus.write(cpu.PC, 0xFD);                      // First byte for IX instructions
    bus.write(cpu.PC + 1, 0xCB);                  // Prefix for BIT instructions with displacement
    bus.write(cpu.PC + 2, displacement);          // Displacement
    bus.write(cpu.PC + 3, 0x76);                  // BIT 6, (IX + d) - 0x76 encodes BIT 6

    cpu.execute(bus.read(cpu.PC));

    // Verify flags
    ASSERT_FALSE(cpu.isFlagSet(z80::Z));     // Z flag reset because bit 6 is 1
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag)); // H flag is always set
    ASSERT_FALSE(cpu.isFlagSet(z80::N));     // N flag is reset

    // Case 2: BIT 6, (IX + d) where bit 6 is clear (0)
    bus.write(0x2000 + displacement, 0b00000000); // Memory at 0x2004: bit 6 is clear
    bus.write(cpu.PC, 0xFD);                      // First byte for IX instructions
    bus.write(cpu.PC + 1, 0xCB);                  // Prefix for BIT instructions with displacement
    bus.write(cpu.PC + 2, displacement);          // Displacement
    bus.write(cpu.PC + 3, 0x76);                  // BIT 6, (IX + d) - 0x76 encodes BIT 6

    cpu.execute(bus.read(cpu.PC));

    // Verify flags
    ASSERT_TRUE(cpu.isFlagSet(z80::Z));      // Z flag set because bit 6 is 0
    ASSERT_TRUE(cpu.isFlagSet(z80::H_flag)); // H flag remains set
    ASSERT_FALSE(cpu.isFlagSet(z80::N));     // N flag remains reset
}
TEST_F(BitGroup, SET_b_r)
{
    // Case 1: SET 4, A (Set bit 4 in Register A)
    cpu.A = 0b00000000;          // Initialize A to all 0s
    bus.write(cpu.PC, 0xCB);     // Prefix for BIT, SET, and RES instructions
    bus.write(cpu.PC + 1, 0xE7); // 0xE7 = SET 4, A (b=4, r=A)

    cpu.execute(bus.read(cpu.PC));

    // Verify the result
    ASSERT_EQ(cpu.A, 0b00010000); // Bit 4 should now be set to 1

    // Case 2: SET 7, B (Set bit 7 in Register B)
    cpu.B = 0b01111111;          // Initialize B with all bits set except bit 7
    bus.write(cpu.PC, 0xCB);     // Prefix for BIT, SET, and RES instructions
    bus.write(cpu.PC + 1, 0xF8); // 0xF8 = SET 7, B (b=7, r=B)

    cpu.execute(bus.read(cpu.PC));

    // Verify the result
    ASSERT_EQ(cpu.B, 0b11111111); // Bit 7 should now be set to 1

    // Case 3: SET 0, D (Set bit 0 in Register D)
    cpu.D = 0b11111110;          // Initialize D with all bits set except bit 0
    bus.write(cpu.PC, 0xCB);     // Prefix for BIT, SET, and RES instructions
    bus.write(cpu.PC + 1, 0xC2); // 0xC2 = SET 0, D (b=0, r=D)

    cpu.execute(bus.read(cpu.PC));

    // Verify the result
    ASSERT_EQ(cpu.D, 0b11111111); // Bit 0 should now be set to 1
}
TEST_F(BitGroup, SET_b_HL)
{
    // Case 1: SET 4, (HL) (Set bit 4 in the memory location pointed by HL)
    cpu.H = 0x30;                     // High byte of HL = 0x30
    cpu.L = 0x00;                     // Low byte of HL = 0x00
    uint16_t hlAddress = 0x3000;      // Address stored in HL
    bus.write(hlAddress, 0b00000000); // Initialize memory at address 0x3000 with 0

    bus.write(cpu.PC, 0xCB);     // Prefix for BIT, SET, and RES instructions
    bus.write(cpu.PC + 1, 0xE6); // 0xE6 = SET 4, (HL) (b=4)

    cpu.execute(bus.read(cpu.PC));

    // Verify the result
    ASSERT_EQ(bus.read(hlAddress), 0b00010000); // Bit 4 should now be set to 1

    // Case 2: SET 7, (HL) (Set bit 7 in the memory location pointed by HL)
    bus.write(hlAddress, 0b01111111); // Initialize memory with all bits set except bit 7

    bus.write(cpu.PC, 0xCB);     // Prefix for BIT, SET, and RES instructions
    bus.write(cpu.PC + 1, 0xFE); // 0xFE = SET 7, (HL) (b=7)

    cpu.execute(bus.read(cpu.PC));

    // Verify the result
    ASSERT_EQ(bus.read(hlAddress), 0b11111111); // Bit 7 should now be set to 1

    // Case 3: SET 0, (HL) (Set bit 0 in the memory location pointed by HL)
    bus.write(hlAddress, 0b11111110); // Initialize memory with all bits set except bit 0

    bus.write(cpu.PC, 0xCB);     // Prefix for BIT, SET, and RES instructions
    bus.write(cpu.PC + 1, 0xC6); // 0xC6 = SET 0, (HL) (b=0)

    cpu.execute(bus.read(cpu.PC));

    // Verify the result
    ASSERT_EQ(bus.read(hlAddress), 0b11111111); // Bit 0 should now be set to 1
}
TEST_F(BitGroup, SET_b_IX_d)
{
    // Case 1: SET 0, (IX+3) (Set bit 0 in memory at IX + 3)
    cpu.IX = 0x2000;                      // Initialize IX to 0x2000
    int8_t displacement = 3;              // Displacement d = 3
    uint16_t targetAddress = 0x2003;      // Effective address = IX + 3
    bus.write(targetAddress, 0b11111110); // Initialize memory with all bits set except bit 0

    bus.write(cpu.PC, 0xDD);             // Prefix for IX instructions
    bus.write(cpu.PC + 1, 0xCB);         // Prefix for BIT, SET, RES instructions with IX+d
    bus.write(cpu.PC + 2, displacement); // Displacement value
    bus.write(cpu.PC + 3, 0xC6);         // 0xC6 = SET 0, (IX+d) (b=0)

    cpu.execute(bus.read(cpu.PC));

    // Verify the result
    ASSERT_EQ(bus.read(targetAddress), 0b11111111); // Bit 0 should now be set to 1

    // Case 2: SET 4, (IX+5) (Set bit 4 in memory at IX + 5)
    displacement = 5;                     // Displacement d = 5
    targetAddress = 0x2005;               // Effective address = IX + 5
    bus.write(targetAddress, 0b00000000); // Initialize memory with all bits cleared

    bus.write(cpu.PC, 0xDD);             // Prefix for IX instructions
    bus.write(cpu.PC + 1, 0xCB);         // Prefix for BIT, SET, RES instructions with IX+d
    bus.write(cpu.PC + 2, displacement); // Displacement value
    bus.write(cpu.PC + 3, 0xE6);         // 0xE6 = SET 4, (IX+d) (b=4)

    cpu.execute(bus.read(cpu.PC));

    // Verify the result
    ASSERT_EQ(bus.read(targetAddress), 0b00010000); // Bit 4 should now be set to 1

    // Case 3: SET 7, (IX-2) (Set bit 7 in memory at IX - 2)
    displacement = -2;                    // Negative displacement d = -2
    targetAddress = 0x1FFE;               // Effective address = IX - 2
    bus.write(targetAddress, 0b01111111); // Initialize memory with all bits set except bit 7

    bus.write(cpu.PC, 0xDD);             // Prefix for IX instructions
    bus.write(cpu.PC + 1, 0xCB);         // Prefix for BIT, SET, RES instructions with IX+d
    bus.write(cpu.PC + 2, displacement); // Displacement value
    bus.write(cpu.PC + 3, 0xFE);         // 0xFE = SET 7, (IX+d) (b=7)

    cpu.execute(bus.read(cpu.PC));

    // Verify the result
    ASSERT_EQ(bus.read(targetAddress), 0b11111111); // Bit 7 should now be set to 1
}
TEST_F(BitGroup, SET_b_IY_d)
{
    // Case 1: SET 0, (IX+3) (Set bit 0 in memory at IX + 3)
    cpu.IY = 0x2000;                      // Initialize IX to 0x2000
    int8_t displacement = 3;              // Displacement d = 3
    uint16_t targetAddress = 0x2003;      // Effective address = IX + 3
    bus.write(targetAddress, 0b11111110); // Initialize memory with all bits set except bit 0

    bus.write(cpu.PC, 0xFD);             // Prefix for IX instructions
    bus.write(cpu.PC + 1, 0xCB);         // Prefix for BIT, SET, RES instructions with IX+d
    bus.write(cpu.PC + 2, displacement); // Displacement value
    bus.write(cpu.PC + 3, 0xC6);         // 0xC6 = SET 0, (IX+d) (b=0)

    cpu.execute(bus.read(cpu.PC));

    // Verify the result
    ASSERT_EQ(bus.read(targetAddress), 0b11111111); // Bit 0 should now be set to 1

    // Case 2: SET 4, (IX+5) (Set bit 4 in memory at IX + 5)
    displacement = 5;                     // Displacement d = 5
    targetAddress = 0x2005;               // Effective address = IX + 5
    bus.write(targetAddress, 0b00000000); // Initialize memory with all bits cleared

    bus.write(cpu.PC, 0xFD);             // Prefix for IX instructions
    bus.write(cpu.PC + 1, 0xCB);         // Prefix for BIT, SET, RES instructions with IX+d
    bus.write(cpu.PC + 2, displacement); // Displacement value
    bus.write(cpu.PC + 3, 0xE6);         // 0xE6 = SET 4, (IX+d) (b=4)

    cpu.execute(bus.read(cpu.PC));

    // Verify the result
    ASSERT_EQ(bus.read(targetAddress), 0b00010000); // Bit 4 should now be set to 1

    // Case 3: SET 7, (IX-2) (Set bit 7 in memory at IX - 2)
    displacement = -2;                    // Negative displacement d = -2
    targetAddress = 0x1FFE;               // Effective address = IX - 2
    bus.write(targetAddress, 0b01111111); // Initialize memory with all bits set except bit 7

    bus.write(cpu.PC, 0xFD);             // Prefix for IX instructions
    bus.write(cpu.PC + 1, 0xCB);         // Prefix for BIT, SET, RES instructions with IX+d
    bus.write(cpu.PC + 2, displacement); // Displacement value
    bus.write(cpu.PC + 3, 0xFE);         // 0xFE = SET 7, (IX+d) (b=7)

    cpu.execute(bus.read(cpu.PC));

    // Verify the result
    ASSERT_EQ(bus.read(targetAddress), 0b11111111); // Bit 7 should now be set to 1
}
TEST_F(BitGroup, RES_b_r)
{
    // Case 1: RES 0, D (Reset bit 0 in register D)
    cpu.D = 0b11111111; // All bits initially set

    bus.write(cpu.PC, 0xCB);     // Prefix for bit operations
    bus.write(cpu.PC + 1, 0x82); // 0x82 = RES 0, D (b = 0, r = D)

    cpu.execute(bus.read(cpu.PC));

    // Verify that bit 0 in D is cleared
    ASSERT_EQ(cpu.D, 0b11111110); // Only bit 0 should be cleared

    // Case 2: RES 3, B (Reset bit 3 in register B)
    cpu.B = 0b11110111; // Bit 3 is already 0, the rest are 1s

    bus.write(cpu.PC, 0xCB);     // Prefix for bit operations
    bus.write(cpu.PC + 1, 0x98); // 0x98 = RES 3, B (b = 3, r = B)

    cpu.execute(bus.read(cpu.PC));

    // Verify that bit 3 in B is cleared
    ASSERT_EQ(cpu.B, 0b11110111); // Bit 3 remains cleared, unchanged result

    // Case 3: RES 7, A (Reset bit 7 in register A)
    cpu.A = 0b10000000; // Only bit 7 set

    bus.write(cpu.PC, 0xCB);     // Prefix for bit operations
    bus.write(cpu.PC + 1, 0xBF); // 0xBF = RES 7, A (b = 7, r = A)

    cpu.execute(bus.read(cpu.PC));

    // Verify that bit 7 in A is cleared
    ASSERT_EQ(cpu.A, 0b00000000); // All bits should now be cleared
}
TEST_F(BitGroup, RES_b_HL)
{
    // Case 1: RES 0, (HL) (Reset bit 0 in memory at address HL)
    cpu.H = 0x20; // Set HL to 0x2000
    cpu.L = 0x00;
    bus.write(0x2000, 0b11111111); // Memory at HL initially has all bits set

    bus.write(cpu.PC, 0xCB);     // Prefix for bit operations
    bus.write(cpu.PC + 1, 0x86); // 0x86 = RES 0, (HL) (b = 0, memory at HL)

    cpu.execute(bus.read(cpu.PC));

    // Verify that bit 0 in memory at HL is cleared
    ASSERT_EQ(bus.read(0x2000), 0b11111110);

    // Case 2: RES 4, (HL) (Reset bit 4 in memory at address HL)
    bus.write(0x2000, 0b11111111); // Reset memory to all bits set

    bus.write(cpu.PC, 0xCB);     // Prefix for bit operations
    bus.write(cpu.PC + 1, 0xA6); // 0xA6 = RES 4, (HL) (b = 4, memory at HL)

    cpu.execute(bus.read(cpu.PC));

    // Verify that bit 4 in memory at HL is cleared
    ASSERT_EQ(bus.read(0x2000), 0b11101111);

    // Case 3: RES 7, (HL) (Reset bit 7 in memory at address HL)
    bus.write(0x2000, 0b10000000); // Memory at HL has only bit 7 set

    bus.write(cpu.PC, 0xCB);     // Prefix for bit operations
    bus.write(cpu.PC + 1, 0xBE); // 0xBE = RES 7, (HL) (b = 7, memory at HL)

    cpu.execute(bus.read(cpu.PC));

    // Verify that bit 7 in memory at HL is cleared
    ASSERT_EQ(bus.read(0x2000), 0b00000000);
}
TEST_F(BitGroup, RES_b_IX_d)
{
    // Case 1: RES 0, (IX + 3) (Reset bit 0 at memory location IX + 3)
    cpu.IX = 0x2000; // Set IX register to 0x2000
    int8_t displacement = 3;
    bus.write(cpu.IX + displacement, 0b11111111); // Memory at IX+3 initially has all bits set

    bus.write(cpu.PC, 0xDD);             // Prefix for IX operations
    bus.write(cpu.PC + 1, 0xCB);         // Secondary prefix for bit operations
    bus.write(cpu.PC + 2, displacement); // Displacement
    bus.write(cpu.PC + 3, 0x86);         // 0x86 = RES 0, (IX+d) (b = 0)

    cpu.execute(bus.read(cpu.PC));

    // Verify that bit 0 at IX + 3 is cleared
    ASSERT_EQ(bus.read(cpu.IX + displacement), 0b11111110);

    // Case 2: RES 4, (IX + 5) (Reset bit 4 at memory location IX + 5)
    displacement = 5;
    bus.write(cpu.IX + displacement, 0b11111111); // Reset memory to all bits set

    bus.write(cpu.PC, 0xDD);             // Prefix for IX operations
    bus.write(cpu.PC + 1, 0xCB);         // Secondary prefix for bit operations
    bus.write(cpu.PC + 2, displacement); // Displacement
    bus.write(cpu.PC + 3, 0xA6);         // 0xA6 = RES 4, (IX+d) (b = 4)

    cpu.execute(bus.read(cpu.PC));

    // Verify that bit 4 at IX + 5 is cleared
    ASSERT_EQ(bus.read(cpu.IX + displacement), 0b11101111);

    // Case 3: RES 7, (IX - 2) (Reset bit 7 at memory location IX - 2)
    displacement = -2;
    bus.write(cpu.IX + displacement, 0b10000000); // Memory at IX-2 has only bit 7 set

    bus.write(cpu.PC, 0xDD);             // Prefix for IX operations
    bus.write(cpu.PC + 1, 0xCB);         // Secondary prefix for bit operations
    bus.write(cpu.PC + 2, displacement); // Displacement
    bus.write(cpu.PC + 3, 0xBE);         // 0xBE = RES 7, (IX+d) (b = 7)

    cpu.execute(bus.read(cpu.PC));

    // Verify that bit 7 at IX - 2 is cleared
    ASSERT_EQ(bus.read(cpu.IX + displacement), 0b00000000);
}
TEST_F(BitGroup, RES_b_IY_d)
{
    // Case 1: RES 0, (IY + 3) (Reset bit 0 at memory location IY + 3)
    cpu.IY = 0x3000; // Set IY register to 0x3000
    int8_t displacement = 3;
    bus.write(cpu.IY + displacement, 0b11111111); // Memory at IY+3 initially has all bits set

    bus.write(cpu.PC, 0xFD);             // Prefix for IY operations
    bus.write(cpu.PC + 1, 0xCB);         // Secondary prefix for bit operations
    bus.write(cpu.PC + 2, displacement); // Displacement
    bus.write(cpu.PC + 3, 0x86);         // 0x86 = RES 0, (IY+d) (b = 0)

    cpu.execute(bus.read(cpu.PC));

    // Verify that bit 0 at IY + 3 is cleared
    ASSERT_EQ(bus.read(cpu.IY + displacement), 0b11111110);

    // Case 2: RES 4, (IY + 5) (Reset bit 4 at memory location IY + 5)
    displacement = 5;
    bus.write(cpu.IY + displacement, 0b11111111); // Reset memory to all bits set

    bus.write(cpu.PC, 0xFD);             // Prefix for IY operations
    bus.write(cpu.PC + 1, 0xCB);         // Secondary prefix for bit operations
    bus.write(cpu.PC + 2, displacement); // Displacement
    bus.write(cpu.PC + 3, 0xA6);         // 0xA6 = RES 4, (IY+d) (b = 4)

    cpu.execute(bus.read(cpu.PC));

    // Verify that bit 4 at IY + 5 is cleared
    ASSERT_EQ(bus.read(cpu.IY + displacement), 0b11101111);

    // Case 3: RES 7, (IY - 2) (Reset bit 7 at memory location IY - 2)
    displacement = -2;
    bus.write(cpu.IY + displacement, 0b10000000); // Memory at IY-2 has only bit 7 set

    bus.write(cpu.PC, 0xFD);             // Prefix for IY operations
    bus.write(cpu.PC + 1, 0xCB);         // Secondary prefix for bit operations
    bus.write(cpu.PC + 2, displacement); // Displacement
    bus.write(cpu.PC + 3, 0xBE);         // 0xBE = RES 7, (IY+d) (b = 7)

    cpu.execute(bus.read(cpu.PC));

    // Verify that bit 7 at IY - 2 is cleared
    ASSERT_EQ(bus.read(cpu.IY + displacement), 0b00000000);
}
