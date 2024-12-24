// main.h
#ifndef MAIN_H
#define MAIN_H

#include <cstdint>
#include <map>
#include <functional>
#include <iomanip>
#include "Bus.hpp"
class Instruction;
enum class InterruptMode
{
  Mode0, // Interrupt Mode 0
  Mode1, // Interrupt Mode 1
  Mode2  // Interrupt Mode 2
};

class z80
{

public:
  static const int C_flag = 1 << 0; // Carry flag
  static const int N = 1 << 1;      // Add/Subtract flag
  static const int P = 1 << 2;      // Parity/Overflow flag
  static const int X = 1 << 3;      // Undocumented flag
  static const int H_flag = 1 << 4; // Half Carry flag
  static const int U = 1 << 5;      // Undocumented flag
  static const int Z = 1 << 6;      // Zero flag
  static const int S = 1 << 7;

  InterruptMode interruptMode = InterruptMode::Mode0;

  uint8_t A, B, C, D, E, H, L, F;
  uint8_t A1, B1, C1, D1, E1, H1, L1, F1;
  uint8_t I, R;
  uint16_t IX, IY;
  uint16_t SP, PC;

  uint16_t MPTR;

  bool IFF2;
  bool IFF1;
  bool halted;

  std::map<uint8_t, Instruction> instructionTable;
  std::map<uint8_t, Instruction> instructionTableDD;
  std::map<uint8_t, Instruction> instructionTableDDCB;
  std::map<uint8_t, Instruction> instructionTableFD;
  std::map<uint8_t, Instruction> instructionTableFDCB;
  std::map<uint8_t, Instruction> instructionTableED;
  std::map<uint8_t, Instruction> instructionTableCB;

  Bus *bus;

  z80();
  // Initialize registers and flags
  void reset(Bus *bus);

  // Read and Write functions
  uint8_t readFromRegister(uint8_t regIndex);
  uint16_t readFromRegisterPair(uint8_t regIndex);
  uint16_t readFromRegisterPair2(uint8_t regIndex);
  uint16_t readFromRegisterPair3(uint8_t regIndex);
  void writeToRegister(uint8_t regIndex, uint8_t value);
  void writeToRegisterPair(uint8_t reg, uint16_t value);

  void IncrementRefreshRegister(int steps);

  // Flag manipulation functions
  void setFlag(uint8_t flagMask);
  void clearFlag(uint8_t flagMask);
  void toggleFlag(uint8_t flagMask);
  bool isFlagSet(uint8_t flagMask) const;

  // Instruction helpers
  void initializeInstructionTable();

  // Execution of instructions
  void execute(uint8_t opCode);

  // Instruction implementations
  void LD_R_R(uint8_t opCode);
  void LD_R_N(uint8_t opCode);
  void LD_R_HL(uint8_t opCode);
  void LD_R_IX_D(uint8_t opCode);
  void LD_R_IY_D(uint8_t opCode);
  void LD_HL_R(uint8_t opCode);
  void LD_IX_D_R(uint8_t opCode);
  void LD_IY_D_R(uint8_t opCode);
  void LD_HL_N(uint8_t opCode);
  void LD_IX_D_N(uint8_t opCode);
  void LD_IY_D_N(uint8_t opCode);
  void LD_A_BC(uint8_t opCode);
  void LD_A_DE(uint8_t opCode);
  void LD_A_NN(uint8_t opCode);
  void LD_DE_A(uint8_t opCode);
  void LD_NN_A(uint8_t opCode);
  void LD_A_I(uint8_t opCode);
  void LD_A_R(uint8_t opCode);
  void LD_I_A(uint8_t opCode);
  void LD_R_A(uint8_t opCode);
  void LD_BC_A(uint8_t opCode);

  void LD_R_IXH(uint8_t opCode);
  void LD_IXH_R(uint8_t opCode);
  void LD_R_IXl(uint8_t opCode);
  void LD_IXH_IXL(uint8_t opCode);

  void LD_IXl_R(uint8_t opCode);
  void LD_R_IXh(uint8_t opCode);
  void LD_IXL_IXH(uint8_t opCode);

  void ADD_A_IX_H(uint8_t opCode);
  void ADD_A_IX_L(uint8_t opCode);

  void SUB_A_IX_H(uint8_t opCode);
  void SUB_A_IX_L(uint8_t opCode);

  void SUB_A_IY_H(uint8_t opCode);
  void SUB_A_IY_L(uint8_t opCode);

  void SBC_A_IX_H(uint8_t opCode);
  void SBC_A_IX_L(uint8_t opCode);

