#include "reactor/EventLoop.h"
#include "TcpServer.h"
#include "Acceptor.h"
#include "reactor/EventLoopGroup.h"

#include "logging/Logging.h"

#include <string>
#include <stdio.h>
#include <assert.h>

using namespace libcpp;
using namespace std::placeholders;

TcpServer::TcpServer(EventLoop* loop, const InetAddress& addr, 
                    const std::string& name)
  : loop_(loop),
    name_(name),
    acceptor_(new Acceptor(loop, addr)),
    loops_(new EventLoopGroup(loop)),
    started_(false),
    nextConnId_(1)
{
  using namespace std::placeholders;
  acceptor_->setNewConnCallback(
        std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer()
{
}

void TcpServer::setThreadNum(int numThreads)
{
  assert(numThreads >= 0);
  loops_->setThreadNum(numThreads);
}

void TcpServer::start()
{
  if (!started_) {
    started_ = true;
    loops_->start();
  }
  
  if (!acceptor_->listening()) {
    loop_->runInLoop(std::bind(&Acceptor::listen, acceptor_.get()));
  }
}

void TcpServer::newConnection(int sockfd, const InetAddress& peerAddr)
{
  loop_->assertInLoopThread();
  char buf[32];
  ::snprintf(buf, sizeof(buf), "#%d", nextConnId_);
  ++nextConnId_;
  std::string connName = name_ + buf;
  
  LOG_INFO << "TcpServer [" << name_ << "] - newConnection ["
            << connName << "] from " << peerAddr.toHostPort();
  
  InetAddress localAddr(sockets::getLocalAddr(sockfd));
  /* create Tcp connection */
  EventLoop* ioLoop = loops_->getNextLoop();
  TcpConnSptr conn = std::make_shared<TcpConnection>(ioLoop, 
                                  connName, sockfd, localAddr, peerAddr);
  connections_[connName] = conn;
  conn->setConnectionCallback(connectionCallback_);
  conn->setMessageCallback(messageCallback_);
  conn->setCloseCallback(std::bind(&TcpServer::removeConnection, this, _1));
  
  ioLoop->runInLoop(
    std::bind(&TcpConnection::connectEstablished, conn));
}


void TcpServer::removeConnection(const TcpConnSptr& conn)
{
  loop_->runInLoop(
    std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(const TcpConnSptr& conn)
{
  loop_->assertInLoopThread();
  LOG_INFO << "TcpServer::removeConnection [" << name_
           << "] - connection " << conn->name();
  size_t n = connections_.erase(conn->name());
  assert(n == 1); (void)n;
  EventLoop* ioLoop = conn->getLoop();
  ioLoop->queueInLoop(
      std::bind(&TcpConnection::connectDestroyed, conn));
}
