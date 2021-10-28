#define OLC_PGE_APPLICATION
#include <iostream>
#include "CPU6502.h"
#include "olcPixelGameEngine.h"

memory* view;


void init_bus(BUS &bus)
{
    bus.Priferals = (module**)calloc(1, sizeof(module*));
    bus.count = 1;
	view = new memory();
	bus.Priferals[0] = view;
    ((memory*)bus.Priferals[0])->memory_stack[0xFFFC] = 0x00;
    ((memory*)bus.Priferals[0])->memory_stack[0xFFFD] = 0x06;

	((memory*)bus.Priferals[0])->load_memory_from_file("Snack_6502.txt");

    /*((memory*)bus.Priferals[0])->memory_stack[0x0600] = 0xa9;
	((memory*)bus.Priferals[0])->memory_stack[0x0601] = 0x45;
	((memory*)bus.Priferals[0])->memory_stack[0x0602] = 0x38;
	((memory*)bus.Priferals[0])->memory_stack[0x0603] = 0xe9;
	((memory*)bus.Priferals[0])->memory_stack[0x0604] = 0x20;
	((memory*)bus.Priferals[0])->memory_stack[0x0605] = 0xa2;
	((memory*)bus.Priferals[0])->memory_stack[0x0606] = 0x53;
	((memory*)bus.Priferals[0])->memory_stack[0x0607] = 0x60;
	((memory*)bus.Priferals[0])->memory_stack[0x0608] = 0x00;
	((memory*)bus.Priferals[0])->memory_stack[0x0609] = 0x00;
	((memory*)bus.Priferals[0])->memory_stack[0x060a] = 0x02;*/

}
/*
int main(){
    BUS main_bus;
    CPU6502 cpu(&main_bus);
    init_bus(main_bus);
    cpu.Rest();
	cpu.status();
	((memory*)main_bus.Priferals[0])->Print_memory(0x01f0, 0x1ff);
	cpu.Execute();
	cpu.status();
	((memory*)main_bus.Priferals[0])->Print_memory(0x01f0, 0x1ff);
	cpu.Execute();
	cpu.status();
	((memory*)main_bus.Priferals[0])->Print_memory(0x01f0, 0x1ff);
	cpu.Execute();
	cpu.status();
	((memory*)main_bus.Priferals[0])->Print_memory(0x01f0, 0x1ff);
	cpu.Execute();
	cpu.status();

	((memory*)main_bus.Priferals[0])->Print_memory(0x0600, 0x750);
	
    std::cout << "CPU status\n";
    return 1;
}*/

// Override base class with your custom functionality
class Example : public olc::PixelGameEngine
{
public:
	
	BUS main_bus;
	CPU6502 cpu;
	float Time;
	uint16_t scroll_address = 0x0;

	int flag0 = 0;
	int flag1 = 0;
	int flag2 = 0;
	int flag3 = 0;
	

	olc::Pixel p_color(uint8_t value)
	{
		switch (value)
		{
		case 0x0:
			return olc::BLACK;
			break;
		case 0x1:
			return olc::WHITE;
			break;
		case 0x2:
			return olc::RED;
			break;
		case 0x3:
			return olc::CYAN;
			break;
		case 0x4:
			return olc::MAGENTA;
			break;
		case 0x5:
			return olc::GREEN;
			break;
		case 0x6:
			return olc::BLUE;
			break;
		case 0x7:
			return olc::YELLOW;
			break;
		case 0x8:
			return olc::DARK_YELLOW;
			break;
		case 0x9:
			return olc::VERY_DARK_BLUE;
			break;
		case 0xa:
			return olc::DARK_RED;
			break;
		case 0xb:
			return olc::DARK_GREY;
			break;
		case 0xc:
			return olc::GREY;
			break;
		case 0xd:
			return olc::VERY_DARK_RED;
			break;
		case 0xe:
			return olc::VERY_DARK_CYAN;
			break;
		case 0xf:
			return olc::YELLOW;
			break;
		default:
			return olc::BLACK;
			break;
		}
	}

	std::string des_to_hex(uint8_t number)
	{
		std::string hex;
		std::string list = "0123456789ABCDEF";
		hex += list[number >> 4];
		hex += list[(number & 0x0f)];
		return hex;
	}

