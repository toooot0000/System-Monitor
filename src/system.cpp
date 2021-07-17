#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <assert.h>
#include <algorithm>
#include <unordered_map>

#include "process.h"
#include "processor.h"
#include "linux_parser.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

typedef bool (*Comp)(Process&, Process&);
const std::unordered_map<System::PROCESS_SORT_BASE, Comp> COMP_MAP
{
    {
        System::PROCESS_SORT_BASE::CPU,
        [](Process& p1, Process& p2)->bool {return p1.CpuUtilization() > p2.CpuUtilization();}
    },
    {
        System::PROCESS_SORT_BASE::MEM,
        [](Process& p1, Process& p2)->bool {return p1.RawRam() > p2.RawRam();}
    },
    {
        System::PROCESS_SORT_BASE::PID,
        [](Process& p1, Process& p2)->bool {return p1.Pid() > p2.Pid();}
    },
    {
        System::PROCESS_SORT_BASE::USR,
        [](Process& p1, Process& p2)->bool {return p1.User() > p2.User();}
    }

};

Processor& System::Cpu()
{
    return cpu_;
}

vector<Process>& System::Processes()
{
    auto newPids = LinuxParser::Pids();
    for (unsigned int i = 0; i != newPids.size(); ++i)
    {
        if (i >= processes_.size())
            processes_.push_back(Process());
        processes_[i].updateData(newPids[i]);
    }
    while (newPids.size() < processes_.size()) processes_.pop_back();
    std::sort(processes_.begin(), processes_.end(), COMP_MAP.at(processSortBase_));
    if(PROCESS_SORT_REVERSE) std::reverse(processes_.begin(), processes_.end());
    return processes_;
}

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }