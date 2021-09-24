#include "CPU6502.h"



void CPU6502::irq()
{
	if (!get_flag(Interrupt))
	{
		//brack the 16 bit address to 8 bit low number and 8 bit high number.
		low = (uint8_t)(PC - 1);
		high = (uint8_t)((PC - 1) >> 8);

		//Store return address in and State register in the stack;
		save_data(0x0100 | SP, low);
		SP--;
		save_data(0x0100 | SP, high);
		SP--;
		save_data(0x0100 | SP, SR);
		SP--;

		//load Interrupt starting address
		low = load_data(0xFFFE);
		high = load_data(0XFFFF);

		PC = (uint16_t)high << 8 | low;
	}
}
void CPU6502::nmi()
{
	//brack the 16 bit address to 8 bit low number and 8 bit high number.
	low = (uint8_t)(PC - 1);
	high = (uint8_t)((PC - 1) >> 8);

	//Store return address in and State register in the stack;
	save_data(0x0100 | SP, low);
	SP--;
	save_data(0x0100 | SP, high);
	SP--;
	save_data(0x0100 | SP, SR);
	SP--;

	//load Interrupt starting address
	low = load_data(0xFFFA);
	high = load_data(0XFFFB);

	PC = (uint16_t)high << 8 | low;
}
void CPU6502::Rest()
{
	
	//this->Cycles = 7;
	low = load_data(0xFFFC);
	high = load_data(0xFFFD);
	PC = (uint16_t)high << 8 | low;

	A_reg = 0x00;
	X_reg = 0x00;
	Y_reg = 0x00;
	SP = 0xff;
	SR = 0x00;
	
}

void CPU6502::Execute()
{
	if (Cycles == 0) {
		Opcode_id = load_Opcode();
		//std::cout << "Opcode: " << list[Opcode_id].name << " loaded  Value: " << std::hex << (uint16_t)Opcode_id << "\n";
		Cycles = list[Opcode_id].Cycles;
		uint8_t address_func = (this->*list[Opcode_id].address)();
		uint8_t Opcode_func = (this->*list[Opcode_id].opcode)();

		Cycles += (address_func + Opcode_func);
		set_Control_pin(SYNC, 1);
	}
	else {
		Cycles--;
		set_Control_pin(SYNC, 0);
		//delay
	}
}

void CPU6502::save_data(uint16_t address, uint8_t data)
{
	bus->ADDRESS_BUS = address;
	bus->DATA_BUS = data;
	bus->save();
}

uint8_t CPU6502::load_data(uint16_t address)
{
	bus->ADDRESS_BUS = address;
	bus->load();
	return bus->DATA_BUS;
}
uint8_t CPU6502::load_data(uint8_t low_address, uint8_t high_address)
{
	return load_data(high_address << 8 | low_address);
}
uint8_t CPU6502::load_data(uint8_t low_address, uint8_t high_address, uint8_t& reg_offset)
{
	return load_data((high_address << 8 | low_address) + reg_offset);
}
uint8_t CPU6502::load_Opcode()
{
	bus->ADDRESS_BUS = PC;
	bus->load();
	PC += 1;
	return bus->DATA_BUS;
}

void CPU6502::set_flag(uint8_t flag, uint8_t state)
{
	if (state == 0)
	{
		SR = SR & ~flag;
	}
	else
	{
		SR = SR | flag;
	}
}
void CPU6502::set_Control_pin(uint8_t pin, uint8_t state)
{
	if (state == 0)
	{
		bus->CONTRL_BUS = bus->CONTRL_BUS & ~pin;
	}
	else
	{
		bus->CONTRL_BUS = bus->CONTRL_BUS | pin;
	}
}

uint8_t CPU6502::get_flag(uint8_t flag)
{
	return (SR & flag) == flag;
}
uint8_t CPU6502::get_Control_pin(uint8_t pin)
{
	return (bus->CONTRL_BUS & pin) == pin;
}
void CPU6502::reset_flags_rag()
{
	SR = 0x00;
}
void CPU6502::Overflow_flag(uint8_t &reg, uint8_t result)
{
	set_flag(Overflow, ((reg & 0x80) ^ (result & 0x80)) != 0);
}


