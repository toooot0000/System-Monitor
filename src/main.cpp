#include <ncurses.h>
#include <iostream>
#include <chrono>
#include <thread>


#include "ncurses_display.h"
#include "system.h"

int main() {
    System system;
    NCursesDisplay ncdisplay(system);
    while (1)
    {
        ncdisplay.Display();
        int ch = ncdisplay.CurrentPressedKey();
        switch(ch)
        {
        case ERR:
            break;
        default:
            std::cout<< ch << std::endl;
            break;
        }
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
}