  void SBC_A_IY_H(uint8_t opCode);
  void SBC_A_IY_L(uint8_t opCode);

  void ADC_A_IX_H(uint8_t opCode);
  void ADC_A_IX_L(uint8_t opCode);

  void ADC_A_IY_H(uint8_t opCode);
  void ADC_A_IY_L(uint8_t opCode);

  void AND_A_IX_H(uint8_t opCode);
  void AND_A_IX_L(uint8_t opCode);
  void XOR_A_IX_H(uint8_t opCode);
  void XOR_A_IX_L(uint8_t opCode);
  void OR_A_IX_H(uint8_t opCode);
  void OR_A_IX_L(uint8_t opCode);

  void AND_A_IY_H(uint8_t opCode);
  void AND_A_IY_L(uint8_t opCode);
  void XOR_A_IY_H(uint8_t opCode);
  void XOR_A_IY_L(uint8_t opCode);
  void OR_A_IY_H(uint8_t opCode);
  void OR_A_IY_L(uint8_t opCode);

  void CP_IX_H(uint8_t opCode);
  void CP_IX_L(uint8_t opCode);
  void SLS_IX_D(uint8_t opCode);

  void CP_A_IY_H(uint8_t opCode);
  void CP_A_IY_L(uint8_t opCode);
  void SLS_IY_D(uint8_t opCode);

  void INC_IY_H(uint8_t opCode);
  void INC_IY_L(uint8_t opCode);

  void DEC_IY_H(uint8_t opCode);
  void DEC_IY_L(uint8_t opCode);

  void LD_IY_H_N(uint8_t opCode);
  void LD_IY_L_N(uint8_t opCode);

  void LD_R_IY_H(uint8_t opCode);
  void LD_R_IY_L(uint8_t opCode);

  void LD_IY_H_R(uint8_t opCode);
  void LD_IY_L_R(uint8_t opCode);

  void LD_IYH_IYL(uint8_t opCode);
  void LD_IYL_IYH(uint8_t opCode);

  void ADD_A_IY_L(uint8_t opCode);
  void ADD_A_IY_H(uint8_t opCode);

  void LD_DD_NN(uint8_t opCode);
  void LD_IX_NN(uint8_t opCode);
  void LD_IXH_N(uint8_t opCode);
  void LD_IXl_N(uint8_t opCode);
  void LD_IY_NN(uint8_t opCode);
  void LD_HL_NN(uint8_t opCode);
  void LD_DD_nn(uint8_t opCode);
  void LD_IX_NN2(uint8_t opCode);
  void LD_IY_NN2(uint8_t opCode);
  void LD_NN_HL(uint8_t opCode);
  void LD_NN_DD(uint8_t opCode);
  void LD_NN_IX(uint8_t opCode);
  void LD_NN_IY(uint8_t opCode);
  void LD_SP_HL(uint8_t opCode);
  void LD_SP_IX(uint8_t opCode);
  void LD_SP_IY(uint8_t opCode);
  void PUSH_QQ(uint8_t opCode);
  void PUSH_IX(uint8_t opCode);
  void PUSH_IY(uint8_t opCode);
  void POP_QQ(uint8_t opCode);
  void POP_IX(uint8_t opCode);
  void POP_IY(uint8_t opCode);

  void EX_DE_HL(uint8_t opCode);
  void EX_AF_AF1(uint8_t opCode);
  void EX_SP_HL(uint8_t opCode);
  void EX_SP_IX(uint8_t opCode);
  void EX_SP_IY(uint8_t opCode);
  void LDI(uint8_t opCode);
  void EXX(uint8_t opCode);
  void LDIR(uint8_t opCode);
  void LDD(uint8_t opCode);
  void LDDR(uint8_t opCode);
  void CPI(uint8_t opCode);
  void CPIR(uint8_t opCode);
  void CPD(uint8_t opCode);
  void CPDR(uint8_t opCode);

