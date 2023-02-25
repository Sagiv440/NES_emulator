
#include <iostream>
#include <fstream>
#include "cartridge.h"

void cartridge::set_mapper(uint8_t type)
{
    switch(type)
    {
    case 0:
        Map = (mapper*)new mapper0(PGR_memory, CHR_memory, mirror, Header[4]);
        break;
    default:
        Map = NULL;
        break;
    }
}


uint8_t cartridge::map_index()
{
    uint8_t high,low;
    low = Header[6];
    high = Header[7];
    high = (high & 0xf0) | (low >> 4);
    return high;
}

cartridge::cartridge(const char* rom_file)
{
    std::ifstream rf(rom_file, std::ios::out | std::ios::binary);
    if(!rf) {std::cout << "ERROR_START_UP_FAILD TO LOAD GAME FILE" << "\n"; return;}
    rf.read((char*) &Header, 16);

    PGR_memory = (uint8_t*)malloc(sizeof(uint8_t) * 0x4000 * Header[4]); // Allocating memory for the game rom
    if(Header[5] != 0)
        CHR_memory = (uint8_t*)malloc(sizeof(uint8_t) * 0x2000 * Header[5]); // Allocating memory for the graphics rom
    else
        CHR_memory = (uint8_t*)malloc(sizeof(uint8_t) * 0x2000); // Allocating memory for the graphics rom

    mirror = (Header[6] & 0x08) > 0? 1:0;
    mirror = (mirror << 1) | (Header[6] & 0x01);
    set_mapper(map_index()); //set the correct mapper;


    if(Header[6] & 0x04){ // If trainer exsist load trainer to ram;
        rf.read((char*) &Map->Ram[0x1000], 512);
    }

    //loading the Game in to memory
    for(uint8_t p = 0;p < Header[4];p++){
        rf.read((char*) &PGR_memory[0x4000 * p], 0x4000);
    }
    //loading the Graphics in to memory
    for(uint8_t c = 0;c < Header[5];c++){
        rf.read((char*) &CHR_memory[0x2000 * c], 0x2000);
    }

    //std::cout << Print_memory(0, 0x4000 * Header[4] ,PGR_memory).c_str();
    //std::cout << Print_memory(0, 0x2000 ,PGR_memory);
    //std::cout << Print_memory(0, 0x2000 * Header[5] ,CHR_memory);
    std::cout << "\n";
 
}
cartridge::~cartridge()
{
    delete(Map);
}
