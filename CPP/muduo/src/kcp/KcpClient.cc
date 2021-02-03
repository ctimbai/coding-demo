#include "KcpClient.h"
#include "reactor/EventLoop.h"

#include "logging/Logging.h"

using namespace libcpp;
using namespace std::placeholders;

KcpClient::KcpClient(EventLoop* loop, const InetAddress& serverAddr,
                      const std::string& name)
  : loop_(loop),
    kcp_(loop, serverAddr),
    name_(name)
{
  kcp_.setMessageCallback(
        std::bind(&KcpClient::handleRead, this, _1));
  kcp_.setConnectionCallback(
        std::bind(&KcpClient::handleConnection, this));
}

KcpClient::~KcpClient()
{

}


void KcpClient::connect()
{
  loop_->runInLoop([this]{ this->kcp_.connect(); });
}

void KcpClient::send(const std::string& msg)
{
  kcp_.send(curConv_, msg);
}

void KcpClient::handleRead(TimeStamp receiveTime)
{
  datagram_.reset();
  int n = kcp_.receive(&curConv_, datagram_.current(), datagram_.avail());
  datagram_.add(n);
  LOG_TRACE << "receive " << datagram_.length() << " bytes";
  onMessage(&datagram_);
}

void KcpClient::handleConnection()
{
  int64_t conv = kcp_.getConnectionConv();
  if (conv > 0) {
    curConv_ = static_cast<IUINT32>(conv);
    onConnection();
  }
  else {
    /* close connection */
  }
}

