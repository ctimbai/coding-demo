#ifndef LIBCPP_KCPCLIENT_H_
#define LIBCPP_KCPCLIENT_H_

#include "KcpLayer.h"
#include "utils/noncopyable.h"

#include <string>

namespace libcpp
{
class EventLoop;

class KcpClient : public utils::noncopyable
{
public:
  using DatagramBuffer = FixedBuffer<kPacketBuffer>;
  KcpClient(EventLoop* loop, const InetAddress& serverAddr,
              const std::string& name = "KcpClient");
  virtual ~KcpClient();

  void connect();
  void send(const std::string& msg);
  
private:
  void handleRead(TimeStamp receiveTime);
  void handleConnection();

private:
  virtual void onConnection() = 0;
  virtual void onMessage(DatagramBuffer* buffer) = 0;

private:
  EventLoop* loop_;
  KcpLayer kcp_;
  const std::string name_;
  IUINT32 curConv_;
  DatagramBuffer datagram_;
};

}

#endif