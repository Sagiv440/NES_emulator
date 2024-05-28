 #include "NES.h"
 #include <SFML/Graphics.hpp>
/*
int main()
{
    std::cout << "Hallo World!"<< "\n";
    NES nes("donkey kong.nes");
	nes.On_start();
	while(1)
		nes.On_Update();

    return 0;
    Testing :)
}*/
#include <SFML/Graphics.hpp>

uint8_t Input()
{
    uint8_t buttons = 0;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
    {
        buttons |= 0x80;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
    {
        buttons |= 0x40;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
    {
        buttons |= 0x20;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
    {
        buttons |= 0x10;
    }
     if (sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
    {
        buttons |= 0x08;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num0))
    {
        buttons |= 0x04;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
    {
        buttons |= 0x02;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
    {
        buttons |= 0x01;
    }
    //buttons |= 0x04;
    return buttons;
}


int Game()
{
    uint8_t R ,G ,B ,A;

    sf::RenderWindow window(sf::VideoMode(256*4, 240*4), "SFML works!");
    sf::RectangleShape shape(sf::Vector2f(4.f ,4.f));
    shape.setFillColor(sf::Color::Green);
    NES nes("donkey kong.nes");
	nes.On_start();
    nes.set_controller(0,*Input);
    //nes.set_controller(1,*Input);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        nes.On_Update();
        window.clear();
        for(int y = 0 ; y < 240; y++)
            for(int x = 0; x < 256 ;x++){
                nes.get_screen(x, y, R, G, B, A);
                shape.setPosition(sf::Vector2f(x * 4.f ,y * 4.f));
                shape.setFillColor(sf::Color(R, G, B));
                window.draw(shape);
            }
        //window.draw(shape);
        window.display();
    }

    return 0;
}

int Debug()
{
    uint8_t color;
    uint8_t R ,G ,B ,A;
    uint8_t buffer[128*128] = {0x00};

    sf::RenderWindow window(sf::VideoMode(128*4, 128*4), "SFML works!");
    sf::RectangleShape shape(sf::Vector2f(4.f ,4.f));
    shape.setFillColor(sf::Color::Green);
    NES nes("color_test.nes");
	nes.On_start();
    nes.set_controller(0,*Input);
    //nes.set_controller(1,*Input);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        nes.ppu.Get_Pallets(buffer, 0);
        window.clear();
        for(int y = 0 ; y < 128; y++)
            for(int x = 0; x < 128 ;x++){
                color = buffer[x + y*128];
                nes.get_color(color*8, R, G, B, A);
                shape.setPosition(sf::Vector2f(x * 4.f ,y * 4.f));
                shape.setFillColor(sf::Color(R, G, B));
                window.draw(shape);
            }
        //window.draw(shape);
        window.display();
    }

    return 0;
}
int main()
{
    return Game();
}