uint8_t CPU6502::Imp() { return 0; }
uint8_t CPU6502::Acc()
{
	data_point = A_reg;
	return 0;
}
uint8_t CPU6502::Imm()
{
	data_point = PC++;
	return 0;
}
uint8_t CPU6502::Zpp()
{
	data_point = load_Opcode(); //one cycle of the cpu
	data_point &= 0x00FF;
	return 0;
}
uint8_t CPU6502::Zpx()
{
	data_point = X_reg + load_Opcode();
	data_point &= 0x00FF;
	return 0;
}
uint8_t CPU6502::Zpy()
{
	data_point = Y_reg + load_Opcode();
	data_point &= 0x00FF;
	return 0;
}
uint8_t CPU6502::Rel()
{
	data_point = load_Opcode();
	if (data_point & 0x80)
		data_point |= 0xFF00;
	return 0;
}
uint8_t CPU6502::Abs()
{
	low = load_Opcode(); //one cycle of the cpu
	high = load_Opcode(); //one cycle of the cpu
	data_point = (uint16_t)high << 8 | low;
	return 0;
}
uint8_t CPU6502::Abx()
{
	low = load_Opcode(); //one cycle of the cpu
	high = load_Opcode(); //one cycle of the cpu

	data_point = ((uint16_t)high << 8 | low) + X_reg;

	if (((PC & 0xff00) + low) > 0xff) //when the address jumps bitween two pages
		return 1;
	else
		return 0;
}
uint8_t CPU6502::Aby()
{
	low = load_Opcode(); //one cycle of the cpu
	high = load_Opcode(); //one cycle of the cpu

	data_point = ((uint16_t)high << 8 | low) + Y_reg;

	if (((PC & 0xff00) + low) > 0xff) //when the address jumps bitween two pages
		return 1;
	else
		return 0;
}
uint8_t CPU6502::Ind()
{
	low = load_Opcode();
	high = load_Opcode();

	address = ((uint16_t)high << 8) | low;

	if (address == 0x00ff)  // Simulate page boundary hardware bug
	{
		low = load_data(address);
		high = load_data(address & 0xff00);
	}
	else {
		low = load_data(address);
		high = load_data(address + 1);
	}

	data_point = ((uint16_t)high << 8) | low;
	return 0;
}
uint8_t CPU6502::Inx()
{
	address = load_Opcode();

	low  = load_data((address + (uint16_t)X_reg) & 0x00FF);
	high = load_data((address + (uint16_t)X_reg + 1) & 0x00FF);

	data_point = ((uint16_t)high << 8) | low;
	return 0;
}
uint8_t CPU6502::Iny()
{
	address = load_Opcode();

	low  = load_data((address + (uint16_t)Y_reg) & 0x00FF);
	high = load_data((address + (uint16_t)Y_reg + 1) & 0x00FF);

	data_point = ((uint16_t)high << 8) | low;
	return 0;
}


