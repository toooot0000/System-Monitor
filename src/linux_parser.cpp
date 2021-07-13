#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <regex>

#include <filesystem>

#include <assert.h>
#include <iostream>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
    string line;
    string key;
    string value;
    std::ifstream filestream(kOSPath);
    if (filestream.is_open()) {
        while (std::getline(filestream, line)) {
            std::replace(line.begin(), line.end(), ' ', '_');
            std::replace(line.begin(), line.end(), '=', ' ');
            std::replace(line.begin(), line.end(), '"', ' ');
            std::istringstream linestream(line);
            while (linestream >> key >> value) {
                if (key == "PRETTY_NAME") {
                    std::replace(value.begin(), value.end(), '_', ' ');
                    return value;
                }
            }
        }
    }
    return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
    string os, kernel, version;
    string line;
    std::ifstream stream(kProcDirectory + kVersionFilename);
    if (stream.is_open()) {
        std::getline(stream, line);
        std::istringstream linestream(line);
        linestream >> os >> version >> kernel;
    }
    return kernel;
}

// Get a list of Pids
vector<int> LinuxParser::Pids() {
    vector<int> pids;
    for (const auto& i : std::filesystem::directory_iterator(kProcDirectory))
    {
        if (!i.is_directory()) continue;
        auto filename = string(std::filesystem::path(i.path()).filename());
        if (std::all_of(filename.begin(), filename.end(), isdigit))
            pids.push_back(stoi(filename));
    }
    return pids;
}