	std::string addressing(uint8_t(CPU6502::*address)(void), uint16_t pc)
	{
		std::string text;
		text.clear();

		if (address == &CPU6502::Imp) {return " ";}
		if (address == &CPU6502::Acc) 
		{ 
			text += "A";
			return text;
		}
		if (address == &CPU6502::Imm) 
		{ 
			text += "#$" + des_to_hex(((memory*)main_bus.Priferals[0])->memory_stack[pc + 1]);
			return text; 
		}
		if (address == &CPU6502::Zpp) 
		{ 
			text += "$" + des_to_hex(((memory*)main_bus.Priferals[0])->memory_stack[pc + 1]);
			return text;
		}
		if (address == &CPU6502::Zpx) 
		{ 
			text += "$" + des_to_hex(((memory*)main_bus.Priferals[0])->memory_stack[pc + 1]) + ",X";
			return text;
		}
		if (address == &CPU6502::Zpy) 
		{
			text += "$" + des_to_hex(((memory*)main_bus.Priferals[0])->memory_stack[pc + 1]) + ",Y";
			return text;
		}
		if (address == &CPU6502::Rel) 
		{ 
			uint16_t data_point = ((memory*)main_bus.Priferals[0])->memory_stack[pc + 1];
			if (data_point & 0x80)
				data_point |= 0xFF00;
			data_point = pc + data_point + 2;

			text += "$" + des_to_hex(data_point >> 8 ) + des_to_hex(data_point);
			return text;
		}
		if (address == &CPU6502::Abs) 
		{ 
			text += "$" + des_to_hex(((memory*)main_bus.Priferals[0])->memory_stack[pc + 2]) + des_to_hex(((memory*)main_bus.Priferals[0])->memory_stack[pc + 1]);
			return text;
		}
		if (address == &CPU6502::Abx) 
		{
			text += "$" + des_to_hex(((memory*)main_bus.Priferals[0])->memory_stack[pc + 2]) + des_to_hex(((memory*)main_bus.Priferals[0])->memory_stack[pc + 1]) + ",X";
			return text;
		}
		if (address == &CPU6502::Aby)
		{
			text += "$" + des_to_hex(((memory*)main_bus.Priferals[0])->memory_stack[pc + 2]) + des_to_hex(((memory*)main_bus.Priferals[0])->memory_stack[pc + 1]) + ",Y";
			return text;
		}
		if (address == &CPU6502::Ind) 
		{ 
			uint8_t low = ((memory*)main_bus.Priferals[0])->memory_stack[pc + 1];
			uint8_t high = ((memory*)main_bus.Priferals[0])->memory_stack[pc + 2];

			uint16_t address = ((uint16_t)high << 8) | low;

			if (address == 0x00ff)  // Simulate page boundary hardware bug
			{
				low = ((memory*)main_bus.Priferals[0])->memory_stack[address];
				high = ((memory*)main_bus.Priferals[0])->memory_stack[address & 0xff00];
			}
			else {
				low = ((memory*)main_bus.Priferals[0])->memory_stack[address];
				high = ((memory*)main_bus.Priferals[0])->memory_stack[address + 1];
			}

			text += "$" + des_to_hex(((uint16_t)high << 8) | low);
			return text;
		}
		if (address == &CPU6502::Inx) 
		{ 
			text += "$(" + des_to_hex(((memory*)main_bus.Priferals[0])->memory_stack[pc + 1]) + "),X";
			return text;
		}
		if (address == &CPU6502::Iny) 
		{ 
			text += "$(" + des_to_hex(((memory*)main_bus.Priferals[0])->memory_stack[pc + 1]) + "),Y";
			return text;
		}
		return "XXX";
	}

	uint16_t extra_byte(uint8_t(CPU6502::*address)(void))
	{
		if (address == &CPU6502::Imp){ return 0; }
		if (address == &CPU6502::Acc){ return 0; }
		if (address == &CPU6502::Imm){ return 1; }
		if (address == &CPU6502::Zpp){ return 1; }
		if (address == &CPU6502::Zpx){ return 1; }
		if (address == &CPU6502::Zpy){ return 1; }
		if (address == &CPU6502::Rel){ return 1; }
		if (address == &CPU6502::Abs){ return 2; }
		if (address == &CPU6502::Abx){ return 2; }
		if (address == &CPU6502::Aby){ return 2; }
		if (address == &CPU6502::Ind){ return 2; }
		if (address == &CPU6502::Inx){ return 1; }
		if (address == &CPU6502::Iny){ return 1; }

	}

	std::string print_code(uint16_t length)
	{
		std::string text;
		text.clear();
		uint16_t offset = 0x0;
		for (uint16_t i = 0; i < length; i++)
		{
			text += des_to_hex((cpu.PC + offset) >> 8);
			text += des_to_hex(cpu.PC + offset);
			text += ' ';
			text += cpu.list[((memory*)main_bus.Priferals[0])->memory_stack[cpu.PC + offset]].name; text += ' ' + addressing(cpu.list[((memory*)main_bus.Priferals[0])->memory_stack[cpu.PC + offset]].address, cpu.PC + offset);
			text += '\n'; text += '\n';
			offset = offset + 1 + extra_byte(cpu.list[((memory*)main_bus.Priferals[0])->memory_stack[cpu.PC + offset]].address);
		}

		return text;
	}

	std::string Print_memory(uint16_t start, uint16_t end)
	{
		std::string text;
		text.clear();
		uint16_t  current;
		text += "      ";
		for (int x = 0; x < 16; x++) {
			text += " ";
			text += des_to_hex(x);
		}
		text += "\n\n";
		for (int y = 0; y < 6553; y++) {
			text += "0x"; text += des_to_hex((start + (y * 16)) >> 8); text += des_to_hex(start + (y * 16));

			for (int x = 0; x < 16; x++) {
				current = start + (y * 16) + x;
				if (current <= end){
					text += " "; text += des_to_hex(((memory*)main_bus.Priferals[0])->memory_stack[start + (y * 16) + x]); 
				}
				else {
					return text;
				}
			}
			text += "\n";
		}
		return text;
	}

