#include "Poller.h"
#include "EventLoop.h"
#include "Channel.h"

#include "logging/Logging.h"

#include <poll.h>

using namespace libcpp;

Poller::Poller(EventLoop* loop)
  : ownerLoop_(loop)
{
}

void Poller::assertInLoopThread() 
{ 
  ownerLoop_->assertInLoopThread(); 
}

TimeStamp Poller::poll(int timeoutMs, ChannelList* activeChannels)
{
  int activeNums = ::poll(pollfds_.data(), pollfds_.size(), timeoutMs);
  TimeStamp now(TimeStamp::now());
  if (activeNums > 0) {
    fillActiveChannels(activeNums, activeChannels);
  }
  else if (activeNums == 0) {
    LOG_TRACE << "[Poller] poll nothing happened";
  }
  else {
    LOG_ERROR << "[Poller] poll()";
  }
  return now;
}


void Poller::fillActiveChannels(int activeNums, ChannelList* activeChannels)
{
  for (auto& pfd : pollfds_) {
    if (pfd.revents > 0) {
      ChannelMap::const_iterator ch = channels_.find(pfd.fd);
      Channel* channel = ch->second;
      channel->set_revents(pfd.revents);
      activeChannels->push_back(channel);
      if (--activeNums == 0) break;
    }
  }
}

void Poller::updateChannel(Channel* channel)
{
  assertInLoopThread();
  LOG_TRACE << "[Poller] Update channel fd=" << channel->fd()
            << " events=" << channel->events();
  if (channel->index() < 0) {
    struct pollfd pfd;
    pfd.fd = channel->fd();
    pfd.events = static_cast<short>(channel->events());
    pfd.revents = 0;
    pollfds_.push_back(pfd);
    int idx = static_cast<int>(pollfds_.size()) - 1;
    channel->set_index(idx);
    channels_[pfd.fd] = channel;
  }
  else {
    int idx = channel->index();
    struct pollfd& pfd = pollfds_[idx];
    pfd.events = static_cast<short>(channel->events());
    pfd.revents = 0;
    if (channel->isNoneEvent()) {
      pfd.fd = (-pfd.fd - 1);
    }
  }
}

void Poller::removeChannel(Channel* channel)
{
  assertInLoopThread();
  int idx = channel->index();
  /* poll doesn't own channel, so do not call delete */
  channels_.erase(channel->fd());
  if ((size_t)idx == pollfds_.size() - 1) {
    pollfds_.pop_back();
  }
  else {
    int channelAtEnd = pollfds_.back().fd;
    std::iter_swap(pollfds_.begin() + idx, pollfds_.end() - 1);
    if (channelAtEnd < 0) {
      channelAtEnd = -channelAtEnd-1;
    }
    channels_[channelAtEnd]->set_index(idx);
    pollfds_.pop_back();
  }
}

