#ifndef LIBCPP_EPOLLER_H_
#define LIBCPP_EPOLLER_H_

#include "EventLoop.h"
#include "datetime/TimeStamp.h"

#include <vector>
#include <map>

struct epoll_event;

namespace libcpp
{
class Channel;

class EPoller
{
public:
  using ChannelList = std::vector<Channel*>;
  
  EPoller(EventLoop* loop);
  ~EPoller();
  
  TimeStamp poll(int timeoutMs, ChannelList* activeChannel);
  
  void updateChannel(Channel* channel);
  void removeChannel(Channel* channel);
  
  void assertInLoopThread() { ownerLoop_->assertInLoopThread(); }
  
private:
  static const int kInitEventListSize = 16;
  
  void fillActiveChannels(int numEvents, 
                          ChannelList* activeChannels) const;
  void update(int operation, Channel* channel);
  
  using EventList = std::vector<struct epoll_event>;
  using ChannelMap = std::map<int, Channel*>;
  
  EventLoop* ownerLoop_;
  int epollfd_;
  EventList events_;
  ChannelMap channels_;
};

}

#endif