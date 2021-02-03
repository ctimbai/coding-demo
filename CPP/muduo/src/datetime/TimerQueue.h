#ifndef LIBCPP_TIMER_QUEUE_H_
#define LIBCPP_TIMER_QUEUE_H_

#include "Timer.h"
#include "TimerId.h"

#include "reactor/Channel.h"

#include <utility>
#include <set>
#include <vector>

namespace libcpp
{
class EventLoop;
class Channel;

class TimerQueue
{
public:
  TimerQueue(EventLoop* loop);
  ~TimerQueue();
  
  void cancel(TimerId timerId);
  
  TimerId addTimer(const TimerCallback& cb, TimeStamp when, double interval);
  
private:
  using TimerEntry = std::pair<TimeStamp, Timer*>;
  using TimerList = std::set<TimerEntry>;
  using ActiveTimer = std::pair<Timer*, int64_t>;
  using ActiveTimerSet = std::set<ActiveTimer>;
  
  void addTimerInLoop(Timer* timer);
  void cancelInLoop(TimerId timerId);
  void handleRead();
  std::vector<TimerEntry> getExpired(TimeStamp now);
  void reset(std::vector<TimerEntry>& expired, TimeStamp now);
  
  bool insert(Timer* timer);
  
  EventLoop* loop_;
  const int timerfd_;
  Channel timerfdChannel_;
  TimerList timers_;
  
  // for cancel()
  bool callingExpiredTimers_;
  ActiveTimerSet activeTimers_;
  ActiveTimerSet cancelingTimers_;
};

}

#endif