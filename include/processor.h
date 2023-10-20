#ifndef PROCESSOR_H
#define PROCESSOR_H

class Processor {
 public:
  float Utilization();  // TODO: See src/processor.cpp

  // TODO: Declare any necessary private members
 private:
  long prev_idle_ = 0;  // Previous idle time
  long prev_total_ = 0; // Previous total time
};

#endif