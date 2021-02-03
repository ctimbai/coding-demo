#ifndef LIBCPP_UDPSERVER_H_
#define LIBCPP_UDPSERVER_H_

#include "utils/noncopyable.h"
#include "reactor/Channel.h"
#include "io/Buffer.h"
#include "datetime/TimeStamp.h"
#include "Socket.h"
#include "InetAddress.h"

#include <functional>
#include <memory>
#include <string>

namespace libcpp
{
class EventLoop;

class UdpServer : public utils::noncopyable
{
public:
  using MessageCallback = std::function<void(const InetAddress&, Buffer*, TimeStamp)>;

  UdpServer(EventLoop* loop, const InetAddress& addr,
            const std::string& name = "UdpServer");
  ~UdpServer();

  void sendto(const std::string& msg, const InetAddress& peerAddr);

  void setMessageCallback(const MessageCallback& cb)
  { messageCallback_ = cb; }

private:
  void handleRead(TimeStamp receivedTime);
  void handleWrite();

private:
  EventLoop* loop_;
  const std::string name_;
  Socket udpSocket_;
  Channel channel_;
  
  Buffer inputBuffer_;
  /* Callbacks */
  MessageCallback messageCallback_;
};

}

#endif 