#include "TcpClient.h"
#include "reactor/EventLoop.h"
#include "Socket.h"
#include "logging/Logging.h"

#include <functional>
#include <assert.h>
#include <stdio.h>

namespace libcpp
{
namespace detail
{

void removeConnection(EventLoop* loop, const TcpConnSptr& conn)
{
  loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

void removeConnector(const ConnectorPtr& connector)
{
  //connector->
}

}
}

using namespace libcpp;
using namespace std::placeholders;

TcpClient::TcpClient(EventLoop* loop, const InetAddress& serverAddr)
  : loop_(loop),
    connector_(new Connector(loop, serverAddr)),
    retry_(false),
    connect_(true),
    nextConnId_(1)
{
  connector_->setNewConnectionCallback(
    std::bind(&TcpClient::newConnection, this, _1));
  LOG_INFO << "TcpClient::TcpClient[" << this
           << "] - connector " << connector_.get();
}


TcpClient::~TcpClient()
{
  LOG_INFO << "TcpClient::~TcpClient[" << this
           << "] - connector " << connector_.get();
  TcpConnSptr conn;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    conn = connection_;
  }
  
  if (conn) {
    // why we need to reset TcpConnection's CloseCallback?
    // FIXME: not 100% safe, if we are in different thread
    CloseCallback cb = std::bind(&detail::removeConnection, loop_, _1);
    loop_->runInLoop(
        std::bind(&TcpConnection::setCloseCallback, conn, cb));
  }
  else {
    connector_->stop();
    // FIXME: HACK
    loop_->runAfter(1, std::bind(&detail::removeConnector, connector_));
  }
}

void TcpClient::connect()
{
  LOG_INFO << "TcpClient::connect[" << this << "] - connecting to "
           << connector_->serverAddress().toHostPort();
  connect_ = true;
  /* when connector create a connection successfully, call newConnection */
  connector_->start();
}

void TcpClient::disconnect()
{
  connect_ = false;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    if (connection_) {
      connection_->shutdown();
    }
  }
}

void TcpClient::stop()
{
  connect_ = false;
  connector_->stop();
}

void TcpClient::newConnection(int sockfd)
{
  loop_->assertInLoopThread();
  InetAddress peerAddr(sockets::getPeerAddr(sockfd));
  char buf[32];
  ::snprintf(buf, sizeof buf, ":%s#%d", 
              peerAddr.toHostPort().c_str(), nextConnId_);
  ++nextConnId_;
  std::string connName = buf;
  
  InetAddress localAddr(sockets::getLocalAddr(sockfd));
  TcpConnSptr conn(new TcpConnection(loop_, 
                                   connName,
                                   sockfd,
                                   localAddr,
                                   peerAddr));

  conn->setConnectionCallback(connectionCallback_);
  conn->setMessageCallback(messageCallback_);
  conn->setWriteCompleteCallback(writeCompleteCallback_);
  conn->setCloseCallback(
      std::bind(&TcpClient::removeConnection, this, _1));
  
  // why we need lock?
  {
    std::lock_guard<std::mutex> lock(mutex_);
    connection_ = conn;
  }
  
  conn->connectEstablished();
}

void TcpClient::removeConnection(const TcpConnSptr& conn)
{
  loop_->assertInLoopThread();
  assert(loop_ == conn->getLoop());

  {
    std::lock_guard<std::mutex> lock(mutex_);
    assert(connection_ == conn);
    connection_.reset();
  }

  loop_->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
  if (retry_ && connect_) {
    LOG_INFO << "TcpClient::connect[" << this << "] - Reconnecting to "
             << connector_->serverAddress().toHostPort();
    connector_->restart();
  }
}


