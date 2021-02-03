#ifndef LIBCPP_KCPSERVER_H_
#define LIBCPP_KCPSERVER_H_

#include "KcpLayer.h"
#include "utils/noncopyable.h"

#include <string>
#include <set>

namespace libcpp
{
class EventLoop;

class KcpServer : public utils::noncopyable
{
public:
  using DatagramBuffer = FixedBuffer<kPacketBuffer>;
  KcpServer(EventLoop* loop, const InetAddress& addr,
            const std::string& name = "KcpServer");
  virtual ~KcpServer();

  void start();
  void send(const std::string& msg);

private:
  void handleRead(TimeStamp receiveTime);
  void handleConnection();

private:
  virtual void onConnection() = 0;
  virtual void onMessage(DatagramBuffer* buffer) = 0;

private:
  KcpLayer kcp_;
  const std::string name_;

  std::set<IUINT32> connections_;
  /* This is which conv we now communicate with */
  IUINT32 curConv_;
  DatagramBuffer datagram_;
};

}

#endif