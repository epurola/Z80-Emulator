#include <gtest/gtest.h>
#include "../main.cpp"
#include "../Bus.cpp"
#include "../Memory.cpp"
#include "../Instruction.cpp"

class JumpGroup : public ::testing::Test
{
protected:
    Memory memory;
    Bus bus;
    z80 cpu;

    JumpGroup() : memory(0x10000), bus(memory), cpu()
    {
        cpu.reset(&bus);
    }
};

TEST_F(JumpGroup, JP_NN)
{

    uint16_t address = 0x1234;

    // Set PC to some initial value (e.g., 0x1000)
    cpu.PC = 0x1000;

    // Write the low-order and high-order bytes of the address to memory
    bus.write(cpu.PC, 0xC3);
    bus.write(cpu.PC + 1, 0x34); // Low byte of address 0x1234
    bus.write(cpu.PC + 2, 0x12); // High byte of address 0x1234

    // Execute the JP nn instruction (opcode 0xC3 for JP nn)
    cpu.execute(bus.read(cpu.PC)); // The first byte is the opcode

    // Verify that the PC is now set to 0x1234
    ASSERT_EQ(cpu.PC, 0x1234);
}
TEST_F(JumpGroup, JP_CC_TRUE)
{
    uint16_t targetAddress = 0x1234;

    // Set PC to some initial value (e.g., 0x1000)
    cpu.PC = 0x1000;

    // Condition: C flag is set (Carry flag = true)
    cpu.setFlag(z80::C_flag);

    // Write JP C, nn instruction (opcode 0xDA for JP C, nn) at the initial PC
    bus.write(cpu.PC, 0xDA);     // JP C, nn opcode
    bus.write(cpu.PC + 1, 0x34); // Low byte of address 0x1234
    bus.write(cpu.PC + 2, 0x12); // High byte of address 0x1234

    // Execute the JP C, nn instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify that the PC is now set to 0x1234 since the condition is true
    ASSERT_EQ(cpu.PC, targetAddress);

    uint16_t targetAddress2 = 0x1234;

    // Set PC to some initial value (e.g., 0x1000)
    cpu.PC = 0x1000;

    // Condition: C flag is set (Carry flag = true)
    cpu.setFlag(z80::C_flag);

    // Write JP C, nn instruction (opcode 0xDA for JP C, nn) at the initial PC
    bus.write(cpu.PC, 0xCA);     // JP C, nn opcode
    bus.write(cpu.PC + 1, 0x34); // Low byte of address 0x1234
    bus.write(cpu.PC + 2, 0x12); // High byte of address 0x1234

    // Execute the JP C, nn instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify that the PC is now set to 0x1234 since the condition is true
    ASSERT_EQ(cpu.PC, targetAddress);
}

TEST_F(JumpGroup, JP_CC_FALSE)
{
    uint16_t targetAddress = 0x1234;

    // Set PC to some initial value (e.g., 0x1000)
    cpu.PC = 0x1000;

    // Condition: C flag is clear (Carry flag = false)
    cpu.clearFlag(z80::C_flag);

    // Write JP C, nn instruction (opcode 0xDA for JP C, nn) at the initial PC
    bus.write(cpu.PC, 0xDA);     // JP C, nn opcode
    bus.write(cpu.PC + 1, 0x34); // Low byte of address 0x1234
    bus.write(cpu.PC + 2, 0x12); // High byte of address 0x1234

    // Execute the JP C, nn instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify that the PC increments to the next instruction (0x1003) since the condition is false
    ASSERT_EQ(cpu.PC, 0x1003);

    uint16_t targetAddress2 = 0x1234;

    // Set PC to some initial value (e.g., 0x1000)
    cpu.PC = 0x1000;

    // Condition: C flag is clear (Carry flag = false)
    cpu.clearFlag(z80::C_flag);

    // Write JP C, nn instruction (opcode 0xDA for JP C, nn) at the initial PC
    bus.write(cpu.PC, 0xC2);     // JP C, nn opcode
    bus.write(cpu.PC + 1, 0x34); // Low byte of address 0x1234
    bus.write(cpu.PC + 2, 0x12); // High byte of address 0x1234

    // Execute the JP C, nn instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify that the PC increments to the next instruction (0x1003) since the condition is false
    ASSERT_EQ(cpu.PC, 0x1003);

    uint16_t targetAddress3 = 0x1234;

    // Set PC to some initial value (e.g., 0x1000)
    cpu.PC = 0x1000;

    // Condition: C flag is clear (Carry flag = false)
    cpu.clearFlag(z80::C_flag);

    // Write JP C, nn instruction (opcode 0xDA for JP C, nn) at the initial PC
    bus.write(cpu.PC, 0xF2);     // JP C, nn opcode
    bus.write(cpu.PC + 1, 0x34); // Low byte of address 0x1234
    bus.write(cpu.PC + 2, 0x12); // High byte of address 0x1234

    // Execute the JP C, nn instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify that the PC increments to the next instruction (0x1003) since the condition is false
    ASSERT_EQ(cpu.PC, 0x1003);
}

