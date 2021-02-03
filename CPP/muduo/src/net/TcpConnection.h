#ifndef LIBCPP_REACTOR_TCPCONNECTION_H_
#define LIBCPP_REACTOR_TCPCONNECTION_H_

#include "InetAddress.h"
#include "io/Buffer.h"
#include "datetime/TimeStamp.h"

#include <map>
#include <string>
#include <functional>
#include <memory>

namespace libcpp
{
class EventLoop;
class TcpConnection;
class Socket;
class Channel;

using TcpConnSptr = std::shared_ptr<TcpConnection>;
using ConnectionCallback = std::function<void(const TcpConnSptr&)>;
using MessageCallback = std::function<void(const TcpConnSptr&,
                            Buffer*,
                            TimeStamp)>;
using CloseCallback = std::function<void(const TcpConnSptr&)>;
using WriteCompleteCallback = std::function<void(const TcpConnSptr&)>;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
  TcpConnection(EventLoop* loop,
                const std::string& name,
                int sockfd,
                const InetAddress& localAddr,
                const InetAddress& peerAddr);
                
  ~TcpConnection();
  
  bool connected() const { return state_ == kConnected; }
  const std::string& name() const { return name_; }
  const InetAddress& peerAddress() const { return peerAddr_; }
  const InetAddress& localAddress() const { return localAddr_; }
  EventLoop* getLoop() const { return loop_; }
  
  void setConnectionCallback(const ConnectionCallback& cb)
  { connectionCallback_ = cb; }

  void setMessageCallback(const MessageCallback& cb)
  { messageCallback_ = cb; }
  
  void setCloseCallback(const CloseCallback& cb)
  { closeCallback_ = cb; }
  
  void setWriteCompleteCallback(const WriteCompleteCallback& cb)
  { writeCompleteCallback_ = cb; }
  
  void connectEstablished();
  void connectDestroyed();
  
  void send(const std::string& msg);
  void shutdown();

private:
  enum ConnState {kConnecting, kConnected, kDisconnecting, kDisconnected};
  
  void handleRead(TimeStamp);
  void handleWrite();
  void handleClose();
  void handleError();
  void sendInLoop(const std::string& msg);
  void shutdownInLoop();
  
  void setConnState(ConnState s) { state_ = s; }
  
  EventLoop* loop_;
  const std::string name_;
  ConnState state_;
  std::unique_ptr<Socket> socket_;
  std::unique_ptr<Channel> channel_;
  InetAddress localAddr_;
  InetAddress peerAddr_;
  Buffer inputBuffer_;
  Buffer outputBuffer_;
  
  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  CloseCallback closeCallback_;
  WriteCompleteCallback writeCompleteCallback_;
};


}

#endif