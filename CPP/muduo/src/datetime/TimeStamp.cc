#include "TimeStamp.h"

#include <sys/time.h>
#include <stdio.h>
#include <inttypes.h>

using namespace libcpp;

// compiling check
static_assert(sizeof(TimeStamp) == sizeof(int64_t), 
                "[Compiling ERROR] Only support 64-bit platform.");
                
TimeStamp::TimeStamp()
  : microSecondsSinceEpoch_(0)
{}

TimeStamp::TimeStamp(int64_t microSecondsSinceEpoch)
  : microSecondsSinceEpoch_(microSecondsSinceEpoch)
{}


std::string TimeStamp::toString() const
{
    char buf[32] = {0};
    int64_t sec = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
    int64_t usec = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
    snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "", sec, usec);
    return buf;
}

std::string TimeStamp::toFormattedString() const
{
  char buf[32] = {0};
  time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
  int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
  struct tm tm_time;
  ::gmtime_r(&seconds, &tm_time);
  
  // year-month-day hour:min:sec.microseconds, 'Z' means UTC time
  snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06dZ",
    tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
    tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
    microseconds);
  return buf;
}



/* struct timeval
 * {
 * __time_t tv_sec;        // Seconds.
 * __suseconds_t tv_usec;  // Microseconds.
 * }; 
 */
TimeStamp TimeStamp::now()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return TimeStamp(tv.tv_sec * kMicroSecondsPerSecond + tv.tv_usec);
}

