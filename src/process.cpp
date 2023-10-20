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


Process::Process(int pid) : pid_(pid) {
}

// TODO: Return this process's ID
int Process::Pid() const{ return  pid_;}

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const{
    long active_jiffies = LinuxParser::ActiveJiffies(pid_);
    long uptime = LinuxParser::UpTime();
    long seconds = uptime - LinuxParser::UpTime(pid_);
    if (seconds > 0) {
        return active_jiffies / seconds;
    }
    return 0.0;
}

// TODO: Return the command that generated this process
string Process::Command() const{ 
    return LinuxParser::Command(Pid());
}

// TODO: Return this process's memory utilization
string Process::Ram() const{ 
    return LinuxParser::Ram(Pid());
}

// TODO: Return the user (name) that generated this process
string Process::User() const{ 
    return LinuxParser::User(Pid());
}

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() const{     
    return LinuxParser::UpTime(Pid());
}

// TODO: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const {
    return CpuUtilization() < a.CpuUtilization();
}