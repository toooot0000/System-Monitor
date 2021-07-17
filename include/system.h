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
    bool PROCESS_SORT_REVERSE{ false };
    Processor& Cpu();
    std::vector<Process>& Processes();
    float MemoryUtilization();
    long UpTime();
    int TotalProcesses();
    int RunningProcesses();
    std::string Kernel();
    std::string OperatingSystem();
    inline void ChangeSortBase(PROCESS_SORT_BASE newBase) { processSortBase_ = newBase; };
    inline void FlipSortReverseFlag() { PROCESS_SORT_REVERSE = !PROCESS_SORT_REVERSE; }

private:
    Processor cpu_ = {};
    PROCESS_SORT_BASE processSortBase_ = PROCESS_SORT_BASE::CPU;
    std::vector<Process> processes_ = {};
    std::vector<int> pids_ = {};
};

#endif