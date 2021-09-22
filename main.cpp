#define OLC_PGE_APPLICATION
#include <iostream>
#include "build/CPU6502.h"
#include "olcPixelGameEngine.h"

memory* view;


void init_bus(BUS &bus)
{
    bus.Priferals = (module**)calloc(1, sizeof(module*));
    bus.count = 1;
	view = new memory();
	bus.Priferals[0] = view;
    ((memory*)bus.Priferals[0])->memory_stack[0xFFFC] = 0x00;
    ((memory*)bus.Priferals[0])->memory_stack[0xFFFD] = 0x06;

	((memory*)bus.Priferals[0])->load_memory_from_file("Snack_6502.txt");

    /*((memory*)bus.Priferals[0])->memory_stack[0x0600] = 0x20;
	((memory*)bus.Priferals[0])->memory_stack[0x0601] = 0x05;
	((memory*)bus.Priferals[0])->memory_stack[0x0602] = 0x06;
	((memory*)bus.Priferals[0])->memory_stack[0x0603] = 0x00;
	((memory*)bus.Priferals[0])->memory_stack[0x0604] = 0x00;
	((memory*)bus.Priferals[0])->memory_stack[0x0605] = 0xa2;
	((memory*)bus.Priferals[0])->memory_stack[0x0606] = 0x53;
	((memory*)bus.Priferals[0])->memory_stack[0x0607] = 0x60;
	((memory*)bus.Priferals[0])->memory_stack[0x0608] = 0x00;
	((memory*)bus.Priferals[0])->memory_stack[0x0609] = 0x00;
	((memory*)bus.Priferals[0])->memory_stack[0x060a] = 0x02;*/

}

int main(){
    BUS main_bus;
    CPU6502 cpu(&main_bus);
    init_bus(main_bus);
   /* cpu.Rest();
	cpu.status();
	((memory*)main_bus.Priferals[0])->Print_memory(0x01f0, 0x1ff);
	cpu.Execute();
	cpu.status();
	((memory*)main_bus.Priferals[0])->Print_memory(0x01f0, 0x1ff);
	cpu.Execute();
	cpu.status();
	((memory*)main_bus.Priferals[0])->Print_memory(0x01f0, 0x1ff);
	cpu.Execute();
	cpu.status();
	((memory*)main_bus.Priferals[0])->Print_memory(0x01f0, 0x1ff);
	cpu.Execute();
	cpu.status();*/

	((memory*)main_bus.Priferals[0])->Print_memory(0x0600, 0x750);
	
    std::cout << "CPU status\n";
    return 1;
}

// Override base class with your custom functionality
class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		// Name you application
		sAppName = "Example";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// called once per frame, draws random coloured pixels
		for (int x = 0; x < ScreenWidth(); x++)
			for (int y = 0; y < ScreenHeight(); y++)
				Draw(x, y, olc::Pixel(rand() % 256, rand() % 256, rand() % 256));
		return true;
	}
};
/*
int main()
{
	Example demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();
	return 0;
}*/