  void ADD_A_R(uint8_t opCode);
  void ADD_A_N(uint8_t opCode);
  void ADD_A_HL(uint8_t opCode);
  void ADD_A_IX_D(uint8_t opCode);
  void ADD_A_IY_D(uint8_t opCode);
  void ADC_A_s(uint8_t opCode);
  void ADC_A_N(uint8_t opCode);
  void ADC_A_HL(uint8_t opCode);
  void ADC_A_IX_D(uint8_t opCode);
  void ADC_A_IY_D(uint8_t opCode);
  void SUB_S(uint8_t opCode);
  void SUB_A_N(uint8_t opCode);
  void SUB_A_HL(uint8_t opCode);
  void SUB_A_IX_D(uint8_t opCode);
  void SUB_A_IY_D(uint8_t opCode);
  void SUB_A_S(uint8_t opCode);
  void SUB_A_R(uint8_t opCode);
  void SBC_A_S(uint8_t opCode);
  void SBC_A_N(uint8_t opCode);
  void SBC_A_HL(uint8_t opCode);
  void SBC_A_IX_D(uint8_t opCode);
  void SBC_A_IY_D(uint8_t opCode);
  void AND_S(uint8_t opCode);
  void OR_S(uint8_t opCode);
  void XOR_S(uint8_t opCode);
  void CP_S(uint8_t opCode);
  void INC_R(uint8_t opCode);
  void INC_HL(uint8_t opCode);
  void INC_IX_D(uint8_t opCode);
  void INC_IY_D(uint8_t opCode);
  void DEC_M(uint8_t opCode);
  void AND_A_R(uint8_t opCode);
  void AND_A_N(uint8_t opCode);
  void AND_A_HL(uint8_t opCode);
  void AND_A_IX_D(uint8_t opCode);
  void AND_A_IY_D(uint8_t opCode);
  void OR_A_R(uint8_t opCode);
  void OR_A_N(uint8_t opCode);
  void OR_A_HL(uint8_t opCode);
  void OR_A_IX_D(uint8_t opCode);
  void OR_A_IY_D(uint8_t opCode);
  void XOR_A_R(uint8_t opCode);
  void XOR_A_N(uint8_t opCode);
  void XOR_A_HL(uint8_t opCode);
  void XOR_A_IX_D(uint8_t opCode);
  void XOR_A_IY_D(uint8_t opCode);

  void CP_R(uint8_t opCode);
  void CP_N(uint8_t opCode);
  void CP_HL(uint8_t opCode);
  void CP_IX_D(uint8_t opCode);
  void CP_IY_D(uint8_t opCode);

  void DEC_R(uint8_t opCode);
  void DEC_HL(uint8_t opCode);
  void DEC_IX_D(uint8_t opCode);
  void DEC_IY_D(uint8_t opCode);

  void DAA(uint8_t opCode);
  void CPL(uint8_t opCode);
  void NEG(uint8_t opCode);
  void CCF(uint8_t opCode);
  void SCF(uint8_t opCode);
  void NOP(uint8_t opCode);
  void HALT(uint8_t opCode);
  void DI(uint8_t opCode);
  void EI(uint8_t opCode);
  void IM0(uint8_t opCode);
  void IM1(uint8_t opCode);
  void IM2(uint8_t opCode);

  void ADD_HL_SS(uint8_t opCode);
  void ADC_HL_SS(uint8_t opCode);
  void SBC_HL_SS(uint8_t opCode);
  void ADD_IX_PP(uint8_t opCode);
  void ADD_IY_RR(uint8_t opCode);
  void INC_SS(uint8_t opCode);
  void INC_IX(uint8_t opCode);
  void INC_IXH(uint8_t opCode);
  void INC_IXl(uint8_t opCode);
  void DEC_IXH(uint8_t opCode);
  void DEC_IXl(uint8_t opCode);
  void INC_IY(uint8_t opCode);
  void INC_IYH(uint8_t opCode);
  void DEC_IYH(uint8_t opCode);
  void DEC_SS(uint8_t opCode);
  void DEC_IX(uint8_t opCode);
  void DEC_IY(uint8_t opCode);

  void RLCA(uint8_t opCode);
  void RLA(uint8_t opCode);
  void RRCA(uint8_t opCode);
  void RRA(uint8_t opCode);
  void RLC_R(uint8_t opCode);
  void RLC_HL(uint8_t opCode);
  void RLC_IX_D(uint8_t opCode);
  void RLC_IY_D(uint8_t opCode);
  void RL_R(uint8_t opCode);
  void RL_HL(uint8_t opCode);
  void RL_IX_D(uint8_t opCode);
  void RL_IY_D(uint8_t opCode);
  void RRC_R(uint8_t opCode);
  void RRC_HL(uint8_t opCode);
  void RRC_IX_D(uint8_t opCode);
  void RRC_IY_D(uint8_t opCode);
  void RR_R(uint8_t opCode);
  void RR_HL(uint8_t opCode);
  void RR_IX_D(uint8_t opCode);
  void RR_IY_D(uint8_t opCode);
  void SLA_R(uint8_t opCode);
  void SLA_HL(uint8_t opCode);
  void SLA_IX_D(uint8_t opCode);
  void SLA_IY_D(uint8_t opCode);
  void SRA_R(uint8_t opCode);
  void SLS_R(uint8_t opCode);
  void SLS_HL(uint8_t opCode);
  void SRA_HL(uint8_t opCode);
  void SRA_IX_D(uint8_t opCode);
  void SRA_IY_D(uint8_t opCode);
  void SRL_R(uint8_t opCode);
  void SRL_HL(uint8_t opCode);
  void SRL_IX_D(uint8_t opCode);
  void SRL_IY_D(uint8_t opCode);
  void RLD(uint8_t opCode);
  void RRD(uint8_t opCode);

