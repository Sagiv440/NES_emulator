#pragma once
#include "BUS.h"

#define PPUCTRL 0	 // Controlle ($2000) > write
#define	PPUMASK 1	 // Mask ($2001) > write
#define	PPUSTATUS 2	 // Status ($2002) < read
#define OAMADDR 3	 // OAM address ($2003) > write
#define OAMDATA 4	 // OAM data ($2004) <> read/write
#define PPUSCROLL 5	 // Scroll ($2005) >> write x2
#define PPUADDR 6	 // Address ($2006) >> write x2
#define PPUDATA 7	 // Data ($2007) <> read/write
#define OAMDMA 8	 // OAM DMA ($4014) > write


class NES_PPU : public module
{
	//register 0
	enum Ctrl
	{
		V_NMI	= 0x80,	//		when the bit 7 is clear "bit = 0" the PPU will not send and nmi interrupt to the cpu whenever a V-Blank occures.
		M_S		= 0x40,	//		PPU Master Slave_slect: (0: read backdrom form EXT pin: 1: output color on EXT pins)
		S_S		= 0x20,	//		Sprite size (0: 8x8 pixels; 1: 8x16 pixels)
		B_P		= 0x10,	//		Background pattern table address (0: $0000; 1: $1000)
		S_P		= 0x08,	//		Sprite pattern table address for 8x8 sprites (0: $0000; 1: $1000; ignored in 8x16 mode)
		VRAM_A	= 0x04,	//		VRAM address increment per CPU read / write of PPUDATA (0: add 1, going across; 1: add 32, going down)
		NTABLE_X		= 0x01,	//		Base nametable address (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
		NTABLE_Y		= 0x02	//		Base nametable address (0 = $2000; 1 = $2400; 2 = $2800; 3 = $2C00)
	};
	//register 1
	enum Mask
	{
		BLU		= 0x80, // Emphasize blue
		GRE		= 0x40, // Emphasize green (red on PAL/Dendy)
		RED		= 0x20, // Emphasize red (green on PAL/Dendy)
		SP_ON		= 0x10, // 1: Show sprites
		BG_ON		= 0x08, // 1: Show background
		MSP_ON		= 0x04, // 1: Show sprites in leftmost 8 pixels of screen, 0: Hide
		MBG_ON		= 0x02, // 1: Show background in leftmost 8 pixels of screen, 0: Hide
		GR_SCLE		= 0x01  // Greyscale (0: normal color, 1: produce a greyscale display)
	};
	//register 2
	enum Status
	{
		V_Blank = 0x80, // Vertical blank has started (0: not in vblank; 1: in vblank).
						// Set at dot 1 of line 241 (the line *after* the post - render
						// line); cleared after reading $2002 and at dot 1 of the
						// pre - render line.
		S_h		= 0x40, // Sprite 0 Hit.  Set when a nonzero pixel of sprite 0 overlaps
						// a nonzero background pixel; cleared at dot 1 of the pre - render
						// line.Used for raster timing.
		S_o		= 0x20, // Sprite overflow. The intent was for this flag to be set
						// whenever more than eight sprites appear on a scanline, but a
						// hardware bug causes the actual behavior to be more complicated
						// and generate false positives as well as false negatives; see
						// PPU sprite evaluation.This flag is set during sprite
						// evaluation and cleared at dot 1 (the second dot) of the
						// pre - render line.
	};
	enum contrl_lines {
		R_W = 0x80,
		IRQ = 0x40,
		NMI = 0x20,
		SYNC = 0x10,
	};

	union vram_address
	{
		struct{
			uint16_t coarse_x : 5;
			uint16_t coarse_y : 5;
			uint16_t nametable_x : 1;
			uint16_t nametable_y : 1;
			uint16_t fine_y : 3;
			uint16_t unused : 1;
		};
		uint16_t reg;
	};


	BUS* bus;
	uint8_t *CONTRL_BUS;
	uint8_t ready = 0x00;
	uint8_t control = 0x00;
	uint8_t mask = 0x00;
	uint8_t status = 0x00;
	uint8_t OAM_address = 0x00;

	vram_address tram_addr;
	vram_address vram_addr;
	uint8_t fine_x;
	uint8_t w_1 = 0x00;

	uint16_t attribute_address 	= 0x0000;

	uint8_t NTbyte;
	uint8_t ATbyte;
	uint8_t BG_low;
	uint8_t BG_hight;

	uint16_t BG_low_shift;
	uint16_t BG_hight_shift;
	uint16_t ATbyte_shift_low;
	uint16_t ATbyte_shift_high;
	uint8_t attribute_shift_registers 	= 0x0000;
	
	int16_t cycle_counter = 0;
	int16_t line_counter = 0;

	uint8_t ntable_test = 0;

	uint8_t x_axes = 0x00;
	uint8_t y_axes = 0xFF;
	uint8_t OAM_memory[0x100] = { 0x00 };
	
	uint8_t Pixel_Id_Buffer[256*240] = {0x00}; // Holld the color id for the final frame
	uint8_t* screen_buffer; // Holld the color id for the final frame
	
	void save_data(uint16_t address, uint8_t &data);
	uint8_t load_data(uint16_t address);

	void set_Status(uint8_t Status, uint8_t state);
	void addressing_mode()
	{
		if ((control & VRAM_A) != 0)
			vram_addr.coarse_y += 1;
		else
			vram_addr.coarse_x += 1;
	}

	void Print_Pallte();
	void inc_horizontal();
	void inc_vertical();
	void LoadShifters();
	void UpdateShifters();

	void TransferAddressX();
	void TransferAddressY();

	void BG_render();
	
public:
	void set_Ready();
	uint8_t Ready();
	void pattern(uint8_t &P);

	NES_PPU(uint16_t top = 0x4000, uint16_t buttom = 0x2000);
	~NES_PPU();

	void SpriteRandarer();

	void Rest();
	void Execute();



	void set_bus(BUS* main_bus){bus = main_bus;}
	void set_Interupt();
	uint8_t* get_screen_buffer(){return screen_buffer;}
	uint8_t send_data(uint16_t* address);
	void receive_data(uint16_t* address, uint8_t& data);
	void interrupt(uint8_t& contrl);

// Debuging funciton
	void Get_Pallets(uint8_t* buffer, uint8_t pallet)
	{
		uint16_t address = 0x0000;
		uint16_t pattern = (pallet&0x01)*0x1000;
		uint8_t data1 = 0x00;
		uint8_t data2 = 0x00;
		uint8_t bit = 0x00;
		for(uint8_t y = 0; y < 0x10; y++)
		{
			for(uint8_t x = 0; x < 0x10; x++)
			{
				address = pattern | x<<0x8 | y<<0x4;
				for(uint8_t h = 0; h < 8;h++)
				{
					
					
					data1 = load_data(address);

					data2 = load_data(address+8);

					for(int v = 0; v < 8;v++)
					{
						bit = data1 & 1 + (data2 & 1)*2;
						data1 = data1>>1;
						data2 = data2>>1;
						buffer[ x*8 + h + 128*(y*8 + v)] = bit;
					}
					address++;
				}
			}
		}
	}
};