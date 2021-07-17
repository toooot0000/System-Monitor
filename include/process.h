#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
public:
    Process(){};
    inline int Pid() const {return _pid;}
    inline std::string User() const {return _user;}
    inline std::string Command() const {return _cmd;}
    inline float CpuUtilization() const {return _CPU;}
    inline std::string Ram() const {return _ram;}
    inline float RawRam() const {return _rawRam;}
    inline long int UpTime() const {return _uptime;}                       
    inline bool operator<(Process const& a) const {return a.CpuUtilization()<_CPU;}
    void updateData(int newPid);

    // TODO: Declare any necessary private members
private:
    int _pid{0};
    std::string _cmd{""};
    std::string _user{""};
    float _CPU{0.0};
    long _starttime{0};
    long _uptime{0};
    std::string _ram{""};
    float _rawRam{0.0};
};

#endif