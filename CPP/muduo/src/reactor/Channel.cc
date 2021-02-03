#include "Channel.h"
#include "EventLoop.h"
#include "logging/Logging.h"

#include <poll.h>

using namespace libcpp;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI | POLLRDHUP;
const int Channel::kWriteEvent = POLLOUT;


Channel::Channel(EventLoop* loop, int fd)
  : loop_(loop),
    fd_(fd),
    events_(0),
    revents_(0),
    index_(-1)
{
}

void Channel::handleEvent(TimeStamp receiveTime)
{
  if (revents_ & POLLNVAL) {
    LOG_WARN << "[Channel] handleEvent() POLLNVAL";
  }
  if (revents_ & (POLLNVAL | POLLERR)) {
    if (errorCallback_) errorCallback_();
  }
  if (revents_ & kReadEvent) {
    if (readCallback_) readCallback_(receiveTime);
  }
  if (revents_ & kWriteEvent) {
    if (writeCallback_) writeCallback_();
  }
}

void Channel::update()
{
  loop_->updateChannel(this);
}