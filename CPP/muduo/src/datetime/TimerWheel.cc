#include "TimerWheel.h"
#include "reactor/EventLoop.h"
#include "logging/Logging.h"

#include <unistd.h>

/*****************************************************************/
/*                    Components Definition                      */
/*****************************************************************/
namespace libcpp
{

class TimerWheel::TimerWheelImpl
{
public:
  TimerWheelImpl(TimerWheel* manager, double tickS, int wheelSize, TimeStamp startTime)
    : manager_(manager),
      tick_(tickS),
      wheelSize_(wheelSize),
      interval_(tick_ * wheelSize_),
      currentTime_(startTime)
  {
    for (int i = 0; i < wheelSize_; ++i) {
      buckets_.emplace_back(new Bucket());
    }
  }

  bool insert(Timer* timer)
  {
    bool expired = false;
    TimeStamp expirationTime = timer->expiration();
    /*
     * if expiration time is in current tick range,
     * it indicate that this timer is already expired
     */
    if (expirationTime < addTime(currentTime_, tick_)) {
      expired = true;
    }
    /*
     * if expiration time is in current wheel interval,
     * we should insert it into current wheel
     */
    else if (expirationTime < addTime(currentTime_, interval_)) {
      int64_t virtualId = expirationTime.microSecondsSinceEpoch()
                            / static_cast<int64_t>(tick_ * TimeStamp::kMicroSecondsPerSecond);
      int bucketId = virtualId % wheelSize_;
      Bucket* b = buckets_[bucketId].get();
      b->timers.push_back(timer);
      /* record its owner bucket and pos in TimerWheel */
      TimerList::iterator iter = b->timers.end();
      --iter;
      manager_->timerHashTable_[timer] = TimerPos(b, iter);

      /*
       * if bucket's expired time is changed, then update TimerWheel's delayQueue
       * and restart timerfd
       */
      bool expiredTimeChanged = b->setExpiredTime(addTime(currentTime_, bucketId * tick_));
      if (expiredTimeChanged) {
        /* expiration time may be changed */
        manager_->updateDelayQueue(b);
      }
    }
    /*
     * when current TimerWheel overflow:
     *  1. if upper level wheel doesn't exist, create a upper level time wheel then insert
     *  2. if upper level wheel exist, insert timer
     */
    else {
      if (!upperTimerWheel_) {
        upperTimerWheel_.reset(new TimerWheelImpl(manager_, tick_ * wheelSize_, wheelSize_, currentTime_));
      }
      upperTimerWheel_->insert(timer);
    }
    return expired;
  }

  void advanceClock(TimeStamp expiration) {
    /* only when at least one tick lapsed, we advance the base time */
    if (!(expiration < addTime(currentTime_, tick_))) {
      currentTime_ = truncate(expiration, tick_);
      if (upperTimerWheel_) upperTimerWheel_->advanceClock(currentTime_);
    }
  }

private:
  TimeStamp truncate(TimeStamp current, double unit) {
    int64_t remain = current.microSecondsSinceEpoch()
                    % static_cast<int64_t>(tick_ * TimeStamp::kMicroSecondsPerSecond);
    return TimeStamp(current.microSecondsSinceEpoch() - remain);
  }

private:
  using BucketUptr = std::unique_ptr<Bucket>;
  TimerWheel* manager_;
  double tick_;
  int wheelSize_;
  double interval_;
  TimeStamp currentTime_;
  std::vector<BucketUptr> buckets_;
  /* if this wheel overflow, create a new upper level wheel */
  std::unique_ptr<TimerWheelImpl> upperTimerWheel_;
};

} /* libcpp */


/*****************************************************************/
/*                TimerWheel Implementation                      */
/*****************************************************************/

using namespace libcpp;
using namespace libcpp::detail;

TimerWheel::TimerWheel(EventLoop* loop, double tick, int wheelSize)
  : loop_(loop),
    timerfd_(createTimerfd()),
    timerfdChannel_(loop_, timerfd_),
    timerWheel_(new TimerWheelImpl(this, tick, wheelSize, TimeStamp::now()))
{
  timerfdChannel_.setReadCallback(
      std::bind(&TimerWheel::handleRead, this));
  timerfdChannel_.enableReading();
}

TimerWheel::~TimerWheel()
{
  ::close(timerfd_);
}

void TimerWheel::cancel(TimerId timerId)
{
  loop_->runInLoop(
    std::bind(&TimerWheel::cancelInLoop, this, timerId));
}

void TimerWheel::cancelInLoop(TimerId timerId)
{
  Timer* timer = timerId.timer_;
  TimerPos pos = timerHashTable_[timer];
  pos.first->timers.erase(pos.second);
  timerHashTable_.erase(timer);
  delete timer;
}

void TimerWheel::handleRead()
{
  loop_->assertInLoopThread();
  TimeStamp nextExpired;
  TimeStamp now(TimeStamp::now());
  readTimerfd(timerfd_, now);

  /* run timeout tasks */
  TimerList expiredTimers = getExpired(now);
  callingExpiredTimers_ = true;
  for (auto& timerPtr : expiredTimers) timerPtr->run();
  callingExpiredTimers_ = false;
  /* advance current time */
  timerWheel_->advanceClock(now);
  /* re-insert repeated timer and clear one-shot timer */
  for (auto& timer : expiredTimers) {
    if (timer->repeat()) {
      timer->restart(now);
      timerWheel_->insert(timer);
    }
    else {
      timerHashTable_.erase(timer);
      delete timer;
    }
  }
  /* reset timer fd */
  if (!delayQueue_.empty()) nextExpired = delayQueue_.front()->expirationTime;
  if (nextExpired.valid()) resetTimerfd(timerfd_, nextExpired);
}

TimerWheel::TimerList TimerWheel::getExpired(TimeStamp now)
{
  TimerList expired;
  while (!delayQueue_.empty() && delayQueue_.front()->expirationTime < now) {
    Bucket* bucket = delayQueue_.front();
    /* we should clear timeout timer in every structure*/
    // Note: splice is transfer elems to another list, so origin list will be empty
    expired.splice(expired.end(), bucket->timers);
    delayQueue_.pop_front();
    delayHashTable_.erase(bucket);
  }
  return expired;
}


TimerId TimerWheel::addTimer(const TimerCallback& cb, TimeStamp when, double interval)
{
  Timer* timer = new Timer(cb, when, interval);
  loop_->runInLoop(std::bind(&TimerWheel::addTimerInLoop, this, timer));
  return TimerId(timer);
}

void TimerWheel::addTimerInLoop(Timer* timer)
{
  /*
   * we try to insert timer into timerwheel, if insert failed,
   * it means this timer already expired, then run it in loop;
   * otherwise, it's inserted into timerwheel for timeout
   */
  bool expired = timerWheel_->insert(timer);
  if (expired) {
    /*
     * we need to determine if the timer is repeatable:
     *  if repeatable, insert again
     *  otherwise, remove it
     */
    timer->run();
    if (timer->repeat()) {
      timer->restart(TimeStamp::now());
      timerWheel_->insert(timer);
    }
    // FIXME: can we delete it now?
    else delete timer;
  }
}

void TimerWheel::updateDelayQueue(Bucket* b)
{
  if (delayQueue_.size() == 0) {
    delayQueue_.push_back(b);
    delayHashTable_[b] = delayQueue_.end();
    --delayHashTable_[b];
    TimeStamp newExpiration = delayQueue_.front()->expirationTime;
    resetTimerfd(timerfd_, newExpiration);
  }
  else {
    TimeStamp currentExpiration = delayQueue_.front()->expirationTime;
    if (delayHashTable_.count(b)) {
      BucketListIter iter = delayHashTable_[b];
      delayQueue_.erase(iter);
    }
    delayQueue_.push_back(b);
    delayHashTable_[b] = delayQueue_.end();
    --delayHashTable_[b];
    delayQueue_.sort();
    TimeStamp newExpiration = delayQueue_.front()->expirationTime;
    if (!(currentExpiration == newExpiration)) {
      resetTimerfd(timerfd_, newExpiration);
    }
  }
}

