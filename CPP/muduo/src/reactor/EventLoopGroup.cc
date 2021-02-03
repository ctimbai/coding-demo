#include "EventLoopGroup.h"
#include "EventLoop.h"

#include <functional>
#include <future>

using namespace libcpp;

EventLoopGroup::EventLoopGroup(EventLoop* baseLoop)
  : baseLoop_(baseLoop),
    started_(false),
    numThreads_(0),
    next_(0)
{
}

EventLoopGroup::~EventLoopGroup()
{
}

void EventLoopGroup::start()
{
  baseLoop_->assertInLoopThread();
  
  if (!started_) {
    started_ = true;
    for (int i = 0; i < numThreads_; ++i) {
      std::promise<EventLoop*> p;
      
      threads_.emplace_back(
        [&p]()
        {
          EventLoop loop;
          p.set_value(&loop);
          loop.loop();
        });
      
      loops_.push_back(p.get_future().get());
    }
  }
}

EventLoop* EventLoopGroup::getNextLoop()
{
  baseLoop_->assertInLoopThread();
  
  EventLoop* loop = baseLoop_;
  
  if (!loops_.empty()) {
    loop = loops_[next_++];
    if (next_ >= loops_.size()) {
      next_ = 0;
    }
  }
  
  return loop;
}
