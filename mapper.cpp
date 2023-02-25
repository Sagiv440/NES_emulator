#include "mapper.h"
#include <iostream>


//Mapper 0 
mapper0::mapper0(uint8_t *pgr, uint8_t *chr, uint8_t mirror, uint8_t &Mem16):mapper(pgr,chr,mirror),Mem16_32(Mem16){}

uint16_t mapper0::address_adapter(uint16_t *address)
{
    if(Mem16_32 == 1)
        return 0x7fff & *address + 0x4000;
    else
        return 0x7fff & *address;
}
uint8_t mapper0::cpu_load(uint16_t *address)
{
    if(*address >= 0x6000 && *address < 0x8000)
    {
        return Ram[*address - 0x6000];
    }
    else //(*address >= 0x8000)
    {
        return PGR_memory[address_adapter(address)];
    }
}
void mapper0::cpu_save(uint16_t *address, uint8_t data)
{
    if(*address >= 0x6000 && *address < 0x8000)
    {
        Ram[*address - 0x6000] = data;
    }
    else //(*address >= 0x8000)
    {
        PGR_memory[address_adapter(address)] = data;
    }
}
uint8_t mapper0::ppu_load(uint16_t *address)
{
    return CHR_memory[*address];
}
void mapper0::ppu_save(uint16_t *address, uint8_t data)
{
    PGR_memory[*address] = data;
}


//Mapper1
mapper1::mapper1(uint8_t *pgr, uint8_t *chr, uint8_t mirror, uint8_t &Mem16):mapper(pgr,chr, mirror),Mem16_32(Mem16){}

uint16_t mapper1::address_adapter(uint16_t *address)
{
    if(Mem16_32 == 1)
        return 0x7fff & *address + 0x4000;
    else
        return 0x7fff & *address;
}
uint8_t mapper1::cpu_load(uint16_t *address)
{
    if(*address >= 0x6000 && *address < 0x8000)
    {
        return Ram[*address - 0x6000];
    }
    else //(*address >= 0x8000)
    {
        return PGR_memory[address_adapter(address)];
    }
}
void mapper1::cpu_save(uint16_t *address, uint8_t data)
{
    if(*address >= 0x6000 && *address < 0x8000)
    {
        Ram[*address - 0x6000] = data;
    }
    else //(*address >= 0x8000)
    {
        if(data & 0x80 == 0x80)
        {
            Control_count = 0;
            for(uint8_t i = 0; i < 5; i++){ Control_reg[i] = 0;};
        }else
        {
            Control_reg[Control_count] = data;
            Control_count = (Control_count < 5)? Control_count++:0;
        }
    }
}
uint8_t mapper1::ppu_load(uint16_t *address)
{
    return CHR_memory[*address];
}
void mapper1::ppu_save(uint16_t *address, uint8_t data)
{
    PGR_memory[*address] = data;
}

