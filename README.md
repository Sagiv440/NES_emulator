NES_emulator
=====
Hello. 

This is my one Take of an NES Emulator.   Insperd by the OLC Emulator Video Series  

All the documatation to create this project were sorced form [**nesdev.org**](https://www.nesdev.org/wiki/Nesdev_Wiki)  **Thank you :)**


# Descriptions
This Emulator is buid using only standrd library.   Sow you can use what ever graphics and input library you want 

In the main file I use the SFML library for handling the printing to screen and IO input

# How to use
1. After you down load the project include the **NES.h** file to you project ` #include "NES.h"`
2. Create an NES Variable `NES nes("file_name.nes");` in the bracet put the name of the game you want to run `file_name.nes`
3. Call the **On_start()** to init the the emulator. `nes.On_start();`
4. Creat an 8bit variable for the controller input "like a char or uint8_t" `uint8_t controller;` send it's pointer to the NES verable using the **set_controller** function `nes.set_controller(0,*controller);` note the 0 means "controler 1" 
5. Creat a While loop and put the stop condition to when the game ends or the window close (your choise)
6. Inside the while loop the NES **On_Update()** function to update the state of the nes every frame `nes.On_Update();`
7. Now the screen buffer in the nes variable is updated an can be printed to the screen. to do that first create two for loops, "one for x  and one for y"
```
for(int y = 0 ; y < 240; y++)
    for(int x = 0; x < 256 ;x++)
```
8.use the **get_screen()** to get the color information of the pixel create a 4 uint8_t varible for the R G B A value to be stored and give the to the functoin like sow `nes.get_screen(x, y, R, G, B, A);`


# Example
This is a Template code to get you started 
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


