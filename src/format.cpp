#include <string>

#include "format.h"

using std::string;

// TODO: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;

    char formatted_time[9]; // HH:MM:SS + '\0'
    snprintf(formatted_time, sizeof(formatted_time), "%02d:%02d:%02d", hours, minutes, secs);

    return string(formatted_time);
}