namespace LinuxParser
{
    // Memory U tility
    std::unordered_map<std::string, long> memoVars
    {
      {"MemTotal", 0},
      {"MemFree", 0},
      {"Buffers", 0},
      {"Cached", 0},
      {"SReclamable", 0},
      {"Shmem", 0},
      {"SwapTotal", 0},
      {"SwapFree", 0}
    };
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization()
{
    std::ifstream stream(kProcDirectory + kMeminfoFilename);
    if (!stream.is_open()) throw "Memoinfo not opened!";
    string line;
    string key;
    long value;
    while (std::getline(stream, line))
    {
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream linestream(line);
        while (linestream >> key >> value)
        {
            if (memoVars.find(key) == memoVars.end()) continue;
            memoVars[key] = value;
        }
    }
    long totalUsedMemory = memoVars["MemTotal"] - memoVars["MemFree"];
    return static_cast<float>(totalUsedMemory) / memoVars["MemTotal"];
}


namespace LinuxParser
{
    long preUptime{0};
    long uptimeDiff{0};
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime()
{
    std::ifstream stream(kProcDirectory + kUptimeFilename);
    if (!stream.is_open()) throw "Uptime not opened";
    string line;
    std::getline(stream, line);
    std::istringstream linesteam(line);
    long upTime, idleTime;
    linesteam >> upTime >> idleTime;
    uptimeDiff = upTime - preUptime;
    preUptime = upTime;
    return upTime;
}

long LinuxParser::UpTimeDiff() {return uptimeDiff;}


// Declare these variables here to avoid multiple declerations 
namespace LinuxParser
{
    // CPU Utility
    long CPUJiffies[CPUStates::SIZE]{ 0 };
    long cpuPreIdle{ 0 };
    long cpuPreTotal{ 0 };
    long cpuJiffiesDiff{0};
}


// DONE: Read and return the number of jiffies for the system
inline long LinuxParser::Jiffies()
{
    long res = 0;
    for (int i = 0; i != CPUStates::SIZE; ++i)
        res += CPUJiffies[i];
    return res;
}

// DONE: Read and return the number of active jiffies for the system
inline long LinuxParser::ActiveJiffies()
{
    return Jiffies() - IdleJiffies();
}

// DONE: Read and return the number of idle jiffies for the system
inline long LinuxParser::IdleJiffies()
{
    return CPUJiffies[CPUStates::kIdle_] + CPUJiffies[CPUStates::kIOwait_];
}

long LinuxParser::JiffiesDifference()
{
    return cpuJiffiesDiff;
}

// DONE: Read and return CPU utilization
float LinuxParser::CpuUtilization()
{
    std::ifstream instream(kProcDirectory + kStatFilename);
    if (!instream.is_open()) throw "Stat not opened!";
    string line;
    getline(instream, line);
    std::istringstream sstream(line);

    string cpu;
    sstream >> cpu;
    for (auto i = 0; i != CPUStates::SIZE; ++i)
        sstream >> CPUJiffies[i];

    long total = Jiffies();
    long idle = IdleJiffies();
    long totald = total - cpuPreTotal;
    long idled = idle - cpuPreIdle;
    float res = totald ? static_cast<float>(totald - idled) / totald : 0.0;

    cpuJiffiesDiff = totald;
    cpuPreIdle = idle;
    cpuPreTotal = total;

    return res;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses()
{
    std::ifstream instream(kProcDirectory + kStatFilename);
    if (!instream.is_open()) throw "Stat not opened!";
    string line;
    while (getline(instream, line))
    {
        std::istringstream sstream(line);
        string key;
        sstream >> key;
        if (key == "processes")
        {
            int value;
            sstream >> value;
            return value;
        }
    }
    return 0;
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses()
{
    std::ifstream instream(kProcDirectory + kStatFilename);
    if (!instream.is_open()) throw "Stat not opened!";
    string line;
    while (getline(instream, line))
    {
        std::istringstream sstream(line);
        string key;
        sstream >> key;
        if (key == "procs_running")
        {
            int value;
            sstream >> value;
            return value;
        }
    }
    return 0;
}


namespace LinuxParser
{
    std::unordered_map<int, string> users = {};
}

std::unordered_map<int, string>& LinuxParser::Users()
{
    if (!users.empty()) return users;
    std::ifstream infilestream(kPasswordPath);
    if (!infilestream.is_open()) throw "/etc/passwd not opened!";
    string line;
    while (getline(infilestream, line))
    {
        std::replace(line.begin(), line.end(), ':', ' ');
        std::stringstream sstream(line);
        string name, x;
        int uid;
        sstream >> name >> x >> uid;
        users[uid] = name;
    }
    return users;
}

namespace LinuxParser
{
    // utime is 14th in [pid]/stat, stime is the 15th
    const int _PROCESS_STAT_UTIME = 13;
    // const int _PROCESS_STAT_STIME = 14;
    // const int _PROCESS_STAT_CUTIME = 15;
    // const int _PROCESS_STAT_CSTIME = 16;
    const int _PROCESS_STAT_STARTTIME = 21;
} // namespace LinuxParser

// use std::regex to split string;
std::vector<std::string> _split(std::string const _str, std::string const _reg)
{
    std::regex reg(_reg);
    return { std::sregex_token_iterator(_str.begin(), _str.end(), reg, -1), std::sregex_token_iterator() };
}

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid)
{
    std::ifstream infstream(kProcDirectory + to_string(pid) + kStatFilename);
    if (!infstream.is_open()) throw "Pid stat not opened!";
    string line;
    getline(infstream, line);
    auto tokens = _split(line, " ");
    if (tokens.size() < _PROCESS_STAT_STARTTIME + 1) throw "Sth wrong with _split!";
    // long stime = std::stol(tokens[_PROCESS_STAT_STIME]);
    long utime = std::stol(tokens[_PROCESS_STAT_UTIME]);
    return utime;
}

// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid)
{
    std::ifstream infstream(kProcDirectory + to_string(pid) + kCmdlineFilename);
    if (!infstream.is_open()) throw "Pid cmdline not opened!";
    string line;
    getline(infstream, line);
    return line;
}

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid)
{
    std::ifstream infstream(kProcDirectory + to_string(pid) + kStatusFilename);
    if (!infstream.is_open()) throw "Process status not opened! Pid:" + to_string(pid);
    string line;
    while (getline(infstream, line))
    {
        std::stringstream sstream(line);
        string key, value;
        sstream >> key >> value;
        if (key == "VmSize:")
            return value;
    }
    return "0";
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid)
{
    std::ifstream infstream(kProcDirectory + to_string(pid) + kStatusFilename);
    if (!infstream.is_open()) throw "Process status not opened! Pid:" + to_string(pid);
    string line;
    while (getline(infstream, line))
    {
        std::stringstream sstream(line);
        string key, value;
        sstream >> key >> value;
        if (key == "Uid:") return value;
    }
    return string();
}

// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid)
{
    auto& _user = Users();
    // string __uid = Uid(pid);
    // if(!__uid.size()) return "";
    int _uid = std::stoi(Uid(pid));
    if (_user.find(_uid) == _user.end()) throw "No uid: " + to_string(_uid);
    return _user[_uid];
}

// DONE: Read and return the StartTime of a process
long LinuxParser::StartTime(int pid)
{
    std::ifstream infstream(kProcDirectory + to_string(pid) + kStatFilename);
    if (!infstream.is_open()) throw "Pid stat not opened!";
    string line;
    getline(infstream, line);
    auto tokens = _split(line, " ");
    if (tokens.size() < _PROCESS_STAT_STARTTIME + 1) throw "Sth wrong with _split!";
    long starttime = std::stol(tokens[_PROCESS_STAT_STARTTIME]);
    return starttime / HERTZ;
}

namespace LinuxParser
{
    std::unordered_map<int, long> processPreJeffies;
}


float LinuxParser::CpuUtilization(int pid)
{
    long curActJif = ActiveJiffies(pid);
    float res = 0.0;
    if(processPreJeffies.find(pid) != processPreJeffies.end())
        res = static_cast<float> (curActJif-processPreJeffies[pid])/JiffiesDifference();
    processPreJeffies[pid] = curActJif;
    return res;
}
