#include "Timer.h"
#include "logging/Logging.h"

#include <unistd.h>
#include <string.h>

namespace libcpp
{
namespace detail
{

struct timespec howMuchTimeFromNow(TimeStamp when)
{
  int64_t microSeconds = when.microSecondsSinceEpoch()
                            - TimeStamp::now().microSecondsSinceEpoch();
  if (microSeconds < 100) {
    microSeconds = 100;
  }
  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(microSeconds
                                  / TimeStamp::kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<long>(
          (microSeconds % TimeStamp::kMicroSecondsPerSecond) * 1000);
  return ts;
}

int createTimerfd()
{
  int fd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  if (fd < 0) {
    LOG_FATAL << "[TimerFd] timerfd_create() failed";
  }
  return fd;
}

void resetTimerfd(int timerfd, TimeStamp expiration)
{
  struct itimerspec newValue;
  struct itimerspec oldValue;
  bzero(&newValue, sizeof newValue);
  bzero(&oldValue, sizeof oldValue);
  newValue.it_value = howMuchTimeFromNow(expiration);
  /*
   * 0 means relative timer, TFD_TIMER_ABSTIME is absolute timer
   */
  int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
  if (ret) {
    LOG_ERROR << "[TimerFd] timerfd_settime() failed";
  }
}

void readTimerfd(int timerfd, TimeStamp now)
{
  uint64_t howmany;
  ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
  LOG_TRACE << "[TimerFd] read timerfd timeout " << howmany
            << " times at " << now.toString();
  if (n != sizeof(howmany)) {
    LOG_ERROR << "[TimerFd] read " << n << "bytes instead of 8";
  }
}

} // detail
} // libcpp

using namespace libcpp;

std::atomic<uint64_t> Timer::s_numCreated_;


void Timer::restart(TimeStamp now)
{
  if (repeat_) {
    expiration_ = addTime(now, intervalSeconds_);
  }
  else {
    expiration_ = TimeStamp::invalid();
  }
}

