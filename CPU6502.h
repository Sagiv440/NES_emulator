#pragma once
#include <iostream>
#include "BUS.h"

class CPU6502
{
public:

	enum flags {
		Negative = 0x80,
		Overflow = 0x40,
		Break = 0x10,
		Decimal = 0x08,
		Interrupt = 0x04,
		Zero = 0x02,
		Carry = 0x01,
	};

	enum contrl_lines {
		R_W = 0x80,
		IRQ = 0x40,
		NMI = 0x20,
		SYNC = 0x10,
	};

	
	uint8_t Opcode_id = 0x00;
	//CPU main ragistar
	uint8_t A_reg;
	uint8_t X_reg;
	uint8_t Y_reg;
	uint16_t PC;
	uint8_t SP;
	uint8_t SR;

	BUS* bus;

	//Asisting verables
	uint8_t low;
	uint8_t high;
	uint16_t sum;
	uint16_t address;
	uint16_t data_point;

	//6502 Addressing Modes
	uint8_t Imp();  //Implicit
	uint8_t Acc();	//Accumulator
	uint8_t Imm();	//Immediate
	uint8_t Zpp();	//ZeroPage
	uint8_t Zpx();	//ZeroPage,X
	uint8_t Zpy();	//ZeroPage,Y
	uint8_t Rel();	//Relative
	uint8_t Abs();	//Absolute
	uint8_t Abx();	//Absolute,X
	uint8_t Aby();	//Absolute,Y
	uint8_t Ind();	//Indirect
	uint8_t Inx();	//Indexed_Indirect
	uint8_t Iny();	//Indirect_Indexed

	//6052 llegal Instructions
	uint8_t ADC();	uint8_t AND();
	uint8_t ASL();	uint8_t BCC();
	uint8_t BCS();	uint8_t BEQ();
	uint8_t BIT();	uint8_t BMI();
	uint8_t BNE();	uint8_t BPL();
	uint8_t BRK();	uint8_t BVC();
	uint8_t BVS();	uint8_t CLC();
	uint8_t CLD();	uint8_t CLI();
	uint8_t CLV();	uint8_t CMP();
	uint8_t CPX();	uint8_t CPY();
	uint8_t DEC();	uint8_t DEX();
	uint8_t DEY();	uint8_t EOR();
	uint8_t INC();	uint8_t INX();
	uint8_t INY();	uint8_t JMP();
	uint8_t JSR();	uint8_t LDA();
	uint8_t LDX();	uint8_t LDY();
	uint8_t LSR();	uint8_t NOP();
	uint8_t ORA();	uint8_t PHA();
	uint8_t PHP();	uint8_t PLA();
	uint8_t PLP();	uint8_t ROL();
	uint8_t ROR();	uint8_t RTI();
	uint8_t RTS();	uint8_t SBC();
	uint8_t SEC();	uint8_t SED();
	uint8_t SEI();	uint8_t STA();
	uint8_t STX();	uint8_t STY();
	uint8_t TAX();	uint8_t TAY();
	uint8_t TSX();	uint8_t TXA();
	uint8_t TXS();	uint8_t TYA();

	uint8_t XXX(); // illegal Instructions

	void save_data(uint16_t address, uint8_t data);

	uint8_t load_data(uint16_t address);
	uint8_t load_data(uint8_t low_address, uint8_t high_address);
	uint8_t load_data(uint8_t low_address, uint8_t high_address, uint8_t& reg_offset);
	uint8_t load_Opcode();
	uint8_t get_flag(uint8_t flag);
	uint8_t get_Control_pin(uint8_t flag);
	void set_flag(uint8_t flag, uint8_t state);
	void set_Control_pin(uint8_t flag, uint8_t state);
	void reset_flags_rag();
	void Overflow_flag(uint8_t &reg, uint8_t result);

	struct OPCODE
	{
		char name[4];
		uint8_t(CPU6502::*opcode)(void);
		uint8_t(CPU6502::*address)(void);
		uint8_t Cycles;
	};

