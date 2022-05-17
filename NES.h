#include <iostream>
#include <fstream>
#include <strstream>
#include "cartridge.h"
#include "PPU.h"
#include "CPU6502.h"
#include "IO_Ports.h"


class NES
{
public:
	CPU6502 cpu;
	NES_PPU ppu;
	IO_Ports ports;
	cartridge *game;
	BUS     *main_bus;
	BUS     *ppu_bus;
	uint8_t* screen_buffer;
	double clock_coutner = 0;

	struct color {
		uint8_t red;
		uint8_t green;
		uint8_t blue;
		uint8_t alfa;
	};

	color color_list[64]
	{
		{ 84 , 84 , 84, 255},	{  0,  30, 116, 255},  {  8,  16, 144, 255},  { 48,   0, 136, 255},  { 68,   0, 100, 255},  { 92,   0,  48, 255},	{ 84,   4,   0, 255},  { 60,  24,   0, 255},  { 32,  42,   0, 255},  {  8,  58,   0, 255},  {  0,  64,   0, 255},  {  0,  60,   0, 255},  {  0,  50,  60, 255},  {  0,   0,   0, 255}, {  0,  0,  0,  255}, {  0,  0,  0, 255},
		{152 ,150 ,152, 255},	{  8,  76, 196, 255},  { 48,  50, 236, 255},  { 92,  30, 228, 255},  {136,  20, 176, 255},  {160,  20, 100, 255},	{152,  34,  32, 255},  {120,  60,   0, 255},  { 84,  90,   0, 255},  { 40, 114,   0, 255},  {  8, 124,   0, 255},  {  0, 118,  40, 255},  {  0, 102, 120, 255},  {  0,   0,   0, 255}, {  0,  0,  0,  255}, {  0,  0,  0, 255},
		{236 ,238 ,236, 255},	{ 76, 154, 236, 255},  {120, 124, 236, 255},  {176,  98, 236, 255},  {228,  84, 236, 255},  {236,  88, 180, 255},	{236, 106, 100, 255},  {212, 136,  32, 255},  {160, 170,   0, 255},  {116, 196,   0, 255},  { 76, 208,  32, 255},  { 56, 204, 108, 255},  { 56, 180, 204, 255},  { 60,  60,  60, 255}, {  0,  0,  0,  255}, {  0,  0,  0, 255},
		{236 ,238 ,236, 255},	{168, 204, 236, 255},  {188, 188, 236, 255},  {212, 178, 236, 255},  {236, 174, 236, 255},  {236, 174, 212, 255},	{236, 180, 176, 255},  {228, 196, 144, 255},  {204, 210, 120, 255},  {180, 222, 120, 255},  {168, 226, 144, 255},  {152, 226, 180, 255},  {160, 214, 228, 255},  {160, 162, 160, 255}, {  0,  0,  0,  255}, {  0,  0,  0, 255}
	};

public:
	void get_color(uint8_t pixel_id, uint8_t &R, uint8_t &G, uint8_t &B, uint8_t &A)
	{
		R = color_list[pixel_id].red;
		G = color_list[pixel_id].green;
		B = color_list[pixel_id].blue;
		A = color_list[pixel_id].alfa;
	}
	void get_screen(uint8_t x, uint8_t y, uint8_t &R, uint8_t &G, uint8_t &B, uint8_t &A)
	{
		uint8_t pixel_id = screen_buffer[x + (256 * y)];
		R = color_list[pixel_id].red;
		G = color_list[pixel_id].green;
		B = color_list[pixel_id].blue;
		A = color_list[pixel_id].alfa;
	}

	NES(const char* game_file)
	{
		main_bus = (BUS*)new(NES_BUS);                      // Set the NES main bus
		ppu_bus = (BUS*)new(PPU_BUS);                      // Set the NES main bus
		cpu.set_bus(main_bus);                              // Assing bus to the CPU
		ppu.set_bus(ppu_bus);                               // Assing bus to the PPU
		ports.set_cpu_bus(main_bus);
		ports.set_ppu(&ppu);
		game = new cartridge(game_file);                    // Loading the cartrige
		((NES_BUS*)main_bus)->set_mapper(game->get_map());  // plugin the cartrige to the CPU bus
		((PPU_BUS*)ppu_bus)->set_mapper(game->get_map());  // plugin the cartrige to the PPU bus
		((NES_BUS*)main_bus)->set_ppu((module*)&ppu);       // Atach the PPU to the cpu buss
		((NES_BUS*)main_bus)->set_IO((module*)&ports);       // Atach the IO, DMA, APU to the cpu buss

	}

	std::string des_to_hex(uint8_t number)
	{
		std::string hex;
		std::string list = "0123456789ABCDEF";
		hex += list[number >> 4];
		hex += list[(number & 0x0f)];
		return hex;
	}

	void set_controller(uint8_t con ,uint8_t(*but)(void)){
		ports.set_io(con, but);
	}
	
	void run()
	{
		screen_buffer = ppu.get_screen_buffer();
		cpu.Rest();
		for(int i = 0; i < 60 ;i++){
			while(!ppu.Ready())
			{
				ppu.Execute();
				ppu.Execute();
				ppu.Execute();
				cpu.Execute();
			}
			ppu.set_Ready();
		}
		for(int y = 0; y < 240; y++){
			for(int x = 0; x < 256; x++){
				std::cout << screen_buffer[x + (256 * y)];
			}
			std::cout << "\n";
		}
	}
	void On_start()
	{
		screen_buffer = ppu.get_screen_buffer();
		cpu.Rest();
	}
	void On_Update()
	{
		while(!ppu.Ready())
		{
			ppu.Execute();
			ppu.Execute();
			ppu.Execute();
			cpu.Execute();
			clock_coutner++;
		}
		ppu.set_Ready();
	}
};
