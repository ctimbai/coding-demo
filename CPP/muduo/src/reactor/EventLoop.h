#ifndef LIBCPP_EVENTLOOP_H_
#define LIBCPP_EVENTLOOP_H_

#include "datetime/TimerQueue.h"

#include <vector>
#include <thread>
#include <mutex>
#include <memory>
#include <functional>

namespace libcpp
{
class Channel;
class EPoller;

class EventLoop
{
public:
  using Functor = std::function<void()>;
  EventLoop(int ms = 10000);
  ~EventLoop();

  void loop();
  void quit();

  void runInLoop(const Functor& cb);
  void queueInLoop(const Functor& cb);
  void wakeup();

  TimerId runAt(const TimeStamp& time, const TimerCallback& cb);
  TimerId runAfter(double delay, const TimerCallback& cb);
  TimerId runEvery(double interval, const TimerCallback& cb);

  void cancel(TimerId timerId);

  /*
   * Internal function: Channel(update) -> EventLoop -> Poller
   */
  void updateChannel(Channel* channel);
  void removeChannel(Channel* channel);

  void assertInLoopThread()
  {
    if (!isInLoopThread()) {
      abortNotInLoopThread();
    }
  }

  bool isInLoopThread() const { return threadId_ == std::this_thread::get_id(); }

private:
  void abortNotInLoopThread();
  void handleWakeup();
  void doPendingFunctors();

  using ChannelList = std::vector<Channel*>;

  bool looping_;
  bool quit_;
  bool doingPendingFunctors_;
  const std::thread::id threadId_;
  TimeStamp pollReturnTime_;

  const int pollMs_;
  std::unique_ptr<EPoller> poller_;
  std::unique_ptr<TimerQueue> timerQueue_;
  ChannelList activeChannels_;

  int wakeupFd_;
  std::unique_ptr<Channel> wakeupChannel_;
  std::mutex mutex_;
  std::vector<Functor> pendingFunctors_;
};

}

#endif