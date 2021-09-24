#pragma once
#include <iostream>
#include <fstream>
#include <strstream>

class module
{
public:
	uint16_t    TOP_ADDRESS;
	uint16_t    BUTTOM_ADDRESS;


	virtual uint8_t send_data(uint16_t& address) { return 0; };
	virtual void receive_data(uint16_t& address, uint8_t& data) {};
	virtual void interrupt(uint8_t& contrl) {};
};

class memory : public module
{
public:
	uint8_t* memory_stack;

	memory(uint16_t top = 0xFFFF, uint16_t buttom = 0x00)
	{
		TOP_ADDRESS = top;
		BUTTOM_ADDRESS = buttom;
		memory_stack = (uint8_t*)malloc((top - buttom) * sizeof(uint8_t));
	};

	~memory() { delete(memory_stack); }

	uint8_t send_data(uint16_t& address)
	{
		return memory_stack[address];
	}
	void receive_data(uint16_t& address, uint8_t& data)
	{
		memory_stack[address] = data;
	}

	void interrupt(uint8_t& contrl) {}

	void Print_memory(uint16_t start, uint16_t end)
	{
		uint16_t  current;
		for (int y = 0; y < 6553; y++) {
			std::cout << "0x" << std::hex << (int)start + (y * 16);
			for (int x = 0; x < 16; x++) {
				current = start + (y * 16) + x;
				if (current <= end)
					std::cout << " , " << std::hex << (int)memory_stack[start + (y * 16) + x];
				else {
					std::cout << "\n";
					std::cout << "\n";
					return;
				}
			}
			std::cout << "\n";
		}
	}

	void load_memory_from_file(std::string file_name)
	{
		std::fstream f(file_name);
		if (!f.is_open())
			return ;
		char line[128];

		std::strstream s;
		uint16_t address;
		uint8_t low, high;
		for (int i = 0; i < 128; i++) {
			line[i] = 0x0;
		}
		f.getline(line, 128);
		for (int i = 0; i < 4; i++)
		{
			if (line[i] <= '9' && line[i] >= '0')
				low = line[i] - '0';
			else
				low = line[i] - 'a' + 0xa;
			low &= 0x0f;
			address = (address << 4) | low;
		}

		
		for(int L = 5; L < 53; L += 3)
		{
			if (line[L + 1] <= '9' && line[L + 1] >= '0')
				high = line[L + 1] - '0';
			else
				high = line[L + 1] - 'a' + 0xa;

			if (line[L + 2] <= '9' && line[L + 2] >= '0')
				low = line[L + 2] - '0';
			else
				low = line[L + 2] - 'a' + 0xa;

			memory_stack[address] = high << 4 | low;
			address++;
		}

		while (!f.eof())
		{
			for (int i = 0; i < 128; i++) {
				line[i] = 0x0;
			}
			f.getline(line, 128);
			for (int i = 5; i < 53; i += 3)
			{
				if (line[i] != '\n') {
					if (line[i + 1] <= '9' && line[i + 1] >= '0')
						high = line[i + 1] - '0';
					else
						high = line[i + 1] - 'a' + 0xa;

					if (line[i + 2] <= '9' && line[i + 2] >= '0')
						low = line[i + 2] - '0';
					else
						low = line[i + 2] - 'a' + 0xa;

					memory_stack[address] = high << 4 | low;
					address++;
				}
				else {
					i = 53;
				}
			}
		}
	}
};

struct BUS
{
	uint8_t     CONTRL_BUS;
	uint8_t     DATA_BUS;
	uint16_t    ADDRESS_BUS;
	module**    Priferals;
	int count = 0;

	void load()
	{
		if (Priferals == nullptr) {
			std::cout << "SYSTEM::BUS::ON_MODUALS_EXIST" << "\n";
			return;
		}
		for (int i = 0; i < count; i++)
		{
			if (Priferals[i] == NULL) {
				std::cout << "SYSTEM::BUS::LOAD_ERROR::MODUAL_NOT_CONNECTED_IN_SLOOT: " << i << " OF: " << count << "\n";
			}
			else if (ADDRESS_BUS <= Priferals[i]->TOP_ADDRESS && ADDRESS_BUS >= Priferals[i]->BUTTOM_ADDRESS)
			{
				DATA_BUS = Priferals[i]->send_data(ADDRESS_BUS);
				return;
			}
		}
		std::cout << "SYSTEM::BUS::LOAD_ERROR::ON_MODUALS_EXZIST_IN_ADDRESS: " << std::hex << ADDRESS_BUS << "\n";
	}
	void save()
	{
		if (Priferals == nullptr) {
			std::cout << "SYSTEM::BUS::ON_MODUALS_EXIST" << "\n";
			return;
		}
		for (int i = 0; i < count; i++)
		{
			if (Priferals[i] == NULL) {
				std::cout << "SYSTEM::BUS::SAVE_ERROR::MODUAL_NOT_CONNECTED_IN_SLOOT: " << i << " OF: " << count << "\n";
			}
			else if (ADDRESS_BUS <= Priferals[i]->TOP_ADDRESS && ADDRESS_BUS >= Priferals[i]->BUTTOM_ADDRESS)
			{
				Priferals[i]->receive_data(ADDRESS_BUS, DATA_BUS);
				return;
			}
		}
		std::cout << "SYSTEM::BUS::SAVE_ERROR::ON_MODUALS_EXZIST_IN_ADDRESS: " << std::hex << ADDRESS_BUS << "\n";
	}
	void interrupt()
	{
		if (Priferals == nullptr) {
			std::cout << "SYSTEM::BUS::ON_MODUALS_ARE_EXIST" << "\n";
			return;
		}
		for (int i = 0; i < count; i++)
		{
			if (Priferals[i] == NULL) {
				std::cout << "SYSTEM::BUS::INTERRUPT_ERROR::MODUAL_NOT_CONNECTED_IN_SLOOT: " << i << " OF: " << count << "\n";
			}
			else if (ADDRESS_BUS <= Priferals[i]->TOP_ADDRESS && ADDRESS_BUS >= Priferals[i]->BUTTOM_ADDRESS)
			{
				Priferals[i]->interrupt(CONTRL_BUS);
				return;
			}
		}
		std::cout << "SYSTEM::BUS::INTERRUPT_ERROR::ON_MODUALS_EXZIST_IN_ADDRESS: " << std::hex << ADDRESS_BUS << "\n";
	}
};

