#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>
#include <assert.h>
#include <algorithm>

#include "process.h"
#include "processor.h"
#include "linux_parser.h"
#include "system.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

Processor& System::Cpu() 
{ 
    return cpu_;
}
    
vector<Process>& System::Processes() 
{
    auto newPids = LinuxParser::Pids();
    for(unsigned int i = 0; i!=newPids.size(); ++i)
    {
        if(i>=processes_.size())
            processes_.push_back(Process());
        processes_[i].updateData(newPids[i]);
    }
    while(newPids.size()<processes_.size()) processes_.pop_back();
    std::sort(processes_.begin(), processes_.end());
    return processes_; 
}

std::string System::Kernel() { return LinuxParser::Kernel(); }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

std::string System::OperatingSystem() { return LinuxParser::OperatingSystem(); }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }