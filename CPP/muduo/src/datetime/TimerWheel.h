#ifndef LIBCPP_TIMERWHEEL_H_
#define LIBCPP_TIMERWHEEL_H_

#include "Timer.h"
#include "TimerId.h"
#include "TimeStamp.h"
#include "reactor/Channel.h"

#include <vector>
#include <list>
#include <unordered_map>
#include <memory>

namespace libcpp
{
class EventLoop;
/*
 * A Hierarchical Timer Wheel
 *
 * Feature:
 *  1. hierarchical structure for various timeout
 *  2. delay queue to prevent from tick/setting timer too frequently
 *  3. insert and delete are O(1) operations
 *  4. create a upper level time wheel only when it's necessary
 *
 * Components:
 *  1. Time Wheel
 *  2. Bucket
 */
class TimerWheel
{
public:
  using TimerList = std::list<Timer*>;
  /*
   * A TimerWheel's params include:
   *  1. base tick unit
   *  2. wheel size(number of buckets) per wheel
   */
  TimerWheel(EventLoop* loop, double tick, int wheelSize);
  ~TimerWheel();

  void cancel(TimerId timer);

  /*
   * Add a timer to run 'cb' at 'when', and if 'interval' > 0, the task will
   * repeat per 'interval'. (the time unit is second)
   */
  TimerId addTimer(const TimerCallback& cb, TimeStamp when, double interval);

private:
  class TimerWheelImpl;
  struct Bucket
  {
    TimeStamp expirationTime;
    TimerList timers;

    bool setExpiredTime(TimeStamp expired) {
      if (expirationTime == expired) return false;
      expirationTime = expired;
      return true;
    }
  };

  using TimerPos = std::pair<Bucket*, TimerList::iterator>;
  using BucketListIter = std::list<Bucket*>::iterator;
  using TimerHashTable = std::unordered_map<Timer*, TimerPos>;
  using DelayHashTable = std::unordered_map<Bucket*, BucketListIter>;

  void cancelInLoop(TimerId timerId);
  void handleRead();
  TimerList getExpired(TimeStamp now);
  void addTimerInLoop(Timer* timer);
  bool insert(Timer* timer);
  void updateDelayQueue(Bucket* b);

private:
  EventLoop* loop_;
  const int timerfd_;
  Channel timerfdChannel_;

  /* in order to achive O(1) deletion */
  TimerHashTable timerHashTable_;
  /* we own the timerWheel entry */
  std::unique_ptr<TimerWheelImpl> timerWheel_;
  /* manage buckets with tasks in timeout order */
  // FIXME: we assume that timers won't be canceled frequently
  std::list<Bucket*> delayQueue_;
  DelayHashTable delayHashTable_;

  /* for cancel() */
  // FIXME: it is for self-canceling timer task
  bool callingExpiredTimers_;
};

}

#endif