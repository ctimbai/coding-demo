#ifndef LIBCPP_EVENTLOOPGROUP_H_
#define LIBCPP_EVENTLOOPGROUP_H_

#include <vector>
#include <memory>
#include <thread>

namespace libcpp
{
class EventLoop;

class EventLoopGroup
{
public:
  EventLoopGroup(EventLoop* baseLoop);
  ~EventLoopGroup();
  
  void setThreadNum(int num) { numThreads_ = num; }
  void start();
  EventLoop* getNextLoop();
  int getLoopNum() const { return numThreads_; }
  
private:
  EventLoop* baseLoop_;
  bool started_;
  int numThreads_;
  size_t next_;
  
  std::vector<std::thread> threads_;
  std::vector<EventLoop*> loops_;
};

}

#endif