uint8_t CPU6502::ADC()
{
	sum = (uint16_t)A_reg + load_data(data_point) + (uint16_t)get_flag(Carry);

	set_flag(Carry, (sum & 0x100) != 0);
	set_flag(Zero, ((uint8_t)sum == 0));
	set_flag(Overflow, ((A_reg & 0x80) ^ ((uint8_t)sum & 0x80)) != 0);
	set_flag(Negative, (((uint8_t)sum & 0x80) != 0));

	A_reg = (uint8_t)sum;

	return 0;
}
uint8_t CPU6502::AND()
{
	sum = A_reg & load_data(data_point);
	A_reg = (uint8_t)sum;

	set_flag(Zero, (A_reg == 0));
	set_flag(Negative, ((A_reg & 0x80) != 0));
	return 0;
}
uint8_t CPU6502::ASL()
{
	if (Cycles > 2)
		sum = load_data(data_point);
	else
		sum = data_point;
	sum = sum << 1;

	set_flag(Carry, (sum & 0x100) != 0);
	set_flag(Zero, ((uint8_t)sum == 0));

	if (Cycles > 2)
		save_data(data_point, (uint8_t)sum);
	else
		A_reg = (uint8_t)sum;

	return 0;
}
uint8_t CPU6502::BCC() 
{ 
	if (!get_flag(Carry))
	{
		sum = PC + data_point;

		if (((PC & 0xff00) + (data_point & 0xff00)) > 0xff) //when the address jumps bitween two pages
			low = 2;
		else
			low = 1;

		PC = sum;
		return low;
	}
	return 0;
}
uint8_t CPU6502::BCS() 
{ 
	if (get_flag(Carry))
	{
		sum = PC + data_point;

		if (((PC & 0xff00) + (data_point & 0xff00)) > 0xff) //when the address jumps bitween two pages
			low = 2;
		else
			low = 1;

		PC = sum;
		return low;
	}
	return 0;
}
uint8_t CPU6502::BEQ() 
{ 
	if (get_flag(Zero))
	{
		sum = PC + data_point;

		if (((PC & 0xff00) + (data_point & 0xff00)) > 0xff) //when the address jumps bitween two pages
			low = 2;
		else
			low = 1;

		PC = sum;
		return low;
	}
	return 0;
}
uint8_t CPU6502::BIT() 
{
	low = A_reg & load_data(data_point);
	set_flag(Zero, low == 0);
	set_flag(Overflow, ((low & 0x40) != 0));
	set_flag(Negative, ((low & 0x80) != 0));
	return 0; 
}
uint8_t CPU6502::BMI() 
{ 
	if (get_flag(Negative))
	{
		sum = PC + data_point;

		if (((PC & 0xff00) + (data_point & 0xff00)) > 0xff) //when the address jumps bitween two pages
			low = 2;
		else
			low = 1;

		PC = sum;
		return low;
	}
	return 0;
}
uint8_t CPU6502::BNE() 
{ 
	if (!get_flag(Zero))
	{
		sum = PC + data_point;

		if (((PC & 0xff00) + (data_point & 0xff00)) > 0xff) //when the address jumps bitween two pages
			low = 2;
		else
			low = 1;

		PC = sum;
		return low;
	}
	return 0;
}
uint8_t CPU6502::BPL() 
{ 
	if (!get_flag(Negative))
	{
		sum = PC + data_point;

		if (((PC & 0xff00) + (data_point & 0xff00)) > 0xff) //when the address jumps bitween two pages
			low = 2;
		else
			low = 1;

		PC = sum;
		return low;
	}
	return 0;
}
uint8_t CPU6502::BRK() 
{ 
	set_flag(Break, 1);
	return 0; 
}
uint8_t CPU6502::BVC() 
{ 
	if (!get_flag(Overflow))
	{
		sum = PC + data_point;

		if (((PC & 0xff00) + (data_point & 0xff00)) > 0xff) //when the address jumps bitween two pages
			low = 2;
		else
			low = 1;

		PC = sum;
		return low;
	}
	return 0;
}
uint8_t CPU6502::BVS() 
{ 
	if (get_flag(Overflow))
	{
		sum = PC + data_point;

		if (((PC & 0xff00) + (data_point & 0xff00)) > 0xff) //when the address jumps bitween two pages
			low = 2;
		else
			low = 1;

		PC = sum;
		return low;
	}
	return 0;
}
uint8_t CPU6502::CLC() 
{ 
	set_flag(Carry, 0);
	return 0; 
}
uint8_t CPU6502::CLD() 
{ 
	set_flag(Decimal, 0);
	return 0; 
}
uint8_t CPU6502::CLI() 
{ 
	set_flag(Interrupt, 0);
	return 0; 
}
uint8_t CPU6502::CLV() 
{
	set_flag(Overflow, 0);
	return 0; 
}
uint8_t CPU6502::CMP() 
{ 
	low = load_data(data_point);
	high = A_reg - low;

	set_flag(Zero, A_reg == low);
	set_flag(Carry, A_reg >= low);
	set_flag(Negative, ((high & 0x80) != 0));
	return 0;
}
uint8_t CPU6502::CPX() 
{ 
	low = load_data(data_point);
	high = X_reg - low;

	set_flag(Zero, X_reg == low);
	set_flag(Carry, X_reg >= low);
	set_flag(Negative, ((high & 0x80) != 0));
	return 0; 
}
uint8_t CPU6502::CPY() 
{ 
	low = load_data(data_point);
	high = Y_reg - low;

	set_flag(Zero, Y_reg == low);
	set_flag(Carry, Y_reg >= low);
	set_flag(Negative, ((high & 0x80) != 0));
	return 0; 
}
uint8_t CPU6502::DEC() 
{
	low = load_data(data_point);
	low -= 1;
	save_data(data_point, low);

	set_flag(Zero, low == 0);
	set_flag(Negative, ((low & 0x80) != 0));
	return 0; 
}
uint8_t CPU6502::DEX() 
{ 
	X_reg -= 1;

	set_flag(Zero, X_reg == 0);
	set_flag(Negative, ((X_reg & 0x80) != 0));
	return 0; 
}
uint8_t CPU6502::DEY() 
{ 
	Y_reg -= 1;

	set_flag(Zero, Y_reg == 0);
	set_flag(Negative, ((Y_reg & 0x80) != 0));
	return 0; 
}
uint8_t CPU6502::EOR() 
{ 
	A_reg = A_reg ^ load_data(data_point);

	set_flag(Zero, A_reg == 0);
	set_flag(Negative, ((A_reg & 0x80) != 0));
	return 0; 
}
uint8_t CPU6502::INC() 
{ 
	low = load_data(data_point);
	low += 1;
	save_data(data_point, low);

	set_flag(Zero, low == 0);
	set_flag(Negative, ((low & 0x80) != 0));
	return 0; 
}
uint8_t CPU6502::INX() 
{ 
	X_reg += 1;

	set_flag(Zero, X_reg == 0);
	set_flag(Negative, ((X_reg & 0x80) != 0));
	return 0;
}
uint8_t CPU6502::INY() 
{ 
	Y_reg += 1;

	set_flag(Zero, Y_reg == 0);
	set_flag(Negative, ((Y_reg & 0x80) != 0));
	return 0;
}
uint8_t CPU6502::JMP() 
{ 
	PC = data_point;
	return 0; 
}
uint8_t CPU6502::JSR() 
{
	low = (uint8_t)(PC - 1);
	high = (uint8_t)((PC - 1) >> 8);

	save_data(0x0100 | SP, high);
	SP--;
	save_data(0x0100 | SP, low);
	SP--;

	PC = data_point;

	return 0; 
}
uint8_t CPU6502::LDA() 
{ 
	A_reg = load_data(data_point);

	set_flag(Zero, A_reg == 0);
	set_flag(Negative, ((A_reg & 0x80) != 0));
	return 0; 
}
uint8_t CPU6502::LDX() 
{
	X_reg = load_data(data_point);

	set_flag(Zero, X_reg == 0);
	set_flag(Negative, ((X_reg & 0x80) != 0));
	return 0; 
}
uint8_t CPU6502::LDY() 
{ 
	Y_reg = load_data(data_point);

	set_flag(Zero, Y_reg == 0);
	set_flag(Negative, ((Y_reg & 0x80) != 0));
	return 0;
}
uint8_t CPU6502::LSR() 
{ 
	if (2 < Cycles)
		low = load_data(data_point);
	else
		low = data_point;

	set_flag(Carry, (low & 0x01) != 0);
	low = low >> 1;

	if (2 < Cycles)
		save_data(data_point, low);
	else
		A_reg = low;

	set_flag(Zero, A_reg == 0);
	set_flag(Negative, ((A_reg & 0x80) != 0));

	return 0; 
}
uint8_t CPU6502::NOP() { return 0; }

uint8_t CPU6502::ORA() //okay
{ 
	A_reg = A_reg | load_data(data_point);

	set_flag(Zero, A_reg == 0);
	set_flag(Negative, ((A_reg & 0x80) != 0));
	return 0;
}
uint8_t CPU6502::PHA() 
{ 
	save_data(0x0100 | SP, A_reg);
	SP--;
	return 0; 
}
uint8_t CPU6502::PHP() 
{ 
	save_data(0x0100 | SP, SR);
	SP--;
	return 0; 
}
uint8_t CPU6502::PLA() 
{ 
	SP++;
	A_reg = load_data(0x0100 | SP);

	set_flag(Zero, A_reg == 0);
	set_flag(Negative, ((A_reg & 0x80) != 0));
	return 0; 
}
uint8_t CPU6502::PLP() 
{ 
	SP++;
	SR = load_data(0x0100 | SP);
	return 0; 
}
uint8_t CPU6502::ROL() //Okay
{ 
	if (2 < Cycles)
		low = load_data(data_point);
	else
		low = data_point;

	if ((low & 0x80) != 0)
	{
		set_flag(Carry, 1);
		low = (low << 1) | 0x01;
	}
	else
	{
		set_flag(Carry, 0);
		low = (low << 1);
	}

	if (2 < Cycles)
		save_data(data_point, low);
	else
		A_reg = low;

	set_flag(Zero, A_reg == 0);
	set_flag(Negative, ((low & 0x80) != 0));

	return 0; 
}
uint8_t CPU6502::ROR() //Okay
{ 
	if (2 < Cycles)
		low = load_data(data_point);
	else
		low = data_point;

	if ((low & 0x01) != 0)
	{
		set_flag(Carry, 1);
		low = (low >> 1) | 0x80;
	}
	else
	{
		set_flag(Carry, 0);
		low = (low >> 1);
	}

	if (2 < Cycles)
		save_data(data_point, low);
	else
		A_reg = low;

	set_flag(Zero, A_reg == 0);
	set_flag(Negative, ((low & 0x80) != 0));

	return 0;
}
uint8_t CPU6502::RTI() 
{ 
	SP++;
	SR = load_data(0x0100 | SP);
	SP++;
	low = load_data(0x0100 | SP);
	SP++;
	high = load_data(0x0100 | SP);

	PC = (uint16_t)high << 8 | low;
	PC++;

	return 0; 
} 
uint8_t CPU6502::RTS() 
{ 
	SP++;
	low = load_data(0x0100 | SP);
	SP++;
	high = load_data(0x0100 | SP);

	PC = (uint16_t)high << 8 | low;
	PC++;

	return 0; 
}
uint8_t CPU6502::SBC() 
{ 
	low = load_data(data_point);
	address = (uint16_t)low ^ 0x00ff;

	sum = (uint16_t)A_reg + address + (uint16_t)get_flag(Carry);

	set_flag(Carry, (sum & 0x100) != 0);
	set_flag(Zero, ((uint8_t)sum == 0));
	set_flag(Overflow, ((A_reg & 0x80) ^ ((uint8_t)sum & 0x80)) != 0);
	set_flag(Negative, (((uint8_t)sum & 0x80) != 0));

	A_reg = (uint8_t)sum;

	return 0; 
}
uint8_t CPU6502::SEC() 
{ 

	set_flag(Carry, 1);
	return 0;
}
uint8_t CPU6502::SED() 
{ 
	set_flag(Decimal, 1);
	return 0; 
}
uint8_t CPU6502::SEI() 
{ 
	set_flag(Interrupt, 1);
	return 0; 
}
uint8_t CPU6502::STA() 
{ 
	save_data(data_point, A_reg);
	return 0; 
}
uint8_t CPU6502::STX() 
{
	save_data(data_point, X_reg);
	return 0; 
}
uint8_t CPU6502::STY() 
{ 
	save_data(data_point, Y_reg);
	return 0; 
}
uint8_t CPU6502::TAX() 
{ 
	X_reg = A_reg;

	set_flag(Zero, X_reg == 0);
	set_flag(Negative, ((X_reg & 0x80) != 0));
	return 0; 
}
uint8_t CPU6502::TAY() 
{ 
	Y_reg = A_reg;

	set_flag(Zero, Y_reg == 0);
	set_flag(Negative, ((Y_reg & 0x80) != 0));
	return 0; 
}
uint8_t CPU6502::TSX() 
{ 
	X_reg = SP;

	set_flag(Zero,  X_reg == 0);
	set_flag(Negative, ((X_reg & 0x80) != 0));
	return 0; 
}
uint8_t CPU6502::TXA() 
{ 
	A_reg = X_reg;

	set_flag(Zero, (A_reg == 0));
	set_flag(Negative, ((A_reg & 0x80) != 0));
	return 0; 
}
uint8_t CPU6502::TXS() 
{ 
	SP = X_reg;
	return 0; 
}
uint8_t CPU6502::TYA() 
{ 
	A_reg = Y_reg;

	set_flag(Zero, (A_reg == 0));
	set_flag(Negative, ((A_reg & 0x80) != 0));

	return 0; 
}

uint8_t CPU6502::XXX() { return 0; }