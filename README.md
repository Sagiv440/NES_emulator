NES_emulator
=====
Hello. 

This is my one take for an NES Emulator.   Inspired by the OLC Emulator Video Series  

All the ocumentation to create this project were sourced form [**nesdev.org**](https://www.nesdev.org/wiki/Nesdev_Wiki)  **Thank you :)**


# Descriptions
This Emulator is build using only standrd library.   Sow you can use what ever graphics and input library you want 

In the `main.cpp` file, I use the SFML library for handling the screen and IO.

# How To Use
1. After you download the project include the **NES.h** file to you project ` #include "NES.h"`
2. Create an NES variable `NES nes("file_name.nes");` And give it the name of the file you want to run as parameter `file_name.nes`
3. Call the **On_start()** to init the the emulator. `nes.On_start();`
4. Create an 8bit variable for the controller input "like a char or uint8_t" `uint8_t controller;` send it's pointer to the NES verable using the **set_controller()** function `nes.set_controller(0,*controller);` note the 0 means "controler 1" 
5. Creat a While loop and put the stop condition to when the game ends or the window close (your choise)
6. Inside the while loop, Call the NES **On_Update()** function to update the state of the nes every frame `nes.On_Update();`
7. Now the screen buffer in the NES variable is updated an can be printed to the screen. to do that first create 2 `for` loops, "one for x  and one for y"
```
for(int y = 0 ; y < 240; y++)
    for(int x = 0; x < 256 ;x++)
```
8.use the **get_screen()** to get the color information of the pixel. create 4 `uint8_t` varible for the R G B A value to store the pixel color and pass them to the functoin like so `nes.get_screen(x, y, R, G, B, A);`


# Template
This is a Template C++ code to get you started 
```
#include "NES.h"
//#include "your Graphics library of chose"

uint8_t R ,G ,B ,A;
uint8_t Controller;

NES nes("donkey kong.nes");
nes.On_start();
nes.set_controller(0.*Controller);

while(true){
  nes.On_Update();
  for(int y = 0 ; y < 240; y++)
    for(int x = 0; x < 256 ;x++){
        nes.get_screen(x, y, R, G, B, A);
        //print pixel to screen
    }
}

```

