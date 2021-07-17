#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

void Process::updateData(int newPid)
{
    _pid = newPid;

    _cmd = LinuxParser::Command(_pid);
    _user = LinuxParser::User(_pid);
    
    _CPU = LinuxParser::CpuUtilization(_pid);

    _starttime = LinuxParser::StartTime(_pid);
    _uptime = LinuxParser::UpTime() - _starttime;

    _rawRam = std::stol(LinuxParser::Ram(_pid))/1024.0;
    std::stringstream sstream;
    sstream.precision(2);
    sstream.setf(std::ios::fixed);
    sstream << _rawRam;
    _ram = sstream.str();

}