TEST_F(JumpGroup, JR_e)
{
    // Initialize a displacement (e) of +5 (relative forward jump)
    int8_t displacement = 0x03; // JR $+5

    // Set the initial Program Counter (PC)
    cpu.PC = 0x0480;

    // Write the JR e opcode and the displacement to memory
    bus.write(cpu.PC, 0x18);             // Opcode for JR e
    bus.write(cpu.PC + 1, displacement); // Displacement value

    // Execute the JR e instruction
    cpu.execute(bus.read(cpu.PC));

    // 0x0482 (PC after opcode+operand) + 5
    ASSERT_EQ(cpu.PC, 0x0485);
}

TEST_F(JumpGroup, JR_C_e)
{

    int8_t displacement = 0x04;

    // Set the initial Program Counter (PC)
    cpu.PC = 0x0081;

    // Write the JR C, e opcode and the displacement to memory
    bus.write(cpu.PC, 0x38);             // Opcode for JR C, e
    bus.write(cpu.PC + 1, displacement); // Displacement value

    // Set the Carry Flag (C flag) to 1
    cpu.setFlag(z80::C_flag);

    // Execute the JR C, e instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify that the PC is updated correctly (jump taken)
    uint16_t expectedPC = 0x0482 + displacement; // 0x0482 (PC after opcode+operand) - 4
    ASSERT_EQ(cpu.PC, 0x0087);
}

