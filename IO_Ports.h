#pragma once
#include "BUS.h"

class Controller
{
    enum buttons
    {
        RIGHT,
        LEFT,
        DOWN,
        UP,
        START,
        SELECT,
        B,
        A
    };

    uint8_t io_set = 0;
    uint8_t(*Input)(void);

    uint8_t buttons = 0;
    uint8_t m_strobe;

public:
    Controller(){};
    ~Controller(){};
    void set_io(uint8_t(*input)(void))
    {
        io_set = 1;
        this->Input = input;
    }

    void set_letch(uint8_t set)
    {
        if(io_set)
            buttons = Input();
    }

    uint8_t read()
    {
        uint8_t data = (buttons & 0x80) > 0;
        buttons <<= 1;
        return data;
    }
};


class IO_Ports : public module
{
    BUS *cpu_bus;
    NES_PPU *ppu;
    uint16_t address;

    Controller control_1;
    Controller control_2;
  

public:
    void set_cpu_bus(BUS* bus){ cpu_bus = bus; };
    void set_ppu(NES_PPU* pu){ ppu = pu; };
    void set_io(uint8_t controller, uint8_t(*button)(void))
    {    
        if(controller == 0) {   control_1.set_io(button);   }
        else                {   control_2.set_io(button);   };
    }
    

    uint8_t send_data(uint16_t* address)
    {
        uint8_t reg = (uint8_t)(*address & 0x001f);
        if(reg == 0x16)
        {
            return control_1.read();
        }
        if(reg == 0x17)
        {
            return control_2.read();
        }
        return 0;
    };

	void receive_data(uint16_t* address, uint8_t& data)
    {
        uint8_t reg = (uint8_t)(*address & 0x001f);

        if(reg == 0x14)
        {
            DMA_trasfer(data);
        }
         if(reg == 0x16)
        {
            control_1.set_letch(data);
        }
         if(reg == 0x17)
        {
            control_2.set_letch(data);
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