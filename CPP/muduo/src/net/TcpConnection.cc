#include "TcpConnection.h"
#include "reactor/EventLoop.h"
#include "Socket.h"
#include "reactor/Channel.h"

#include "logging/Logging.h"

#include <unistd.h>
#include <assert.h>
#include <errno.h>

using namespace libcpp;


TcpConnection::TcpConnection(EventLoop* loop,
                      const std::string& name,
                      int sockfd,
                      const InetAddress& localAddr,
                      const InetAddress& peerAddr)
  : loop_(loop),
    name_(name),
    state_(kConnecting),
    socket_(new Socket(sockfd)),
    channel_(new Channel(loop, sockfd)),
    localAddr_(localAddr),
    peerAddr_(peerAddr)
{
  using namespace std::placeholders;
  LOG_DEBUG << "TcpConnection::ctor[" <<  name_ << "] at " << this
            << " fd=" << sockfd;
  channel_->setReadCallback(
      std::bind(&TcpConnection::handleRead, this, _1));
  channel_->setCloseCallback(
      std::bind(&TcpConnection::handleClose, this));
  channel_->setWriteCallback(
      std::bind(&TcpConnection::handleWrite, this));
  channel_->setErrorCallback(
      std::bind(&TcpConnection::handleError, this));
}


TcpConnection::~TcpConnection()
{
  LOG_DEBUG << "TcpConnection::dtor[" <<  name_ << "] at " << this
            << " fd=" << channel_->fd();
}


void TcpConnection::send(const std::string& msg)
{
  if (state_ == kConnected) {
    sendInLoop(msg);
  }
  else {
    loop_->runInLoop(
        std::bind(&TcpConnection::sendInLoop, this, msg));
  }
}


void TcpConnection::sendInLoop(const std::string& msg)
{
  loop_->assertInLoopThread();
  ssize_t nwrote = 0;
  
  // if output queue empty, send immediately
  if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
    nwrote = ::write(channel_->fd(), msg.data(), msg.size());
    if (nwrote >= 0) {
      if (static_cast<size_t>(nwrote) < msg.size()) {
        LOG_TRACE << "[" << name_ << "] more data need to be sent";
      }
      else if (writeCompleteCallback_) {
        loop_->runInLoop(
          std::bind(writeCompleteCallback_, shared_from_this()));
      }
    }
    else {
      nwrote = 0;
      if (errno != EWOULDBLOCK) {
        LOG_ERROR << "[" << name_ << "] TcpConnection::sendInLoop";
      }
    }
  }
  
  // put remain data into output buffer, 
  // and enable writting event
  if (static_cast<size_t>(nwrote) < msg.size()) {
    outputBuffer_.append(msg.data()+nwrote, msg.size()-nwrote);
    if (!channel_->isWriting()) {
      channel_->enableWriting();
    }
  }
}

void TcpConnection::connectEstablished()
{
  loop_->assertInLoopThread();
  assert(state_ == kConnecting);
  setConnState(kConnected);
  channel_->enableReading();

  connectionCallback_(shared_from_this());
}

void TcpConnection::handleRead(TimeStamp receiveTime)
{
  int savedErrno = 0;
  ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
  if (n > 0) {
    messageCallback_(shared_from_this(), &inputBuffer_, receiveTime);
  }
  else if (n == 0) {
    handleClose();
  }
  else {
    handleError();
  }
}

void TcpConnection::handleWrite()
{
  loop_->assertInLoopThread();
  if (channel_->isWriting()) {
    ssize_t n = ::write(channel_->fd(), 
        outputBuffer_.data(), outputBuffer_.readableBytes());
    if (n > 0) {
      outputBuffer_.retrieve(n);
      if (outputBuffer_.readableBytes() == 0) {
        channel_->disableWriting();
        if (writeCompleteCallback_) {
          loop_->runInLoop(
            std::bind(writeCompleteCallback_, shared_from_this()));
        }
        if (state_ == kDisconnecting) {
          shutdownInLoop();
        }
      }
      else {
        LOG_TRACE << "[" << name_ << "] more data need to be sent";
      }
    }
    else {
      LOG_ERROR << "[" << name_ << "] TcpConnection::handleWrite";
    }
  }
  else {
    LOG_TRACE << "Connection is down, no more writing";
  }
}

void TcpConnection::handleClose()
{
  loop_->assertInLoopThread();
  LOG_TRACE << "TcpConnection::handleClose [" << name_ << "] "
            << "state = " << state_;
  assert(state_ == kConnected || state_ == kDisconnecting);
  channel_->disableAllEvents();
  closeCallback_(shared_from_this());
}

void TcpConnection::handleError()
{
  int err = sockets::getSocketError(channel_->fd());
  errno = err;
  LOG_ERROR << "TcpConnection::handleError [" << name_
            << "] - SO_ERROR";
}


void TcpConnection::shutdown()
{
  if (state_ == kConnected) {
    setConnState(kDisconnecting);
    loop_->runInLoop(
        std::bind(&TcpConnection::shutdownInLoop, this));
  }
}

void TcpConnection::shutdownInLoop()
{
  loop_->assertInLoopThread();
  if(!channel_->isWriting()) {
    socket_->shutdownWrite();
  }
}


void TcpConnection::connectDestroyed()
{
  loop_->assertInLoopThread();
  assert(state_ == kConnected || state_ == kDisconnecting);
  setConnState(kDisconnected);
  channel_->disableAllEvents();
  connectionCallback_(shared_from_this());
  
  loop_->removeChannel(channel_.get());
}
