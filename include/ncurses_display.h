#ifndef NCURSES_DISPLAY_H
#define NCURSES_DISPLAY_H

#include <unordered_map>
#include <curses.h>
#include <chrono>
#include <thread>
#include <mutex>

#include "process.h"
#include "system.h"

class NCursesDisplay
{
public:
    NCursesDisplay(System& _sys, int interval_sec = 1)
        :system(_sys), update_interval(std::chrono::seconds(interval_sec))
    {
        Init();
    };
    ~NCursesDisplay()
    {
        endwin();
        if (system_window) delete system_window;
        if (process_window) delete process_window;
    }

    void Start();
    void ForceUpdate();
    static std::string ProgressBar(float percent);

    int CurrentPressedKey() const;

private:
    const int PROCESS_NUM = 10;
    System& system;
    WINDOW* system_window{ nullptr };
    WINDOW* process_window{ nullptr };
    WINDOW* instr_window{ nullptr };
    std::chrono::time_point<std::chrono::system_clock> update_time;
    std::mutex mutex_update_time;
    std::chrono::seconds update_interval;

    int curKey{0};
    

    void MainLoop();
    void ProcessInput();
    void Update();
    void Init();
    void DisplaySystem(System& system, WINDOW* window);
    void DisplayProcesses(std::vector<Process>& processes, WINDOW* window, int n);
    void DisplayInstructionBar(WINDOW* window);
};  

#endif