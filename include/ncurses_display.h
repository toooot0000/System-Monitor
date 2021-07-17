#ifndef NCURSES_DISPLAY_H
#define NCURSES_DISPLAY_H

#include <curses.h>

#include "process.h"
#include "system.h"

class NCursesDisplay 
{
public:
    NCursesDisplay(System &_sys):system(_sys)
    {
        Init();
    };
    ~NCursesDisplay()
    {
        endwin();
        if(system_window) delete system_window;
        if(process_window) delete process_window;
    }
    
    void Display();
    static std::string ProgressBar(float percent);

    int CurrentPressedKey();

private:
    const int PROCESS_NUM = 10;
    System &system;
    WINDOW *system_window{nullptr};
    WINDOW *process_window{nullptr};

    void Init();
    void ProcessInput();
    void DisplaySystem(System& system, WINDOW* window);
    void DisplayProcesses(std::vector<Process>& processes, WINDOW* window, int n);
};  // namespace NCursesDisplay

#endif