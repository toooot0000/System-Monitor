#ifndef SYSTEM_PARSER_H
#define SYSTEM_PARSER_H

#include <dirent.h>
#include <unistd.h>

#include <fstream>
#include <regex>
#include <string>
#include <unordered_map>

namespace LinuxParser 
{
    // Paths
    const std::string kProcDirectory{ "/proc/" };
    const std::string kCmdlineFilename{ "/cmdline" };
    const std::string kCpuinfoFilename{ "/cpuinfo" };
    const std::string kStatusFilename{ "/status" };
    const std::string kStatFilename{ "/stat" };
    const std::string kUptimeFilename{ "/uptime" };
    const std::string kMeminfoFilename{ "/meminfo" };
    const std::string kVersionFilename{ "/version" };
    const std::string kOSPath{ "/etc/os-release" };
    const std::string kPasswordPath{ "/etc/passwd" };
    const long HERTZ = sysconf(_SC_CLK_TCK);

    // System
    float MemoryUtilization();
    long UpTime();
    long UpTimeDiff();
    std::vector<int> Pids();
    int TotalProcesses();
    int RunningProcesses();
    std::string OperatingSystem();
    std::string Kernel();
    std::unordered_map<int, std::string>& Users();

    // CPU
    enum CPUStates {
        kUser_ = 0,
        kNice_,
        kSystem_,
        kIdle_,
        kIOwait_,
        kIRQ_,
        kSoftIRQ_,
        kSteal_,
        kGuest_,
        kGuestNice_,
        SIZE
    };

    float CpuUtilization();
    long Jiffies();
    long ActiveJiffies();
    long JiffiesDifference();
    long ActiveJiffies(int pid);
    long IdleJiffies();

    // Processes
    std::string Command(int pid);
    std::string Ram(int pid);
    std::string Uid(int pid);
    std::string User(int pid);
    float CpuUtilization(int pid);
    long int StartTime(int pid);

};  // namespace LinuxParser

#endif