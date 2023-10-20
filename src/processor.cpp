#include "processor.h"
#include "linux_parser.h"

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  long idle, iowait, system, irq, softirq, steal;
  long user, nice;

  // Read the CPU statistics
  std::vector<std::string> cpu_utilization = LinuxParser::CpuUtilization();

  if (cpu_utilization.size() >= 8) {
    user = std::stol(cpu_utilization[0]);
    nice = std::stol(cpu_utilization[1]);
    system = std::stol(cpu_utilization[2]);
    idle = std::stol(cpu_utilization[3]);
    iowait = std::stol(cpu_utilization[4]);
    irq = std::stol(cpu_utilization[5]);
    softirq = std::stol(cpu_utilization[6]);
    steal = std::stol(cpu_utilization[7]);

    // Calculate the total and active jiffies
    long total = user + nice + system + idle + iowait + irq + softirq + steal;
    long active = user + nice + system + irq + softirq + steal;

    // Calculate the utilization as a percentage
    if (total > 0) {
      float utilization = static_cast<float>(active) / static_cast<float>(total);
      return utilization;
    }
  }

  return 0.0;
}