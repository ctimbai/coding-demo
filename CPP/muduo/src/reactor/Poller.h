#ifndef LIBCPP_POLLER_H_
#define LIBCPP_POLLER_H_

#include "datetime/TimeStamp.h"

#include <vector>
#include <map>

/*
 * int   fd;        
 * short events;     
 * short revents; 
 */
struct pollfd;

namespace libcpp
{
class EventLoop;
class Channel;

class Poller
{
public:
  using ChannelList = std::vector<Channel*>;
  
  Poller(EventLoop* loop);
  TimeStamp poll(int timeoutMs, ChannelList* activeChannels);
  
  void updateChannel(Channel* channel);
  void removeChannel(Channel* channel);
  
  void assertInLoopThread();
  
private:
  void fillActiveChannels(int activeNums, ChannelList* activeChannels);
  
  using PollfdList = std::vector<struct pollfd>;
  using ChannelMap = std::map<int, Channel*>;
  
  EventLoop* ownerLoop_;
  PollfdList pollfds_;
  ChannelMap channels_;
};

}

#endif