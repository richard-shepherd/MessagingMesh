#include <cstdarg>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include "Utils.h"
using namespace MessagingMesh;

// Returns a std::string created using the string format and variadic arguments.
std::string Utils::format(const char* format, ...)
{
    // We allocate a result string with an initial size...
    const int BUFFER_SIZE = 4096;
    std::string result;
    result.resize(BUFFER_SIZE);

    // We format the data into the result...
    va_list args;
    va_start(args, format);
    int size = vsnprintf(&result[0], BUFFER_SIZE + 1, format, args);
    va_end(args);

    // We set the size of the string...
    result.resize(size);

    // If the formatted string has fit into the result, we return it...
    if (size <= BUFFER_SIZE)
    {
        return result;
    }

    // The formatted string did not fit into the original string size, so we re-format 
    // it as the string has now been set to the discovered size above...
    va_start(args, format);
    vsnprintf(&result[0], size + 1, format, args);
    va_end(args);

    return result;
}

// Returns a time string in the format HH:MM:SS.mmm
std::string Utils::getTimeString()
{
    using namespace std::chrono;

    // get current time
    auto now = system_clock::now();

    // get number of milliseconds for the current second
    // (remainder after division into seconds)
    auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

    // convert to std::time_t in order to convert to std::tm (broken time)
    auto timer = system_clock::to_time_t(now);

    // convert to broken time
    std::tm bt;
    localtime_s(&bt, &timer);

    std::ostringstream oss;
    oss << std::put_time(&bt, "%H:%M:%S"); // HH:MM:SS
    oss << '.' << std::setfill('0') << std::setw(3) << ms.count();
    return oss.str();
}