	std::string status()
	{
		std::string text;
		text.clear();

		text += "\nA_reg: "; text += des_to_hex(cpu.A_reg); text += '\n';
		text += "X_reg: "; text += des_to_hex(cpu.X_reg); text += '\n';
		text += "Y_reg: "; text += des_to_hex(cpu.Y_reg); text += '\n';
		text += "PC   : "; text += des_to_hex(cpu.PC >> 8); text += des_to_hex(cpu.PC);	  text += '\n';
		text += "SP   : "; text += des_to_hex(cpu.SP);	  text += '\n';
		text += "Negative flage  : "; text += des_to_hex((cpu.SR & cpu.Negative)	== cpu.Negative)	;text+='\n';
		text += "Overflow flage  : "; text += des_to_hex((cpu.SR & cpu.Overflow)	== cpu.Overflow)	;text+='\n';
		text += "Break flage     : "; text += des_to_hex((cpu.SR & cpu.Break)		== cpu.Break)		;text+='\n';
		text += "Decimal flage   : "; text += des_to_hex((cpu.SR & cpu.Decimal)	== cpu.Decimal)		;text+='\n';
		text += "Interrupt flage : "; text += des_to_hex((cpu.SR & cpu.Interrupt) == cpu.Interrupt)	;text+='\n';
		text += "Zero flage      : "; text += des_to_hex((cpu.SR & cpu.Zero)		== cpu.Zero)		;text+='\n';
		text += "Carry flage     : "; text += des_to_hex((cpu.SR & cpu.Carry)		== cpu.Carry)		;text+='\n';

		return text;
	}

	Example()
	{
		// Name you application
		sAppName = "Example";
	}

public:

	void random() {
		((memory*)main_bus.Priferals[0])->memory_stack[0x00fe] = std::rand() % 16;
	}

	void input()
	{
		if (GetKey(olc::W).bPressed) {
			((memory*)main_bus.Priferals[0])->memory_stack[0x00ff] = 0x77;
		}
		if (GetKey(olc::A).bPressed) {
			((memory*)main_bus.Priferals[0])->memory_stack[0x00ff] = 0x61;
		}
		if (GetKey(olc::S).bPressed) {
			((memory*)main_bus.Priferals[0])->memory_stack[0x00ff] = 0x73;
		}
		if (GetKey(olc::D).bPressed) {
			((memory*)main_bus.Priferals[0])->memory_stack[0x00ff] = 0x64;
		}
		if (GetKey(olc::SPACE).bPressed) {
			cpu.Execute();
			while(cpu.Cycles > 0)
				cpu.Execute();
		}
		if (GetKey(olc::ENTER).bPressed) {
			if (flag0 == 0) { flag0 = 1; }
			else { flag0 = 0; }
			
		}
		if (GetKey(olc::ESCAPE).bPressed) {
			cpu.Rest();
			flag3 = 0;

			for (int x = 0; x < 32; x++)
				for (int y = 0; y < 32; y++)
					((memory*)main_bus.Priferals[0])->memory_stack[0x200 + (y * 32) + x] = 0x00;

		}
	}

	void flags()
	{
		if (cpu.PC == 0x0735 && flag3 == 0) {
			flag3 = 1;
		}
	}

	bool OnUserCreate() override
	{
		Time = 0.f;
		// Called once at the start, so create things here
		cpu.set_bus(&main_bus);
		init_bus(main_bus);
		((memory*)main_bus.Priferals[0])->memory_stack[0x00ff] = 0x64;

		cpu.Rest();
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		input();
		flags();
		if (flag3 == 0 && flag0 == 1) {
			random();
			cpu.Execute();
		}
		Time += fElapsedTime;

		if (Time > 0.04) {
			Clear(olc::DARK_CYAN);
			//DrawString(10, 10, "Instraction memory", olc::WHITE, 2);
			//DrawString(10, 40, Print_memory(0x100, 0x200), olc::WHITE, 2);
			//DrawString(10, 450, Print_memory(0x000, 0x100), olc::WHITE, 2);

			//DrawString(1000, 10, "CPU Status", olc::WHITE, 2);
			DrawString(920, 20, status(), olc::WHITE, 2);

			//DrawString(920, 280, "Current_Opcode", olc::WHITE, 2);
			DrawString(920, 300, print_code(14), olc::WHITE, 2);
			for (int x = 0; x < 32; x++)
				for (int y = 0; y < 32; y++)
					FillRect(x * 5, y * 5, 5, 5, p_color(((memory*)main_bus.Priferals[0])->memory_stack[0x200 + (y * 32) + x]));

			Time = 0;
		}
		/*if (cpu.PC == 0x0735)
			return false;
		else*/
			return true;
	}
};

int main()
{
	Example demo;
	if (demo.Construct(1280, 800, 1, 1))
		demo.Start();
	return 0;
}