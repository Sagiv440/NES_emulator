#include <iostream>
#include <fstream>
#include <strstream>
#include "cartridge.h"
#include "PPU.h"
#include "CPU6502.h"


class NES
{
    CPU6502 cpu;
    NES_PPU ppu;
    cartridge *game;
    BUS     *main_bus;
    BUS     *ppu_bus;
    uint8_t* screen_buffer;
public:
    NES(const char* game_file)
    {
        main_bus = (BUS*)new(NES_BUS);                      // Set the NES main bus
        ppu_bus  = (BUS*)new(PPU_BUS);                      // Set the NES main bus
        cpu.set_bus(main_bus);                              // Assing bus to the CPU
        ppu.set_bus(ppu_bus);                               // Assing bus to the PPU
        game = new cartridge(game_file);                    // Loading the cartrige
        ((NES_BUS*)main_bus)->set_mapper(game->get_map());  // plugin the cartrige to the CPU bus
        ((PPU_BUS*) ppu_bus)->set_mapper(game->get_map());  // plugin the cartrige to the PPU bus
        ((NES_BUS*)main_bus)->set_ppu((module*)&ppu);       // Atach the PPU to the cpu buss
    }

    std::string des_to_hex(uint8_t number)
	{
		std::string hex;
		std::string list = "0123456789ABCDEF";
		hex += list[number >> 4];
		hex += list[(number & 0x0f)];
		return hex;
	}

    void run()
    {
        screen_buffer = ppu.get_screen_buffer();
        cpu.Rest();
        /*while(1)
        {
            cpu.Execute();
        }*/
        for(int i = 0; i < 10000000; i++){
            cpu.Execute();
            ppu.Execute();
            ppu.Execute();
            ppu.Execute();
        }
        for(int y = 0; y < 240; y++){
            for(int x = 0; x < 256; x++){
                if(screen_buffer[x + (256 * y)] == 1)
                    std::cout << "@";
                else
                    std::cout << " ";
            }
            std::cout << "\n";
        }
    }
};



int main()
{
    std::cout << "Hallo World!"<< "\n";
    NES nes("donkey kong.nes");
    nes.run();
    return 0;
}