	using a = CPU6502;
	OPCODE list[256] = {
		//					0							1							2							3							4					|			5				|			6								7							8								9							A							B							C								D							E							F
		/*0*/	{"BRK", &a::BRK, &a::Imp, 7},{"ORA", &a::ORA, &a::Inx, 6},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::Imp, 1},{"ORA", &a::ORA, &a::Zpp, 3},{"ASL", &a::ASL, &a::Zpp, 5},{"XXX", &a::XXX, &a::XXX, 1},{"PHP", &a::PHP, &a::Imp, 3},{"ORA", &a::ORA, &a::Imm, 2},{"ASL", &a::ASL, &a::Acc, 2},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"ORA", &a::ORA, &a::Abs, 4},{"ASL", &a::ASL, &a::Abs, 6},{"XXX", &a::XXX, &a::XXX, 1},
		/*1*/	{"BPL", &a::BPL, &a::Rel, 2},{"ORA", &a::ORA, &a::Iny, 5},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"ORA", &a::ORA, &a::Zpx, 4},{"ASL", &a::ASL, &a::Zpx, 6},{"XXX", &a::XXX, &a::XXX, 1},{"CLC", &a::CLC, &a::Imp, 2},{"ORA", &a::ORA, &a::Aby, 4},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"ORA", &a::ORA, &a::Abx, 4},{"ASL", &a::ASL, &a::Abx, 7},{"XXX", &a::XXX, &a::XXX, 1},
		/*2*/	{"JSR", &a::JSR, &a::Abs, 6},{"AND", &a::AND, &a::Inx, 6},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"BIT", &a::BIT, &a::Zpp, 3},{"AND", &a::AND, &a::Zpp, 3},{"ROL", &a::ROL, &a::Zpp, 5},{"XXX", &a::XXX, &a::XXX, 1},{"PLP", &a::PLP, &a::Imp, 4},{"AND", &a::AND, &a::Imm, 2},{"ROL", &a::ROL, &a::Acc, 2},{"XXX", &a::XXX, &a::XXX, 1},{"BIT", &a::BIT, &a::Abs, 4},{"AND", &a::AND, &a::Abs, 4},{"ROL", &a::ROL, &a::Abs, 6},{"XXX", &a::XXX, &a::XXX, 1},
		/*3*/	{"BMI", &a::BMI, &a::Rel, 2},{"AND", &a::AND, &a::Iny, 5},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"AND", &a::AND, &a::Zpp, 3},{"ROL", &a::ROL, &a::Zpp, 5},{"XXX", &a::XXX, &a::XXX, 1},{"SEC", &a::SEC, &a::Imp, 2},{"AND", &a::AND, &a::Aby, 4},{"ROL", &a::ROL, &a::Acc, 2},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"AND", &a::AND, &a::Abx, 4},{"ROL", &a::ROL, &a::Abx, 7},{"XXX", &a::XXX, &a::XXX, 1},
		/*4*/	{"RTI", &a::RTI, &a::Imp, 6},{"EOR", &a::EOR, &a::Inx, 6},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"EOR", &a::EOR, &a::Zpp, 3},{"LSR", &a::LSR, &a::Zpp, 5},{"XXX", &a::XXX, &a::XXX, 1},{"PHA", &a::SEC, &a::Imp, 3},{"EOR", &a::EOR, &a::Imm, 2},{"LSR", &a::LSR, &a::Acc, 2},{"XXX", &a::XXX, &a::XXX, 1},{"JMP", &a::JMP, &a::Abs, 3},{"EOR", &a::EOR, &a::Abs, 4},{"LSR", &a::LSR, &a::Abs, 6},{"XXX", &a::XXX, &a::XXX, 1},
		/*5*/	{"BVC", &a::BVC, &a::Rel, 2},{"EOR", &a::EOR, &a::Iny, 5},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"EOR", &a::EOR, &a::Zpx, 4},{"LSR", &a::LSR, &a::Zpx, 6},{"XXX", &a::XXX, &a::XXX, 1},{"CLI", &a::CLI, &a::Imp, 2},{"EOR", &a::EOR, &a::Aby, 4},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"EOR", &a::EOR, &a::Abx, 4},{"LSR", &a::LSR, &a::Abx, 7},{"XXX", &a::XXX, &a::XXX, 1},
		/*6*/	{"RTS", &a::RTS, &a::Imp, 6},{"ADC", &a::ADC, &a::Inx, 5},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"ADC", &a::ADC, &a::Zpp, 3},{"ROR", &a::ROR, &a::Zpp, 5},{"XXX", &a::XXX, &a::XXX, 1},{"PLA", &a::PLA, &a::Imp, 4},{"ADC", &a::ADC, &a::Imm, 2},{"ROR", &a::ROR, &a::Acc, 2},{"XXX", &a::XXX, &a::XXX, 1},{"JMP", &a::JMP, &a::Ind, 5},{"ADC", &a::ADC, &a::Abs, 4},{"ROR", &a::ROR, &a::Abs, 6},{"XXX", &a::XXX, &a::XXX, 1},
		/*7*/	{"BVS", &a::BVS, &a::Rel, 2},{"ADC", &a::ADC, &a::Iny, 5},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"ADC", &a::ADC, &a::Zpx, 4},{"ROR", &a::ROR, &a::Zpx, 6},{"XXX", &a::XXX, &a::XXX, 1},{"SEI", &a::SEI, &a::Imp, 2},{"ADC", &a::ADC, &a::Abs, 4},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"ADC", &a::ADC, &a::Abx, 4},{"ROR", &a::ROR, &a::Abx, 4},{"XXX", &a::XXX, &a::XXX, 1},
		/*8*/	{"XXX", &a::XXX, &a::XXX, 1},{"STA", &a::STA, &a::Inx, 6},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"STY", &a::STY, &a::Zpp, 3},{"STA", &a::STA, &a::Zpp, 3},{"STX", &a::STX, &a::Zpp, 3},{"XXX", &a::XXX, &a::XXX, 1},{"DEY", &a::DEY, &a::Imp, 2},{"XXX", &a::XXX, &a::XXX, 1},{"TXA", &a::TXA, &a::Imp, 1},{"XXX", &a::XXX, &a::XXX, 1},{"STY", &a::STY, &a::Abs, 4},{"STA", &a::STA, &a::Abs, 4},{"STX", &a::STX, &a::Abs, 4},{"XXX", &a::XXX, &a::XXX, 1},
		/*9*/	{"BCC", &a::BCC, &a::Rel, 2},{"STA", &a::STA, &a::Iny, 6},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"STY", &a::STY, &a::Zpx, 4},{"STA", &a::STA, &a::Zpx, 4},{"STX", &a::STX, &a::Zpy, 4},{"XXX", &a::XXX, &a::XXX, 1},{"TYA", &a::TYA, &a::Imp, 2},{"STA", &a::STA, &a::Aby, 5},{"TXS", &a::TXS, &a::Imp, 2},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"STA", &a::STA, &a::Abx, 5},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},
		/*A*/	{"LDY", &a::LDY, &a::Imm, 2},{"LDA", &a::LDA, &a::Inx, 6},{"LDX", &a::LDX, &a::Imm, 2},{"XXX", &a::XXX, &a::XXX, 1},{"LDY", &a::LDY, &a::Zpp, 3},{"LDA", &a::LDA, &a::Zpp, 3},{"LDX", &a::LDX, &a::Zpp, 3},{"XXX", &a::XXX, &a::XXX, 1},{"TAY", &a::TAY, &a::Imp, 2},{"LDA", &a::LDA, &a::Imm, 2},{"TAX", &a::TAX, &a::Imp, 2},{"XXX", &a::XXX, &a::XXX, 1},{"LDY", &a::LDY, &a::Abs, 4},{"LDA", &a::LDA, &a::Abs, 4},{"LDX", &a::LDX, &a::Abs, 4},{"XXX", &a::XXX, &a::XXX, 1},
		/*B*/	{"BCS", &a::BCS, &a::Rel, 2},{"LDA", &a::LDA, &a::Iny, 5},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"LDY", &a::LDY, &a::Zpx, 4},{"LDA", &a::LDA, &a::Zpx, 4},{"LDX", &a::LDX, &a::Zpy, 4},{"XXX", &a::XXX, &a::XXX, 1},{"CLV", &a::CLV, &a::Imp, 2},{"LDA", &a::LDA, &a::Aby, 4},{"TSX", &a::TSX, &a::Imp, 2},{"XXX", &a::XXX, &a::XXX, 1},{"LDY", &a::LDY, &a::Abx, 4},{"LDA", &a::LDA, &a::Abx, 4},{"LDX", &a::LDX, &a::Aby, 4},{"XXX", &a::XXX, &a::XXX, 1},
		/*C*/	{"CPY", &a::CPY, &a::Inx, 2},{"CMP", &a::CMP, &a::Inx, 6},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"CPY", &a::CPY, &a::Zpp, 3},{"CMP", &a::CMP, &a::Zpp, 3},{"DEC", &a::DEC, &a::Zpp, 5},{"XXX", &a::XXX, &a::XXX, 1},{"INY", &a::INY, &a::Imp, 2},{"CMP", &a::CMP, &a::Imm, 2},{"DEX", &a::DEX, &a::Imp, 2},{"XXX", &a::XXX, &a::XXX, 1},{"CPY", &a::CPY, &a::Abs, 4},{"CMP", &a::CMP, &a::Abs, 4},{"DEC", &a::DEC, &a::Abs, 6},{"XXX", &a::XXX, &a::XXX, 1},
		/*D*/	{"BNE", &a::BNE, &a::Rel, 2},{"CMP", &a::CMP, &a::Iny, 5},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"CMP", &a::CMP, &a::Zpx, 4},{"DEC", &a::DEC, &a::Zpx, 6},{"XXX", &a::XXX, &a::XXX, 1},{"CLD", &a::CLD, &a::Imp, 2},{"CMP", &a::CMP, &a::Aby, 4},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"CMP", &a::CMP, &a::Abx, 4},{"DEC", &a::DEC, &a::Abx, 7},{"XXX", &a::XXX, &a::XXX, 1},
		/*E*/	{"CPX", &a::CPX, &a::Imm, 2},{"SBC", &a::SBC, &a::Inx, 6},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"CPX", &a::CPX, &a::Zpp, 3},{"SBC", &a::SBC, &a::Zpp, 4},{"INC", &a::INC, &a::Zpp, 5},{"XXX", &a::XXX, &a::XXX, 1},{"INX", &a::INX, &a::Imp, 2},{"SBC", &a::SBC, &a::Imm, 2},{"NOP", &a::NOP, &a::Imp, 2},{"XXX", &a::XXX, &a::XXX, 1},{"CPX", &a::CPX, &a::Abs, 4},{"SBC", &a::SBC, &a::Abs, 4},{"INC", &a::INC, &a::Abs, 6},{"XXX", &a::XXX, &a::XXX, 1},
		/*F*/	{"BEQ", &a::BEQ, &a::Rel, 2},{"SBC", &a::SBC, &a::Iny, 5},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"SBC", &a::CMP, &a::Zpx, 4},{"INC", &a::INC, &a::Zpx, 6},{"XXX", &a::XXX, &a::XXX, 1},{"SED", &a::SED, &a::Imp, 2},{"SBC", &a::SBC, &a::Abs, 4},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"XXX", &a::XXX, &a::XXX, 1},{"SBC", &a::SBC, &a::Abx, 4},{"INC", &a::INC, &a::Abx, 6},{"XXX", &a::XXX, &a::XXX, 1}
		//					0							1							2							3							4					|			5				|			6								7							8								9							A							B							C								D							E							F
	};

	// For debuging purpose only
	void print_binary(uint8_t byte)
	{
		std::cout << "MSB :" << ((byte & 0x80) != 0) << ", " << ((byte & 0x40) != 0) << ", " << ((byte & 0x20) != 0) << ", " << ((byte & 0x10) != 0) << ", " << ((byte & 0x08) != 0) << ", " << ((byte & 0x04) != 0) << ", " << ((byte & 0x02) != 0) << ", " << ((byte & 0x01) != 0) << " LSB" << "\n";
	}
