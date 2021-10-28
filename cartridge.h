#pragma once
#include "mapper.h"


class cartridge
{
private:

    uint8_t Header[16];
    uint8_t *PGR_memory;
    uint8_t *CHR_memory;

    uint8_t mapper_type;
    mapper  *Map;

    std::string des_to_hex(uint8_t number)
	{
		std::string hex;
		std::string list = "0123456789ABCDEF";
		hex += list[number >> 4];
		hex += list[(number & 0x0f)];
		return hex;
	}

    std::string Print_memory(uint16_t start, uint16_t end, uint8_t* mem)
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
					text += " "; text += des_to_hex(mem[start + (y * 16) + x]); 
				}
				else {
					return text;
				}
			}
			text += "\n";
		}
		return text;
	}

    void set_mapper(uint8_t type);
    uint8_t map_index();
public:
    cartridge(const char* rom_file);
    ~cartridge();

    

    mapper* get_map()
    {
        return Map;
    }
};


