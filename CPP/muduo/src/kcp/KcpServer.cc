#include "KcpServer.h"
#include "logging/Logging.h"


using namespace libcpp;
using namespace std::placeholders;

KcpServer::KcpServer(EventLoop* loop, const InetAddress& addr,
                      const std::string& name)
  : kcp_(loop, addr),
    name_(name)
{
  kcp_.setMessageCallback(
        std::bind(&KcpServer::handleRead, this, _1));
  kcp_.setConnectionCallback(
        std::bind(&KcpServer::handleConnection, this));
}

KcpServer::~KcpServer()
{

}

void KcpServer::start()
{
  kcp_.listen();
}

void KcpServer::send(const std::string& msg)
{
  kcp_.send(curConv_, msg);
}

void KcpServer::handleRead(TimeStamp receiveTime)
{
  datagram_.reset();
  int n = kcp_.receive(&curConv_, datagram_.current(), datagram_.avail());
  datagram_.add(n);
  LOG_TRACE << "receive " << datagram_.length() << " bytes";
  onMessage(&datagram_);
}

void KcpServer::handleConnection()
{
  int64_t conv = kcp_.getConnectionConv();
  if (conv > 0) {
    curConv_ = static_cast<IUINT32>(conv);
    connections_.insert(curConv_);
    onConnection();
  }
  else {
    /* close connection */
  }
}