TEST_F(JumpGroup, JR_C_e_NoJump)
{
    // Initialize a displacement (e) of +5 (relative forward jump)
    int8_t displacement = 5; // JR C, $+5

    // Set the initial Program Counter (PC)
    cpu.PC = 0x0480;

    // Write the JR C, e opcode and the displacement to memory
    bus.write(cpu.PC, 0x38);             // Opcode for JR C, e
    bus.write(cpu.PC + 1, displacement); // Displacement value

    // Clear the Carry Flag (C flag = 0)
    cpu.clearFlag(z80::C_flag);

    // Execute the JR C, e instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify that the PC is incremented by 2 (jump not taken)
    uint16_t expectedPC = 0x0482; // No jump, PC = PC + 2
    ASSERT_EQ(cpu.PC, expectedPC);
}
TEST_F(JumpGroup, JR_NC_e)
{

    int8_t displacement = 0x04;

    // Set the initial Program Counter (PC)
    cpu.PC = 0x0081;

    // Write the JR C, e opcode and the displacement to memory
    bus.write(cpu.PC, 0x30);             // Opcode for JR C, e
    bus.write(cpu.PC + 1, displacement); // Displacement value

    // Set the Carry Flag (C flag) to 1
    cpu.clearFlag(z80::C_flag);

    // Execute the JR C, e instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify that the PC is updated correctly (jump taken)
    uint16_t expectedPC = 0x0482 + displacement; // 0x0482 (PC after opcode+operand) - 4
    ASSERT_EQ(cpu.PC, 0x0087);
}
TEST_F(JumpGroup, JR_Z_e)
{

    int8_t displacement = 0x04;

    // Set the initial Program Counter (PC)
    cpu.PC = 0x0081;

    // Write the JR C, e opcode and the displacement to memory
    bus.write(cpu.PC, 0x28);             // Opcode for JR C, e
    bus.write(cpu.PC + 1, displacement); // Displacement value

    // Set the Carry Flag (C flag) to 1
    cpu.setFlag(z80::Z);

    // Execute the JR C, e instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify that the PC is updated correctly (jump taken)
    uint16_t expectedPC = 0x0482 + displacement; // 0x0482 (PC after opcode+operand) - 4
    ASSERT_EQ(cpu.PC, 0x0087);
}
TEST_F(JumpGroup, JR_NZ_e)
{

    int8_t displacement = 0x04;

    // Set the initial Program Counter (PC)
    cpu.PC = 0x0081;

    // Write the JR C, e opcode and the displacement to memory
    bus.write(cpu.PC, 0x20);             // Opcode for JR C, e
    bus.write(cpu.PC + 1, displacement); // Displacement value

    // Set the Carry Flag (C flag) to 1
    cpu.clearFlag(z80::Z);

    // Execute the JR C, e instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify that the PC is updated correctly (jump taken)
    uint16_t expectedPC = 0x0482 + displacement; // 0x0482 (PC after opcode+operand) - 4
    ASSERT_EQ(cpu.PC, 0x0087);
}

TEST_F(JumpGroup, JP_HL)
{
    // Initialize the HL register with the target address
    uint16_t targetAddress = 0x4800;
    cpu.H = (targetAddress >> 8) & 0xFF; // High byte
    cpu.L = targetAddress & 0xFF;        // Low byte

    // Set the initial Program Counter (PC)
    cpu.PC = 0x1000;

    // Write the JP (HL) opcode to the current PC location in memory
    bus.write(cpu.PC, 0xE9); // Opcode for JP (HL)

    // Execute the JP (HL) instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify that the PC is updated to the contents of HL
    ASSERT_EQ(cpu.PC, targetAddress);
}
TEST_F(JumpGroup, JP_IX)
{
    // Initialize the IX register with the target address
    uint16_t targetAddress = 0x3456;
    cpu.IX = targetAddress;

    // Set the initial Program Counter (PC)
    cpu.PC = 0x1000;

    // Write the JP (IX) opcode to the current PC location in memory
    bus.write(cpu.PC, 0xDD);     // Prefix for IX instructions
    bus.write(cpu.PC + 1, 0xE9); // Opcode for JP (IX)

    // Execute the JP (IX) instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify that the PC is updated to the contents of IX
    ASSERT_EQ(cpu.PC, targetAddress);
}
TEST_F(JumpGroup, JP_IY)
{
    // Initialize the IY register with the target address
    uint16_t targetAddress = 0x789A;
    cpu.IY = targetAddress;

    // Set the initial Program Counter (PC)
    cpu.PC = 0x1000;

    // Write the JP (IY) opcode to the current PC location in memory
    bus.write(cpu.PC, 0xFD);     // Prefix for IY instructions
    bus.write(cpu.PC + 1, 0xE9); // Opcode for JP (IY)

    // Execute the JP (IY) instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify that the PC is updated to the contents of IY
    ASSERT_EQ(cpu.PC, targetAddress);
}
TEST_F(JumpGroup, DJNZ_e)
{
    // Initialize the B register with a non-zero value
    cpu.B = 0x0;

    // Set the initial Program Counter (PC)
    cpu.PC = 0x0080;

    // Write the DJNZ e instruction and displacement value to memory
    bus.write(cpu.PC, 0x10); // Opcode for DJNZ
    bus.write(cpu.PC + 1, 0xFA);

    // Execute the instruction
    for (int i = 0; i < 10; i++)
    {
        cpu.execute(bus.read(cpu.PC));
    }

    // Verify that PC is updated to PC + e (0x1000 - 2 = 0x0FFE) since B != 0
    ASSERT_EQ(cpu.PC, 0x07C);
}

TEST_F(JumpGroup, DJNZ_NoJumpWhenBIsZero)
{
    // Initialize the B register to 1
    cpu.B = 1;

    // Set the initial Program Counter (PC)
    cpu.PC = 0x1000;

    // Write the DJNZ e instruction and displacement value to memory
    bus.write(cpu.PC, 0x10);     // Opcode for DJNZ
    bus.write(cpu.PC + 1, 0xFE); // Displacement -2 (two's complement)

    // Execute the instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify that B is decremented to 0
    ASSERT_EQ(cpu.B, 0);

    // Verify that PC moves to the next sequential instruction (0x1002) since B == 0
    ASSERT_EQ(cpu.PC, 0x1002);
}
TEST_F(JumpGroup, CALL_nn)
{
    // Set up the initial conditions
    uint16_t address = 0x2135; // The target address for the CALL instruction
    cpu.PC = 0x0080;           // Initial Program Counter value
    cpu.SP = 0x2000;           // Initial Stack Pointer value

    // Write the CALL instruction (opcode 0xCD) and the address (0x2135) to memory
    bus.write(cpu.PC, 0xCD);     // Opcode for CALL
    bus.write(cpu.PC + 1, 0x35); // Low byte of address 0x2135
    bus.write(cpu.PC + 2, 0x21); // High byte of address 0x2135

    // Execute the CALL instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify the stack pointer (SP) has been updated correctly
    ASSERT_EQ(cpu.SP, 0x1FFE); // SP should have been decremented by 2

    ASSERT_EQ(cpu.PC, address);

    // Verify that the high byte of the PC was pushed onto the stack at address SP
    ASSERT_EQ(bus.read(cpu.SP + 1), 0x00); // High byte of the original PC

    // Verify that the low byte of the PC was pushed onto the stack at address SP + 1
    ASSERT_EQ(bus.read(cpu.SP), 0x83); // Low byte of the original PC

    // Verify the Program Counter (PC) has been set to the new address (0x2135)
}
TEST_F(JumpGroup, CALL_CC_NN)
{
    // Set up the initial conditions
    uint16_t address = 0x2135; // The target address for the CALL instruction
    cpu.PC = 0x0000;           // Initial Program Counter value
    cpu.SP = 0x5698;           // Initial Stack Pointer value
    cpu.setAF(0x004e);
    // Write the CALL instruction (opcode 0xCD) and the address (0x2135) to memory
    bus.write(cpu.PC, 0xC4);     // Opcode for CALL
    bus.write(cpu.PC + 1, 0x61); // Low byte of address 0x2135
    bus.write(cpu.PC + 2, 0x9c); // High byte of address 0x2135

    // Execute the CALL instruction
    cpu.execute(bus.read(cpu.PC));

    ASSERT_EQ(cpu.getAF(), 0x004e);
    // Verify the stack pointer (SP) has been updated correctly
    ASSERT_EQ(cpu.SP, 0x5698); // SP should have been decremented by 2

    ASSERT_EQ(cpu.PC, 0x0003);
}
TEST_F(JumpGroup, CALL_CC_NN_notzero)
{

    cpu.PC = 0x0080; // Initial Program Counter value
    cpu.SP = 0x2000; // Initial Stack Pointer value

    // Write the CALL instruction (opcode 0xCD) and the address (0x2135) to memory
    bus.write(cpu.PC, 0xCC);     // Opcode for CALL
    bus.write(cpu.PC + 1, 0x90); // Low byte of address 0x2135
    bus.write(cpu.PC + 2, 0x01); // High byte of address 0x2135

    cpu.clearFlag(z80::Z);

    // Execute the CALL instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify the stack pointer (SP) has been updated correctly
    ASSERT_EQ(cpu.PC, 0x0083); // SP should have been decremented by 2

    ASSERT_EQ(cpu.SP, 0x2000);
}
TEST_F(JumpGroup, RET)
{
    // Set up the initial conditions
    cpu.SP = 0x1FFE; // Initial Stack Pointer value
    cpu.PC = 0x0080; // Some arbitrary PC value (not relevant for RET start)

    // Write return address to the stack (low byte first, then high byte)
    bus.write(cpu.SP, 0x90);     // Low byte of return address
    bus.write(cpu.SP + 1, 0x01); // High byte of return address

    // Write the RET opcode to memory at the current PC
    bus.write(cpu.PC, 0xC9); // Opcode for RET

    // Execute the RET instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify the Stack Pointer (SP) has been incremented correctly
    ASSERT_EQ(cpu.SP, 0x2000); // SP should have been incremented by 2

    // Verify the Program Counter (PC) has been set to the address popped from the stack
    ASSERT_EQ(cpu.PC, 0x0190); // PC should now contain the address 0x18B5
}
TEST_F(JumpGroup, RET_cc)
{
    // Set up the initial conditions
    cpu.SP = 0x1FFE;          // Initial Stack Pointer value
    cpu.PC = 0x0080;          // Some arbitrary PC value
    cpu.setFlag(z80::C_flag); // Set the S flag (Sign Negative condition)

    // Write return address to the stack (low byte first, then high byte)
    bus.write(cpu.SP, 0x90);     // Low byte of return address
    bus.write(cpu.SP + 1, 0x01); // High byte of return address

    // Write the RET M opcode to memory at the current PC (RET M: 0xF8)
    bus.write(cpu.PC, 0xD8);

    // Execute the RET M instruction (condition "M" checks if the S flag is set)
    cpu.execute(bus.read(cpu.PC));

    // Verify the Stack Pointer (SP) has been incremented correctly
    ASSERT_EQ(cpu.SP, 0x2000); // SP should have been incremented by 2

    // Verify the Program Counter (PC) has been set to the address popped from the stack
    ASSERT_EQ(cpu.PC, 0x0190); // PC should now contain the address 0x18B5

    // Verify behavior when condition is false
    cpu.clearFlag(z80::S); // Clear the S flag
    cpu.SP = 0x2000;       // Reset SP
    cpu.PC = 0x1234;       // Reset PC
    cpu.execute(bus.read(cpu.PC));

    // Verify that SP and PC remain unchanged when the condition is false
    ASSERT_EQ(cpu.SP, 0x2000); // SP should remain the same
    ASSERT_EQ(cpu.PC, 0x1235); // PC should increment to the next instruction
}

class InputOutputGroupTest : public ::testing::Test
{
protected:
    Memory memory;
    Bus bus;
    z80 cpu;

    InputOutputGroupTest() : memory(0x10000), bus(memory), cpu()
    {
        cpu.reset(&bus);
    }
};

TEST_F(InputOutputGroupTest, IN_A_n)
{
    // Set up initial conditions
    cpu.A = 0x23;             // Set the accumulator to an initial value
    uint8_t port = 0x01;      // The I/O port to read from
    uint8_t portValue = 0x7B; // The value available at the peripheral device

    // Mock the I/O device to return a specific value when the port is accessed
    bus.write(port, portValue);

    // Load the instruction IN A, (n) into memory
    bus.write(cpu.PC, 0xDB);     // Opcode for IN A, (n)
    bus.write(cpu.PC + 1, port); // Port number

    // Execute the instruction
    cpu.execute(bus.read(cpu.PC));

    // Verify the accumulator contains the value read from the port
    ASSERT_EQ(cpu.A, portValue);

    // Verify that the program counter (PC) has been incremented correctly
    ASSERT_EQ(cpu.PC, 0x0002); // PC should have advanced by 2 bytes (opcode + operand)
}
