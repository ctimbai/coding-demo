#ifndef LIBCPP_KCPLAYER_H_
#define LIBCPP_KCPLAYER_H_

#include "ikcp.h"
#include "reactor/Channel.h"
#include "net/InetAddress.h"
#include "net/Socket.h"
#include "datetime/TimeStamp.h"
#include "io/Buffer.h"

#include <string>
#include <map>
#include <functional>

namespace libcpp
{
class EventLoop;
/*
 * KcpLayer takes the responsibility to manage tcp connection
 * and upper-level send data to different conn via conv id
 * 
 * when there is data received, we write eventfd to notify app
 */
class KcpLayer
{
public:
  using EventCallback = std::function<void(TimeStamp)>;
  KcpLayer(EventLoop* loop, const InetAddress& addr);
  ~KcpLayer();
  
  /* server-side call */
  void listen();
  /* client-side call */
  void connect();

  /* user/upper level send */
  void send(IUINT32 conv, const std::string& msg);
  /* user/upper level recv */
  int receive(IUINT32* conv, char* buf, int len);

  ssize_t recvUdpMsg(struct sockaddr_in*);
  ssize_t sendUdpMsg(IUINT32 conv, const char* buf, int len);
  static int output(const char* buf, int len, ikcpcb* kcp, void* user);

  void setMessageCallback(const EventCallback& cb);
  void setConnectionCallback(const EventCallback& cb);

  int64_t getConnectionConv();
  

private:
  void handleRead(TimeStamp receivedTime);

  // Note: eventfd buffer must be 8 bytes
  void notifyData(int64_t conv);
  void notifyConnection(int64_t conv);
  void kcpUpdate(ikcpcb* cb);

protected:
  EventLoop* loop_;

private:
  /* server-side and client-side InetAddress is different */
  InetAddress inetAddr_;
  Socket socket_;
  Channel channel_;
  /* when data arrive we need to call app recv */
  int dataFd_;
  Channel dataChannel_;
  /* when new connection arrive, call new connnection callback */
  int connFd_;
  Channel connChannel_;

private:
  /*
   * Here are member for kcp conn manage
   */
  // FIXME: use more buffer
  using DatagramBuffer = FixedBuffer<1500>;
  DatagramBuffer datagram_;

  IUINT32 nextConv_;

  struct KcpConnInfo
  {
    ikcpcb* kcpcb_;
    struct sockaddr_in peerAddr_;
  };

  using ConnectionMap = std::map<IUINT32, KcpConnInfo>;
  ConnectionMap connections_;
};

}

#endif