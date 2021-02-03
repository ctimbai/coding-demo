#ifndef LIBCPP_TCPCLIENT_H_
#define LIBCPP_TCPCLIENT_H_

#include <memory>
#include <mutex>
#include "TcpConnection.h"
#include "Connector.h"

namespace libcpp
{

class TcpClient
{
public:
  TcpClient(EventLoop* loop, const InetAddress& serverAddr);
  ~TcpClient();
  
  void connect();
  void disconnect();
  void stop();
  
  // for what?
  TcpConnSptr connection() const 
  {
    std::lock_guard<std::mutex> lock(mutex_);
    return connection_;
  }
  
  bool retry() const;
  void enableRetry() { retry_ = true; }
  
  void setConnectionCallback(const ConnectionCallback& cb)
  { connectionCallback_ = cb; }
  
  void setMessageCallback(const MessageCallback& cb)
  { messageCallback_ = cb; }
  
  void setWriteCompleteCallback(const WriteCompleteCallback& cb)
  { writeCompleteCallback_ = cb; }
  
private:
  void newConnection(int sockfd);
  void removeConnection(const TcpConnSptr& conn);
  
  EventLoop* loop_;
  ConnectorPtr connector_;  // why not unique_ptr?
  ConnectionCallback connectionCallback_;
  MessageCallback messageCallback_;
  WriteCompleteCallback writeCompleteCallback_;
  
  bool retry_;
  bool connect_;
  
  int nextConnId_;
  mutable std::mutex mutex_;
  TcpConnSptr connection_;
};

}

#endif