public:

	uint8_t Cycles = 0;

	CPU6502(BUS* main_bus) :bus(main_bus), A_reg(0x00), X_reg(0x00), Y_reg(0x00), PC(0x00), SP(0xff), SR(0x00) {}
	CPU6502(): A_reg(0x00), X_reg(0x00), Y_reg(0x00), PC(0x00), SP(0xff), SR(0x00) {}
	//the cpu read form memory location    FFFC, FFFD to feel the Program Coutner value

	void set_bus(BUS* main_bus)
	{
		bus = main_bus;
	}

	void irq();
	void nmi();
	void Rest();
	void Execute();

	// For debuging purpose only
	void status()
	{
		std::cout << "\n" << "A_reg: " << std::hex << (int)A_reg << " ";
		print_binary(A_reg);
		std::cout << "X_reg: " << std::hex << (int)X_reg << " ";
		print_binary(X_reg);
		std::cout << "Y_reg: " << std::hex << (int)Y_reg << " ";
		print_binary(Y_reg);
		std::cout << "PC   : " << std::hex << (int)PC << "\n";
		std::cout << "SP   : " << std::hex << (int)SP << "\n";
		std::cout << "Negative flage  : " << ((SR & Negative) == Negative) << "\n";
		std::cout << "Overflow flage  : " << ((SR & Overflow) == Overflow) << "\n";
		std::cout << "Break flage     : " << ((SR & Break) == Break) << "\n";
		std::cout << "Decimal flage   : " << ((SR & Decimal) == Decimal) << "\n";
		std::cout << "Interrupt flage : " << ((SR & Interrupt) == Interrupt) << "\n";
		std::cout << "Zero flage      : " << ((SR & Zero) == Zero) << "\n";
		std::cout << "Carry flage     : " << ((SR & Carry) == Carry) << "\n" << "\n";
	}
};


