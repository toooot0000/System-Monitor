#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>
#include <vector>
#include <unordered_set>

#include "process.h"
#include "processor.h"

class System {
public:
    enum PROCESS_SORT_BASE
    {
        CPU = 0,
        MEM,
        PID,
        USR,
    };
    Processor& Cpu();                   // TODO: See src/system.cpp
    std::vector<Process>& Processes();  // TODO: See src/system.cpp
    float MemoryUtilization();          // TODO: See src/system.cpp
    long UpTime();                      // TODO: See src/system.cpp
    int TotalProcesses();               // TODO: See src/system.cpp
    int RunningProcesses();             // TODO: See src/system.cpp
    std::string Kernel();               // TODO: See src/system.cpp
    std::string OperatingSystem();      // TODO: See src/system.cpp
    inline void ChangeSortBase(PROCESS_SORT_BASE newBase) { processSortBase_ = newBase; };

private:
    Processor cpu_ = {};
    PROCESS_SORT_BASE processSortBase_ = PROCESS_SORT_BASE::CPU;
    std::vector<Process> processes_ = {};
    std::vector<int> pids_  = {};
};

#endif