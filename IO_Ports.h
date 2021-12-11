#pragma once
#include "BUS.h"

class IO_Ports : public module
{
    BUS *cpu_bus;
    NES_PPU *ppu;
    uint16_t address;

public:
    void set_cpu_bus(BUS* bus){ cpu_bus = bus; };
    void set_ppu(NES_PPU* pu){ ppu = pu; };

    uint8_t send_data(uint16_t* address)
    {
        uint8_t reg = (uint8_t)(*address & 0x001f);
        return 0;
    };
	void receive_data(uint16_t* address, uint8_t& data)
    {
        uint8_t reg = (uint8_t)(*address & 0x001f);

        if(reg == 0x14)
        {
            DMA_trasfer(data);
        }
        
    };
	void interrupt(uint8_t& contrl){};

    void DMA_trasfer(uint8_t &data)
    {
        uint16_t ppuSet = 0x0003;
        uint16_t OAM_address = 0x0000;
        uint8_t start = 0x00;
        ppu->receive_data(&ppuSet, start);

        OAM_address = (uint16_t)data << 8;
        ppuSet = 0x0004;
        for(int i = 0; i < 256; i++)
        {
            cpu_bus->load(OAM_address);
            ppu->receive_data(&ppuSet, cpu_bus->DATA_BUS);
            OAM_address++;
        }
    }
};