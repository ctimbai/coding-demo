#ifndef LIBCPP_TIMER_ID_H_
#define LIBCPP_TIMER_ID_H_

#include <stdint.h>
#include <list>

namespace libcpp
{

class Timer;

class TimerId
{
public:
  explicit TimerId(Timer* timer = nullptr, int64_t seq = 0)
    : timer_(timer),
      sequence_(seq)
  {}

  friend class TimerQueue;
  friend class TimerWheel;
private:
  Timer* timer_;
  int64_t sequence_;
};

}

#endif