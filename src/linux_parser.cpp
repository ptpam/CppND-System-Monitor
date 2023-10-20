#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

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

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line, key;
  float total_memory = 0; 
  float free_memory = 0;

  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "MemTotal:"){
        linestream >> total_memory;
      }else if (key == "MemFree:"){
        linestream >> free_memory;
        break; // exit the loop
      }
    }
  }

  return (total_memory - free_memory) / total_memory;

}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  long uptime = 0;
  string line;
  std::ifstream filestream(kProcDirectory+kUptimeFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
 }

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  long jiffies = 0;
  vector<string> cpu_utilization = CpuUtilization();
  if (cpu_utilization.size() >= 10) {
    jiffies = stol(cpu_utilization[CPUStates::kUser_]) +
              stol(cpu_utilization[CPUStates::kNice_]) +
              stol(cpu_utilization[CPUStates::kSystem_]) +
              stol(cpu_utilization[CPUStates::kIRQ_]) +
              stol(cpu_utilization[CPUStates::kSoftIRQ_]) +
              stol(cpu_utilization[CPUStates::kSteal_]) +
              stol(cpu_utilization[CPUStates::kGuest_]) +
              stol(cpu_utilization[CPUStates::kGuestNice_]);
  }
  return jiffies;
}

// TODO: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string line, value;
  long utime, stime, cutime, cstime;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 1; i <= 17; i++) {
      linestream >> value;
      if (i == 14) utime = stol(value);
      if (i == 15) stime = stol(value);
      if (i == 16) cutime = stol(value);
      if (i == 17) cstime = stol(value);
    }
  }
  return utime + stime + cutime + cstime;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {   vector<string> cpu_utilization = CpuUtilization();
  if (cpu_utilization.size() >= 10) {
    long active_jiffies = stol(cpu_utilization[CPUStates::kUser_]) +
                          stol(cpu_utilization[CPUStates::kNice_]) +
                          stol(cpu_utilization[CPUStates::kSystem_]) +
                          stol(cpu_utilization[CPUStates::kIRQ_]) +
                          stol(cpu_utilization[CPUStates::kSoftIRQ_]) +
                          stol(cpu_utilization[CPUStates::kSteal_]);
    return active_jiffies;
  } else {
    return 0;
  } }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  vector<string> cpu_utilization = CpuUtilization();
  if (cpu_utilization.size() >= 10) {
    long idle_jiffies = stol(cpu_utilization[CPUStates::kIdle_]) +
                        stol(cpu_utilization[CPUStates::kIOwait_]);
    return idle_jiffies;
  } else {
    return 0;
  }
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  string line;
  vector<string> cpu_utilization;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    string cpu;
    linestream >> cpu; // Read and discard the "cpu" label
    while (linestream >> cpu) {
      cpu_utilization.push_back(cpu);
    }
  }
  return cpu_utilization;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  int total_processes = 0;
  string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.substr(0, 9) == "processes") {
        std::istringstream linestream(line.substr(10));
        linestream >> total_processes;
        break;  // We've obtained the necessary data, exit the loop.
      }
    }
  }
  return total_processes;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  int running_processes = 0;
  string line;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.substr(0, 8) == "procs_running") {
        std::istringstream linestream(line.substr(13));
        linestream >> running_processes;
        break;  // We've obtained the necessary data, exit the loop.
      }
    }
  }
  return running_processes;
}


// TODO: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string command;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, command);
  }
  return command;
}

// TODO: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string ram = "0";
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "VmSize:") {
        linestream >> ram;
        break;  // We've obtained the necessary data, exit the loop.
      }
    }
  }
  return ram;
}


// TODO: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string uid = "0";
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if (key == "Uid:") {
        linestream >> uid;
        break;  // We've obtained the necessary data, exit the loop.
      }
    }
  }
  return uid;
}


// TODO: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string line;
  string user;
  string uid = Uid(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.find(":" + uid + ":") != string::npos) {
        user = line.substr(0, line.find(":"));
        break;
      }
    }
  }
  return user;
}


// TODO: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line;
  string value;
  long uptime = 0;
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    for (int i = 1; i <= 22; i++) {
      linestream >> value;
      if (i == 22) uptime = UpTime() - (stol(value) / sysconf(_SC_CLK_TCK));
    }
  }
  return uptime;
}

