#pragma once
#include <iostream>
class mapper
{
public:
    uint8_t mirror;
    uint8_t *PGR_memory;
    uint8_t *CHR_memory;
    uint8_t Ram[0x2000] = {0x00};
public:
    mapper(uint8_t *pgr, uint8_t *chr, uint8_t mirror):PGR_memory(pgr), CHR_memory(chr), mirror(mirror){};
    ~mapper() {delete(PGR_memory);  delete(CHR_memory);};

    virtual uint8_t cpu_load(uint16_t *address){return 0;};
    virtual void cpu_save(uint16_t *address, uint8_t data){};
    
    virtual uint8_t ppu_load(uint16_t *address){return 0;};
    virtual void ppu_save(uint16_t *address, uint8_t data){};

};
class mapper0 : public mapper
{
private:
    uint8_t Mem16_32; //hold the type of memory module

    uint16_t address_adapter(uint16_t *address);
public:
    mapper0(uint8_t *pgr, uint8_t *chr, uint8_t mirror, uint8_t &Mem16);

    uint8_t cpu_load(uint16_t *address);
    void cpu_save(uint16_t *address, uint8_t data);
    uint8_t ppu_load(uint16_t *address);
    void ppu_save(uint16_t *address, uint8_t data);
};


class mapper1 : public mapper
{
private:
    uint8_t Mem16_32; //hold the type of memory module
    uint8_t Control_count = 0;
    uint8_t Control_reg[5] = {0};

    uint16_t address_adapter(uint16_t *address);
public:
    mapper1(uint8_t *pgr, uint8_t *chr, uint8_t mirror, uint8_t &Mem16);

    uint8_t cpu_load(uint16_t *address);
    void cpu_save(uint16_t *address, uint8_t data);
    
    uint8_t ppu_load(uint16_t *address);
    void ppu_save(uint16_t *address, uint8_t data);
};