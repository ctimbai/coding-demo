/*
 * Description: timer timeout once
 */
#include "reactor/EventLoop.h"
#include "reactor/Channel.h"
#include "datetime/TimeStamp.h"

#include <sys/timerfd.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

using namespace libcpp;

EventLoop* g_loop;

void timeout(TimeStamp receiveTime)
{
  printf("timeout %s\n", receiveTime.toFormattedString().c_str());
  g_loop->quit();
}

int main()
{
  EventLoop loop;
  g_loop = &loop;
  
  int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
  Channel channel(&loop, timerfd);
  channel.setReadCallback(timeout);
  channel.enableReading();
  
  struct itimerspec howlong;
  bzero(&howlong, sizeof(howlong));
  howlong.it_value.tv_sec = 5;
  ::timerfd_settime(timerfd, 0, &howlong, NULL);
  
  loop.loop();
  ::close(timerfd);
}