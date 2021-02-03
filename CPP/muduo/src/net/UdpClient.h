#ifndef LIBCPP_UDPCLIENT_H_
#define LIBCPP_UDPCLIENT_H_

#include "utils/noncopyable.h"
#include "io/Buffer.h"
#include "datetime/TimeStamp.h"
#include "reactor/Channel.h"
#include "InetAddress.h"
#include "Socket.h"

#include <functional>

namespace libcpp
{
class EventLoop;

class UdpClient : public utils::noncopyable
{
public:
  using MessageCallback = std::function<void(Buffer*, TimeStamp)>;
  using RunCallback = std::function<void()>;
  
  UdpClient(EventLoop* loop, const InetAddress& serverAddr);
  ~UdpClient();

  void start();
  void send(const std::string& msg);

  void setRunCallback(const RunCallback& cb) 
  { runCallback_ = cb; }
  void setMessageCallback(const MessageCallback& cb)
  { messageCallback_ = cb; }
  
private:
  void handleRead(TimeStamp receivedTime);

private:
  EventLoop* loop_;
  Socket udpSocket_;
  Channel channel_;
  Buffer inputBuffer_;
  InetAddress serverAddr_;

  RunCallback runCallback_;
  MessageCallback messageCallback_;
};

}

#endif