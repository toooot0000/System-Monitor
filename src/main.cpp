#include <ncurses.h>

#include "ncurses_display.h"
#include "system.h"


int main() 
{
    System system;
    NCursesDisplay ncdisplay(system);
    ncdisplay.Start();
}