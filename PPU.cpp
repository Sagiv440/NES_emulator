#include "PPU.h"

NES_PPU::NES_PPU(uint16_t top = 0x4000, uint16_t buttom = 0x2000, BUS* module_bus = NULL)
{
	TOP_ADDRESS = top;
	BUTTOM_ADDRESS = buttom;
	bus = module_bus;
};
NES_PPU::~NES_PPU() {};

void NES_PPU::save_data(uint16_t address, uint8_t &data)
{
	bus->ADDRESS_BUS = address;
	bus->DATA_BUS = data;
	bus->save();
}
uint8_t NES_PPU::load_data(uint16_t address)
{
	bus->ADDRESS_BUS = address;
	bus->load();
	return bus->DATA_BUS;
}

uint8_t NES_PPU::send_data(uint16_t& address)
{
	switch (address & 0x0007)  // using the last 3 bit of the address bus to select the correct registor
	{
	case PPUSTATUS:
		return status;
		break;

	case OAMDATA:
		return OAM_memory[OAM_address];
		break;

	case PPUDATA:
		return load_data(D_address);
		break;

	default:
		return 0x00;
		break;
	}
	
	// Status ($2002) < read
	// OAM data ($2004) <> read/write
	// Data ($2007) <> read/write

};
void NES_PPU::receive_data(uint16_t& address, uint8_t& data)
{
	switch (address & 0x0007)  // using the last 3 bit of the address bus to select the correct registor
	{
	case PPUCTRL:
		control = data;
		w_1 = 0;
		break;

	case PPUMASK:
		mask = data;
		w_1 = 0;
		break;

	case OAMADDR:
		OAM_address = data;
		w_1 = 0;
		break;

	case OAMDATA:
		OAM_memory[OAM_address] = data;
		w_1 = 0;
		break;

	case PPUSCROLL:
		// Scrooling is on a brack
		if (w_1 == 0)
		{
			//Change the cam_X position
			X_cam = data;
			w_1 = 1;
		}
		else
		{
			//Change the cam_Y position
			Y_cam = data;
			w_1 = 0;
		}
		break;
		break;

	case PPUADDR:
		if (w_1 == 0)
		{
			D_address = (uint16_t)data << 8;
			D_address &= 0xff00;
			w_1 = 1;
		}
		else
		{
			D_address = D_address | data;
			w_1 = 0;
		}
		break;

	case PPUDATA:
		save_data(D_address, data);
		w_1 = 0;
		break;

	case OAMDMA:

		//Activate_DMA_protocol

		w_1 = 0;
		break;

	default:
		w_1 = 0;
		break;
	}
	// Controlle ($2000) > write
	// Mask ($2001) > write
	// OAM address ($2003) > write
	// OAM data ($2004) <> read/write
	// Scroll ($2005) >> write x2
	// Address ($2006) >> write x2
	// Data ($2007) <> read/write
	// OAM DMA ($4014) > write


}
void NES_PPU::interrupt(uint8_t& contrl) {};

void NES_PPU::pattern(uint8_t &P)
{
	uint8_t pixel, p0, p1;
	for (uint8_t y; y < 8; y++) {
		p0 = load_data((uint16_t)P * 8 + y);
		p1 = load_data((uint16_t)P * 8 + y + 0x0800);
		for (uint8_t x; x < 8; x++) {
			pixel =   (p0 & (0xfe << x)) != 0;
			pixel += ((p1 & (0xfe << x)) != 0)? 2:0 ;

		}
	}
}

void NES_PPU::BG_render()
{

}

void NES_PPU::FG_render()
{

}

void NES_PPU::Execute() 
{
	// Chack control register
	// Scroling
	// Rendering
}

void NES_PPU::Scroling()
{

}
 
void NES_PPU::Rendering()
{
	
}