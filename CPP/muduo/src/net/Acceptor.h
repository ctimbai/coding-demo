#ifndef LIBCPP_REACTOR_ACCEPTOR_H_
#define LIBCPP_REACTOR_ACCEPTOR_H_


#include "Socket.h"
#include "reactor/Channel.h"
#include <functional>

namespace libcpp
{
class EventLoop;
class InetAddress;

class Acceptor
{
public:
  typedef std::function<void(int, const InetAddress&)> NewConnCallback;
  
  Acceptor(EventLoop* loop, const InetAddress& listenAddr);
  
  void setNewConnCallback(const NewConnCallback& cb)
  { newConnCallback_ = cb; }
  
  void listen();
  bool listening() const { return listening_; }
  
private:
  void handleRead();

  EventLoop* loop_;
  Socket acceptSocket_;
  Channel acceptChannel_;
  NewConnCallback newConnCallback_;
  bool listening_;
};

}

#endif