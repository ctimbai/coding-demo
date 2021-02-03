#include "TimerQueue.h"
#include "reactor/EventLoop.h"

#include <string.h>
#include <assert.h>
#include <unistd.h>

using namespace libcpp;
using namespace libcpp::detail;

TimerQueue::TimerQueue(EventLoop* loop)
  : loop_(loop),
    timerfd_(createTimerfd()),
    timerfdChannel_(loop, timerfd_),
    timers_(),
    callingExpiredTimers_(false)
{
  timerfdChannel_.setReadCallback(
            std::bind(&TimerQueue::handleRead, this));
  timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue()
{
  ::close(timerfd_);
  for (auto& tentry : timers_) {
    delete tentry.second;
  }
}

void TimerQueue::cancel(TimerId timerId)
{
  loop_->runInLoop(std::bind(&TimerQueue::cancelInLoop, this, timerId));
}


void TimerQueue::cancelInLoop(TimerId timerId)
{
  loop_->assertInLoopThread();
  assert(activeTimers_.size() == timers_.size());

  ActiveTimer timer(timerId.timer_, timerId.sequence_);
  ActiveTimerSet::iterator it = activeTimers_.find(timer);
  if (it != activeTimers_.end()) {
    size_t n = timers_.erase(TimerEntry(it->first->expiration(), it->first));
    assert(n == 1); (void)n;
    delete it->first;
    activeTimers_.erase(it);
  }
  else if (callingExpiredTimers_) {
    cancelingTimers_.insert(timer);
  }
}

TimerId TimerQueue::addTimer(const TimerCallback& cb, TimeStamp when, double interval)
{
  Timer* timer = new Timer(cb, when, interval);
  loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop, this, timer));
  return TimerId(timer);
}

void TimerQueue::addTimerInLoop(Timer* timer)
{
  bool earliestChanged = insert(timer);
  if (earliestChanged) {
    resetTimerfd(timerfd_, timer->expiration());
  }
}

bool TimerQueue::insert(Timer* timer)
{
  bool earliestChanged = false;
  TimerList::iterator it = timers_.begin();
  TimeStamp when = timer->expiration();
  if (it == timers_.end() || when < it->first) {
    earliestChanged = true;
  }

  {
  std::pair<TimerList::iterator, bool> ret =
                      timers_.insert(std::make_pair(when, timer));
  assert(ret.second == true);
  }

  {
  std::pair<ActiveTimerSet::iterator, bool> ret =
          activeTimers_.insert(std::make_pair(timer, timer->sequence()));
  assert(ret.second == true);
  }
  return earliestChanged;
}


void TimerQueue::handleRead()
{
  loop_->assertInLoopThread();
  TimeStamp now(TimeStamp::now());
  readTimerfd(timerfd_, now);

  std::vector<TimerEntry> expired = getExpired(now);

  callingExpiredTimers_ = true;
  cancelingTimers_.clear();
  for (auto& tentry : expired) {
    tentry.second->run();
  }
  callingExpiredTimers_ = false;
  reset(expired, now);
}

std::vector<TimerQueue::TimerEntry> TimerQueue::getExpired(TimeStamp now)
{
  std::vector<TimerEntry> expired;
  TimerEntry sentry = std::make_pair(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
  TimerList::iterator it = timers_.lower_bound(sentry); // FIXME: upper_bound?
  assert(it == timers_.end() || now < it->first);
  std::copy(timers_.begin(), it, back_inserter(expired));
  timers_.erase(timers_.begin(), it);

  for (auto& entry : expired) {
    ActiveTimer timer(entry.second, entry.second->sequence());
    activeTimers_.erase(timer);
  }

  return expired;
}

void TimerQueue::reset(std::vector<TimerEntry>& expired, TimeStamp now)
{
  TimeStamp nextExpired;
  for(auto& tentry : expired) {
    ActiveTimer timer(tentry.second, tentry.second->sequence());
    if (tentry.second->repeat()
        && cancelingTimers_.find(timer) == cancelingTimers_.end()) {
      tentry.second->restart(now);
      insert(tentry.second);
    }
    else {
      delete tentry.second;
    }
  }

  if (!timers_.empty()) {
    nextExpired = timers_.begin()->second->expiration();
  }

  if (nextExpired.valid()) {
    resetTimerfd(timerfd_, nextExpired);
  }
}
