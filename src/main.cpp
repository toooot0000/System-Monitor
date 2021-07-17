// #include <pthread.h>
#include <ncurses.h>
// #include <iostream>
// #include <chrono>
// #include <thread>
// #include <mutex>

#include "ncurses_display.h"
#include "system.h"


// int curKeyCode = 100;
// std::mutex key_mutex;

// void processInput(NCursesDisplay &display[[maybe_unused]])
// {
    
//     while(1)
//     {
//         std::scoped_lock sl(key_mutex);
//         curKeyCode = display.CurrentPressedKey();
//     }
// }

int main() 
{
    System system;
    NCursesDisplay ncdisplay(system);

    // create a thread to read keyboard input
    // std::thread inputThread(processInput, std::ref(ncdisplay));

    // main loop for display information
    // while (1)
    // {
    ncdisplay.Start();
        // std::this_thread::sleep_for(std::chrono::seconds(1));
    // }

    // inputThread.join();
}