#include "EPoller.h"
#include "Channel.h"
#include "logging/Logging.h"

#include <sys/epoll.h>
#include <string.h>
#include <unistd.h>

using namespace libcpp;

namespace
{
const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;
}


EPoller::EPoller(EventLoop* loop)
  : ownerLoop_(loop),
    epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
    events_(kInitEventListSize)
{
  if (epollfd_ < 0) {
    LOG_SYSFATAL << "[EPoller] create epoll fd failed";
  }
}


EPoller::~EPoller()
{
  ::close(epollfd_);
}

TimeStamp EPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
  /* events_ will store the active epoll_events */
  int numEvents = ::epoll_wait(epollfd_,
                                &*events_.begin(),
                                static_cast<int>(events_.size()),
                                timeoutMs);
  TimeStamp now(TimeStamp::now());

  if (numEvents > 0) {
    LOG_TRACE << "[EPoller] " << numEvents << " events happened";
    fillActiveChannels(numEvents, activeChannels);
    /* it means there may be more events going to occur, thus scale */
    if (static_cast<size_t>(numEvents) == events_.size()) {
      events_.resize(numEvents * 2);
    }
  }
  else if (numEvents == 0) {
    LOG_TRACE << "[EPoller] nothing happened";
  }
  else {
    LOG_SYSERR << "[EPoller] epoll_wait failed";
  }

  return now;
}

/*
 *   typedef union epoll_data {
 *      void    *ptr;
 *      int      fd;
 *      uint32_t u32;
 *      uint64_t u64;
 *   } epoll_data_t;
 *
 *   struct epoll_event {
 *      uint32_t     events;    // Epoll events
 *      epoll_data_t data;      // User data variable
 *   };
 */

void EPoller::fillActiveChannels(int numEvents,
                                  ChannelList* activeChannels) const
{
  for (int i = 0; i < numEvents; ++i) {
    // Question: why data.ptr can be safely changed into Channel*?
    // Answer: when update channel data.ptr is set to Channel*
    Channel* channel = static_cast<Channel*>(events_[i].data.ptr);
    channel->set_revents(events_[i].events);
    activeChannels->push_back(channel);
  }
}

void EPoller::updateChannel(Channel* channel)
{
  assertInLoopThread();

  int index = channel->index();
  if (index == kNew || index == kDeleted) {
    int fd = channel->fd();
    if (index == kNew) {
      /* kNew means it isn't in channels_ */
      channels_[fd] = channel;
    }
    else {
      /* kDeleted means it is in channels_, but not in listen list */
    }
    channel->set_index(kAdded);
    update(EPOLL_CTL_ADD, channel);
  }
  else {
    if (channel->isNoneEvent()) {
      update(EPOLL_CTL_DEL, channel);
      channel->set_index(kDeleted);
    }
    else {
      update(EPOLL_CTL_MOD, channel);
    }
  }
}

void EPoller::removeChannel(Channel* channel)
{
  assertInLoopThread();
  int fd = channel->fd();
  size_t n = channels_.erase(fd);
  (void)n;
  if (channel->index() == kAdded) {
    update(EPOLL_CTL_DEL, channel);
  }
  /*
   * it has been removed from channels_, so for channels_,
   * it's a new channel
   */
  channel->set_index(kNew);
}


void EPoller::update(int operation, Channel* channel)
{
  struct epoll_event event;
  bzero(&event, sizeof event);
  event.events = channel->events();
  event.data.ptr = channel;
  int fd = channel->fd();
  if (::epoll_ctl(epollfd_, operation, fd, &event) < 0) {
    if (operation == EPOLL_CTL_DEL) {
      LOG_SYSERR << "[EPoller] epoll_ctl op=" << operation
                    << " fd=" << fd;
    }
    else {
      LOG_SYSFATAL << "[EPoller] epoll_ctl op=" << operation
                    << " fd=" << fd;
    }
  }
}

