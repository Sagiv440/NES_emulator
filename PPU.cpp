#include "PPU.h"

NES_PPU::NES_PPU(uint16_t top, uint16_t buttom)
{
	TOP_ADDRESS = top;
	BUTTOM_ADDRESS = buttom;

	screen_buffer = (uint8_t*)malloc(sizeof(uint8_t) * 256 * 240);

};
NES_PPU::~NES_PPU() { delete(screen_buffer); };

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
	uint8_t st;
	switch (address & 0x0007)  // using the last 3 bit of the address bus to select the correct registor
	{
	case PPUSTATUS:
		st = status;
		set_Status(V_Blank, 0); //Clear Vertical blank bit after reading the status register
		return st;
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

	status = (status & 0xE0) | (data & 0x1F); 
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

void NES_PPU::set_Status(uint8_t Status, uint8_t state)
{
	if (state == 0)
	{
		status = status & ~Status;
	}
	else
	{
		status = status | Status;
	}
}

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

void NES_PPU::Execute()  //only render the background "for now"
{
	if(x_axes == 0) {
		y_axes += 1;
		if(y_axes >= 240)
		{	
			set_Status(V_Blank, 1);
			y_axes = 0;	
		}
	}

	// Chack control register
	// Scroling
	// Rendering

	uint8_t pallet_x = (uint8_t)(0.125f * (float)x_axes); // Calculating the x position for the pallet corisponding with the pixel.
	uint8_t pallet_y = (uint8_t)(0.125f * (float)y_axes); // Calculating the y position for the pallet corisponding with the pixel.

	uint8_t attribute_x = (uint8_t)(0.03125f * (float)x_axes);
	uint8_t attribute_y = (uint8_t)(0.03125f * (float)y_axes);

	//reading nametable pallet form memory

	uint16_t NTaddress = 0x2000 + 0x0400 *(control & 0x03); // selete the Name table
	NTaddress += (pallet_x + (32 * pallet_y));	// set the pallet offset

	uint8_t Pattern = load_data(NTaddress);


	//reading attriblute form memory

	uint16_t ATaddress = 0x23C0 + 0x0400 *(control & 0x03); 	// selete the Attrebute table
	ATaddress += (attribute_x + (8 * attribute_y));				// set the Attrebute offset

	uint8_t Pallet = load_data(ATaddress);

	//load Pattern register

	uint16_t AP0 = ((y_axes%8) + (uint16_t)Pattern * 8); 			// calculate the address for Pattern 0 register
	uint16_t AP1 = ((y_axes%8) + (uint16_t)Pattern * 8) & 0x1000; 	// calculate the address for Pattern 1 register

	uint8_t P0 = load_data(AP0); // Load Pattern 0 register
	uint8_t P1 = load_data(AP1); // Load Pattern 1 register

	//get the pixel ID

	uint8_t pixel_id = 0;
	pixel_id = (P0 & (0x01 << (x_axes%8))) ? 1 : 0;
	pixel_id += (P1 & (0x01 << (x_axes%8))) ? 2 : 0;

	//get color set 

	uint8_t CPset_x = pallet_x%2;
	uint8_t CPset_y = pallet_y%2;
	uint8_t CP_pos = CPset_x + CPset_y * 2;
	uint8_t CP_set = 0;
	
	switch(CP_pos)
	{
		case 0:
			CP_set = Pallet & (0x03);
			break;
		case 1:
			CP_set = Pallet & (0x0C);
			CP_set = CP_set >> 2;
			break;
		case 2:
			CP_set = Pallet & (0x30);
			CP_set = CP_set >> 4;
			break;
		case 3:
			CP_set = Pallet & (0xC0);
			CP_set = CP_set >> 6;
			break;
	}

	//Get the pixel color

	uint16_t PCaddress = 0x3F00 + pixel_id + (4 * CP_set);
	//uint8_t pixel = load_data(PCaddress);
	uint8_t pixel = pixel_id;
	//Write the pixel to the buffer

	screen_buffer[x_axes + (256 * y_axes)] = pixel;

	x_axes += 1;

	
}

void NES_PPU::Scroling()
{

}
 
void NES_PPU::Rendering()
{
	
}