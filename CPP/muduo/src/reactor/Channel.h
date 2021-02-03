#ifndef LIBCPP_CHANNEL_H_
#define LIBCPP_CHANNEL_H_

#include <functional>
#include "datetime/TimeStamp.h"

namespace libcpp
{
class EventLoop;

class Channel
{
public:
  using EventCallback = std::function<void()>;
  using ReadEventCallback = std::function<void(TimeStamp)>;
  
  Channel(EventLoop* loop, int fd);
  
  void handleEvent(TimeStamp);
  
  void setReadCallback(const ReadEventCallback& cb)
  { readCallback_ = cb; }
  void setWriteCallback(const EventCallback& cb)
  { writeCallback_ = cb; }
  void setErrorCallback(const EventCallback& cb)
  { errorCallback_ = cb; }
  void setCloseCallback(const EventCallback& cb)
  { closeCallback_ = cb; }
  
  void enableReading()
  { events_ |= kReadEvent; update(); }
  void disableReading()
  { events_ &= ~kReadEvent; update(); }
  void enableWriting()
  { events_ |= kWriteEvent; update(); }
  void disableWriting()
  { events_ &= ~kWriteEvent; update(); }
  bool isWriting() const { return events_ & kWriteEvent; }
  
  void disableAllEvents()
  { events_ = kNoneEvent; update(); }
  
  int fd() const { return fd_; }
  int events() const { return events_; }
  bool isNoneEvent() const { return events_ == kNoneEvent; }
  
  int index() const { return index_; }
  void set_index(int idx) { index_ = idx; }
  void set_revents(int revents) { revents_ = revents; }
  
private:
  void update();
  
  static const int kReadEvent;
  static const int kWriteEvent;
  static const int kNoneEvent;
  
  EventLoop* loop_;
  
  const int fd_;
  int events_;
  int revents_;
  int index_;
  
  ReadEventCallback readCallback_;
  EventCallback writeCallback_;
  EventCallback errorCallback_;
  EventCallback closeCallback_;
};

}

#endif