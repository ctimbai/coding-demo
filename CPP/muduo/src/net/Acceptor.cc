#include "Acceptor.h"
#include "Socket.h"
#include "reactor/EventLoop.h"

using namespace libcpp;


Acceptor::Acceptor(EventLoop* loop, const InetAddress& listenAddr)
  : loop_(loop),
    acceptSocket_(sockets::createSockfdNonBlockingOrDie(sockets::SOCK_TYPE_TCP)),
    acceptChannel_(loop, acceptSocket_.fd()),
    listening_(false)
{
  acceptSocket_.setReuseAddr(true);
  acceptSocket_.bind(listenAddr);
  acceptChannel_.setReadCallback(
          std::bind(&Acceptor::handleRead, this));
}


void Acceptor::listen()
{
  loop_->assertInLoopThread();
  listening_ = true;
  acceptSocket_.listen();
  acceptChannel_.enableReading();
}


void Acceptor::handleRead()
{
  loop_->assertInLoopThread();
  InetAddress peerAddr(0);
  int connfd = acceptSocket_.accept(&peerAddr);
  if (connfd >= 0) {
    if (newConnCallback_) newConnCallback_(connfd, peerAddr);
    else sockets::close(connfd);
  }
}

