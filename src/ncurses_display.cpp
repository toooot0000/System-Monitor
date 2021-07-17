#include <curses.h>
#include <chrono>
#include <string>
#include <thread>
#include <vector>
#include <iostream>

#include "format.h"
#include "ncurses_display.h"
#include "system.h"

using std::string;
using std::to_string;

// 50 bars uniformly displayed from 0 - 100 %
// 2% is one bar(|)
std::string NCursesDisplay::ProgressBar(float percent) {
    std::string result{ "0%" };
    int size{ 50 };
    float bars{ percent * size };

    for (int i{ 0 }; i < size; ++i) {
        result += i <= bars ? '|' : ' ';
    }

    string display{ to_string(percent * 100).substr(0, 4) };
    if (percent < 0.1 || percent == 1.0)
        display = " " + to_string(percent * 100).substr(0, 3);
    return result + " " + display + "/100%";
}

void NCursesDisplay::DisplaySystem(System& system, WINDOW* window) {
    int row{ 0 };
    mvwprintw(window, ++row, 2, ("OS: " + system.OperatingSystem()).c_str());
    mvwprintw(window, ++row, 2, ("Kernel: " + system.Kernel()).c_str());
    mvwprintw(window, ++row, 2, "CPU: ");
    wattron(window, COLOR_PAIR(1));
    mvwprintw(window, row, 10, "");
    wprintw(window, ProgressBar(system.Cpu().Utilization()).c_str());
    wattroff(window, COLOR_PAIR(1));
    mvwprintw(window, ++row, 2, "Memory: ");
    wattron(window, COLOR_PAIR(1));
    mvwprintw(window, row, 10, "");
    wprintw(window, ProgressBar(system.MemoryUtilization()).c_str());
    wattroff(window, COLOR_PAIR(1));
    mvwprintw(window, ++row, 2,
        ("Total Processes: " + to_string(system.TotalProcesses())).c_str());
    mvwprintw(
        window, ++row, 2,
        ("Running Processes: " + to_string(system.RunningProcesses())).c_str());
    mvwprintw(window, ++row, 2,
        ("Up Time: " + Format::ElapsedTime(system.UpTime())).c_str());
    wrefresh(window);
}

void NCursesDisplay::DisplayProcesses(std::vector<Process>& processes,
    WINDOW* window, int n) {
    int row{ 0 };
    int const pid_column{ 2 };
    int const user_column{ 9 };
    int const cpu_column{ 16 };
    int const ram_column{ 26 };
    int const time_column{ 35 };
    int const command_column{ 46 };
    wattron(window, COLOR_PAIR(2));
    mvwprintw(window, ++row, pid_column, "PID");
    mvwprintw(window, row, user_column, "USER");
    mvwprintw(window, row, cpu_column, "CPU[%%]");
    mvwprintw(window, row, ram_column, "RAM[MB]");
    mvwprintw(window, row, time_column, "TIME+");
    mvwprintw(window, row, command_column, "COMMAND");
    wattroff(window, COLOR_PAIR(2));
    for (int i = 0; i < n; ++i) {
        // Clear the line
        mvwprintw(window, ++row, pid_column, (string(window->_maxx - 2, ' ').c_str()));

        mvwprintw(window, row, pid_column, to_string(processes[i].Pid()).c_str());
        mvwprintw(window, row, user_column, processes[i].User().c_str());
        float cpu = processes[i].CpuUtilization() * 100;
        mvwprintw(window, row, cpu_column, to_string(cpu).substr(0, 4).c_str());
        mvwprintw(window, row, ram_column, processes[i].Ram().c_str());
        mvwprintw(window, row, time_column,
            Format::ElapsedTime(processes[i].UpTime()).c_str());
        mvwprintw(window, row, command_column,
            processes[i].Command().substr(0, window->_maxx - 46).c_str());
    }
}

void NCursesDisplay::DisplayInstructionBar(WINDOW* window)
{
    int row{1};
    int column{ 2 };
    mvwprintw(window, row, column, "Sorted by:");
    column+=10;
    mvwprintw(window, row, column, "F1-PID");
    column+=10;
    mvwprintw(window, row, column, "F2-CPU");
    column+=10;
    mvwprintw(window, row, column, "F3-Mem");
    column+=10;
    mvwprintw(window, row, column, "F4-User");
}


void NCursesDisplay::Update() {
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    box(system_window, 0, 0);
    box(process_window, 0, 0);
    DisplaySystem(system, system_window);
    DisplayProcesses(system.Processes(), process_window, PROCESS_NUM);
    DisplayInstructionBar(instr_window);
    wrefresh(system_window);
    wrefresh(process_window);
    wrefresh(instr_window);
    refresh();
}

void NCursesDisplay::Init()
{
    initscr();      // start ncurses
    noecho();       // do not print input values
    cbreak();       // terminate ncurses on ctrl + c
    start_color();  // enable color
    keypad(stdscr, true);

    int x_max{ getmaxx(stdscr) };
    system_window = newwin(9, x_max - 1, 0, 0);
    process_window = newwin(3 + PROCESS_NUM, x_max - 1, system_window->_maxy + 1, 0);
    instr_window = newwin(3, x_max - 1, process_window->_maxy + system_window->_maxy +1, 0);
    update_time = std::chrono::system_clock::now();

}

int NCursesDisplay::CurrentPressedKey() const
{
    return getch();
}

// main loop
void NCursesDisplay::MainLoop()
{
    while (1)
    {
        {
            std::scoped_lock sl(mutex_update_time);
            if (update_time <= std::chrono::system_clock::now()) // passed scheduled update time
            {
                update_time += update_interval;
                Update();
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(200)); //sleep for a little while
    }
}

void NCursesDisplay::ForceUpdate()
{
    std::scoped_lock sl(mutex_update_time);
    update_time = std::chrono::system_clock::now();
}

void NCursesDisplay::Start()
{
    // Create new thread for input
    std::thread input_thread(&NCursesDisplay::ProcessInput, this);
    // Call MainLoop();
    MainLoop();
    input_thread.join();
}

void NCursesDisplay::ProcessInput()
{
    while(1)
    {
        int tempKey = getch();
        bool validInput = true;
        switch (tempKey)
        {
        case KEY_F(1):
            if(tempKey == curKey) system.FlipSortReverseFlag();
            system.ChangeSortBase(System::PROCESS_SORT_BASE::PID);
            break;
        case KEY_F(2):
            if(tempKey == curKey) system.FlipSortReverseFlag();
            system.ChangeSortBase(System::PROCESS_SORT_BASE::CPU);
            break;
        case KEY_F(3):
            if(tempKey == curKey) system.FlipSortReverseFlag();
            system.ChangeSortBase(System::PROCESS_SORT_BASE::MEM);
            break;
        case KEY_F(4):
            if(tempKey == curKey) system.FlipSortReverseFlag();
            system.ChangeSortBase(System::PROCESS_SORT_BASE::USR);
            break;
        default:
            validInput = false;
        }
        curKey = tempKey;
        if (validInput) ForceUpdate();
    }
    
}