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
	//bus->ADDRESS_BUS = address;
	bus->DATA_BUS = data;
	bus->save(address);
}
uint8_t NES_PPU::load_data(uint16_t address)
{
	//bus->ADDRESS_BUS = address;
	bus->load(address);
	return bus->DATA_BUS;
}

uint8_t NES_PPU::send_data(uint16_t* address)
{
	uint8_t st;
	switch (*address & 0x0007)  // using the last 3 bit of the address bus to select the correct registor
	{
	case PPUSTATUS:
		st = status;
		w_1 = 0;
		set_Status(V_Blank, 0); //Clear Vertical blank bit after reading the status register
		return st;
		break;

	case OAMDATA:
		return OAM_memory[OAM_address];
		break;

	case PPUDATA:
		st = load_data(vram_addr.reg);
		addressing_mode();
		return st;
		break;

	default:
		return 0x00;
		break;
	}
	
	// Status ($2002) < read
	// OAM data ($2004) <> read/write
	// Data ($2007) <> read/write

};
void NES_PPU::receive_data(uint16_t *address, uint8_t& data)
{
	switch (*address & 0x0007)  // using the last 3 bit of the address bus to select the correct registor
	{
	case PPUCTRL:
		control = data;
		tram_addr.nametable_x = (control & Ctrl::NTABLE_X) > 0;
		tram_addr.nametable_y = (control & Ctrl::NTABLE_Y) > 0;
		break;

	case PPUMASK:
		mask = data;
		break;

	case OAMADDR:
		OAM_address = data;
		break;

	case OAMDATA:
		OAM_memory[OAM_address] = data;
		OAM_address++;
		break;

	case PPUSCROLL:
		// Scrooling is on a brack
		if (w_1 == 0)
		{
			//Change the cam_X position
			fine_x = data & 0x07;
			tram_addr.coarse_x = data >> 3;
			w_1 = 1;
		}
		else
		{
			//Change the cam_Y position
			tram_addr.fine_y = data & 0x07;
			tram_addr.coarse_y = data >> 3;
			w_1 = 0;
		}
		break;

	case PPUADDR:
		if (w_1 == 0)
		{
			tram_addr.reg = (uint16_t)((data & 0x3F) << 8) | (tram_addr.reg & 0x00ff);
			w_1 = 1;
		}
		else
		{
			tram_addr.reg = (tram_addr.reg & 0xff00) | (uint16_t)data ;
			vram_addr.reg = tram_addr.reg;
			w_1 = 0;
		}
		break;

	case PPUDATA:
		save_data(vram_addr.reg, data);
		addressing_mode();
		break;

	default:
		std::cout << "ERROR::PPU::WRITE_DATA WRITE DATA TO A NONE VALID RGISTER " << std::hex << (int)(*address & 0x0007) << "\n";
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

uint8_t NES_PPU::Ready()
{
	return ready;
}
void NES_PPU::set_Ready()
{
	ready = 0;
}

void NES_PPU::set_Interupt()
{
	if((control & V_NMI) != 0) //Set an Interupte if NMI is enable
	{	
		*INTERRUPTS |= NMI;	
	}
}

void NES_PPU::pattern(uint8_t &P)
{
	uint8_t pixel, p0, p1;
	for (uint8_t y; y < 8; y++) {
		p0 = load_data((uint16_t)P * 8 + y);
		p1 = load_data((uint16_t)P * 8 + y + 0x0800);
		for (uint8_t x; x < 8; x++) {
			pixel =   (p0 & (0x80 >> x)) != 0;
			pixel += ((p1 & (0x80 >> x)) != 0)? 2:0 ;
		}
	}
}

/*
uint8_t fine_x;
uint8_t fine_y;
uint8_t x_pattorn;
uint8_t y_pattorn;

uint16_t NTbyte;
uint16_t ATbyte;
uint8_t BG_low;
uint8_t BG_hight;
*/
void NES_PPU::inc_horizontal(){
	if(((mask&BG_ON) == BG_ON) || ((mask&SP_ON) == SP_ON)){
		if(vram_addr.coarse_x == 31){
			vram_addr.coarse_x = 0;
			vram_addr.nametable_x = ~vram_addr.nametable_x;
		}else{
			vram_addr.reg += 1;
		}
	}
}

void NES_PPU::inc_vertical(){
	if(((mask&BG_ON) == BG_ON) || ((mask&SP_ON) == SP_ON)){
		if(vram_addr.fine_y < 7){
			vram_addr.fine_y += 1;
		}else{
			vram_addr.fine_y = 0;
			if(vram_addr.coarse_y == 29){

				vram_addr.coarse_y = 0;
				vram_addr.nametable_y = ~vram_addr.nametable_y;
			}else if(vram_addr.coarse_y == 31){
				vram_addr.coarse_y = 0;
			}else{
				vram_addr.coarse_y += 1;
			}
		}
	}
}

void NES_PPU::TransferAddressX(){
	if(((mask&BG_ON) == BG_ON) || ((mask&SP_ON) == SP_ON)){
			vram_addr.nametable_x = tram_addr.nametable_x;
			vram_addr.coarse_x    = tram_addr.coarse_x;
	}
}

void NES_PPU::TransferAddressY(){
	if(((mask&BG_ON) == BG_ON) || ((mask&SP_ON) == SP_ON)){
			vram_addr.fine_y      = tram_addr.fine_y;
			vram_addr.nametable_y = tram_addr.nametable_y;
			vram_addr.coarse_y    = tram_addr.coarse_y;
	}
}

void NES_PPU::LoadShifters(){
	BG_low_shift 	= (BG_low_shift 	& 0xFF00) | BG_low;
	BG_hight_shift 	= (BG_hight_shift 	& 0xFF00) | BG_hight;

	ATbyte_shift_low  	= (ATbyte_shift_low 	& 0xFF00) | ((ATbyte & 0b01) ? 0xFF : 0x00);
	ATbyte_shift_high  	= (ATbyte_shift_high 	& 0xFF00) | ((ATbyte & 0b10) ? 0xFF : 0x00);
}

void NES_PPU::UpdateShifters(){
	if((mask&BG_ON) == BG_ON){
		BG_low_shift	  <<= 1;
		BG_hight_shift	  <<= 1;	
		ATbyte_shift_low  <<= 1;
		ATbyte_shift_high <<= 1;
	}
}
 
void NES_PPU::Execute()
{	
	if(line_counter >= -1 && line_counter < 240){
		if(line_counter == 0 && cycle_counter == 0)
			cycle_counter = 1;

		if(line_counter == -1 && cycle_counter == 0)
			set_Status(V_Blank, 0);
		
		if((cycle_counter >= 2 && cycle_counter < 258) || (cycle_counter >= 321 && cycle_counter < 338)){
			uint16_t bg_address = 0x0000;
			UpdateShifters();
			switch((cycle_counter-1)%8)
			{
				case 0:
					//Load Nametable byte 
					LoadShifters();

					NTbyte = load_data(0x2000 | (vram_addr.reg & 0x0FFF));
					break;
				case 2:
					//Load Attribute byte
					ATbyte = load_data(0x23C0 | (vram_addr.reg & 0x0C00) | ((vram_addr.reg >> 4) & 0x38) | ((vram_addr.reg >> 2) & 0x07));
					if (vram_addr.coarse_y & 0x02) ATbyte >>= 4;
					if (vram_addr.coarse_x & 0x02) ATbyte >>= 2;
					ATbyte &= 0x03;
					break;
				case 4:
					//Load low Backgrond byte
					BG_low = load_data(((control & Ctrl::B_P) << 8)
										+ ((uint16_t)NTbyte << 4) 
					                    + (vram_addr.fine_y));
					break;
				case 6:
					//Load high Backgrond byte
					BG_hight = load_data(((control & Ctrl::B_P) << 8)
										+ ((uint16_t)NTbyte << 4) 
					                    + (vram_addr.fine_y) + 8);
					break;
				case 7:
					//incress hrizontal
					inc_horizontal();
					break;
			}
		}
		if(cycle_counter == 256){
			inc_vertical();
		}
		if(cycle_counter == 257){
			LoadShifters();
			TransferAddressX();
		}
		if(cycle_counter == 338 || cycle_counter == 340){
			NTbyte = load_data(0x2000 | (vram_addr.reg & 0x0FFF));
		}
		if(line_counter == -1 && cycle_counter >= 280 && cycle_counter < 305){
			TransferAddressY();
		}
	}
	if(line_counter >= 241 && line_counter < 261){
		if(line_counter == 241 && cycle_counter == 1){
			set_Status(V_Blank, 1);
			set_Interupt();
			ready = 1;
		}
	}
	/*if(line_counter == 241 && cycle_counter == 1){
			set_Status(V_Blank, 1);
			set_Interupt();
			ready = 1;
	}*/
	//Rendering

	uint8_t pixel, palette;
	uint16_t palette_address;

	if((mask&BG_ON) == BG_ON){
		
		uint16_t bit_mux = 0x8000 >> fine_x;

		uint8_t px0 = (BG_low_shift & bit_mux) > 0;
		uint8_t px1 = (BG_hight_shift & bit_mux) > 0;
		pixel = (px1 << 1) | px0 ;

		uint8_t pl0 = (ATbyte_shift_low & bit_mux) != 0? 1:0;
		uint8_t pl1 = (ATbyte_shift_high & bit_mux) != 0? 2:0;
		palette = pl0 + pl1;
	}

	palette_address = 0x3F00 | (palette << 2) | pixel;
	pixel = load_data(palette_address);


	int x = cycle_counter-1;
	int y = line_counter;
	//std::cout << x << " " << y << "\n";
	if(x+ 256*y < 256*240)
		screen_buffer[x+ 256*y] = pixel;

	cycle_counter += 1;
	if (cycle_counter >= 341)
	{
		cycle_counter = 0;
		line_counter++;
		if (line_counter >= 261)
		{
			line_counter = -1;
			//frame_complete = true;
		}
	}

}