  void BIT_B_R(uint8_t opCode);
  void BIT_B_HL(uint8_t opCode);
  void BIT_B_IX_D(uint8_t opCode);
  void BIT_B_IY_D(uint8_t opCode);
  void SET_B_R(uint8_t opCode);
  void SET_B_HL(uint8_t opCode);
  void SET_B_IX_D(uint8_t opCode);
  void SET_B_IY_D(uint8_t opCode);
  void RES_B_R(uint8_t opCode);
  void RES_B_HL(uint8_t opCode);
  void RES_B_IX_D(uint8_t opCode);
  void RES_B_IY_D(uint8_t opCode);

  void JP_NN(uint8_t opCode);
  void JP_CC_NN(uint8_t opCode);
  void JR_E(uint8_t opCode);
  void JR_C_E(uint8_t opCode);
  void JR_NC_E(uint8_t opCode);
  void JR_Z_E(uint8_t opCode);
  void JR_NZ_E(uint8_t opCode);
  void JP_HL(uint8_t opCode);
  void JP_IX(uint8_t opCode);
  void JP_IY(uint8_t opCode);
  void DJNZ_E(uint8_t opCode);

  void CALL_NN(uint8_t opCode);
  void CALL_CC_NN(uint8_t opCode);
  void RET(uint8_t opCode);
  void RET_CC(uint8_t opCode);
  void RETI(uint8_t opCode);
  void RETN(uint8_t opCode);
  void RST_P(uint8_t opCode);

  void IN_A_N(uint8_t opCode);
  void IN_R_C(uint8_t opCode);
  void INI(uint8_t opCode);
  void INIR(uint8_t opCode);
  void IND(uint8_t opCode);
  void INDR(uint8_t opCode);
  void OUT_N_A(uint8_t opCode);
  void OUT_C_R(uint8_t opCode);
  void OUTI(uint8_t opCode);
  void OTIR(uint8_t opCode);
  void OUTD(uint8_t opCode);
  void OTDR(uint8_t opCode);

  // Helper to fetch immediate data
  uint8_t fetchImmediate();
  uint8_t fetchHL();
  uint8_t fetchIX();
  uint16_t getBC();
  uint16_t getDE();
  uint16_t getHL();
  uint16_t getAF();
  uint16_t getAF1();
  uint16_t getBC1();
  uint16_t getDE1();
  uint16_t getHL1();

  void setBC(uint16_t value);
  void setDE(uint16_t value);
  void setHL(uint16_t value);
  void setAF(uint16_t value);

  void setBC1(uint16_t value);
  void setDE1(uint16_t value);
  void setHL1(uint16_t value);
  void setAF1(uint16_t value);

  uint8_t Add8_Bit(uint8_t a, uint8_t b);
  uint8_t Sub8_Bit(uint8_t a, uint8_t b);
  uint8_t And8_Bit(uint8_t a, uint8_t b);
  uint8_t Or8_Bit(uint8_t a, uint8_t b);
  uint8_t Xor8_Bit(uint8_t a, uint8_t b);
  uint8_t CP_Flags(uint8_t a, uint8_t b);
  void IncFlags(uint8_t value, uint8_t result);
  void DecFlags(uint8_t value, uint8_t result);
  void rotateFlags(uint8_t value);
  void shiftFlags(uint8_t value);
  void shiftFlags2(uint8_t value);
  void pushPC();
  void popPC();

  uint16_t Add16_Bit(uint16_t a, uint16_t b);
  uint16_t Add16_BitCarry(uint16_t a, uint16_t b);
  uint16_t Sub16_Bit(uint16_t a, uint16_t b);
  uint16_t getPageZeroAddress(uint8_t value);

  bool isEvenParity(uint8_t result);
  bool evaluateCC(uint8_t result);
};

#endif
