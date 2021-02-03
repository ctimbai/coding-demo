#include "net/UdpServer.h"
#include "net/InetAddress.h"
#include "reactor/EventLoop.h"
#include "datetime/TimeStamp.h"
#include "logging/Logging.h"

using namespace libcpp;
using namespace std::placeholders;

class UdpEchoServer
{
public:
  UdpEchoServer(EventLoop* loop, const InetAddress& addr)
    : server_(loop, addr)
  {
    server_.setMessageCallback(
        std::bind(&UdpEchoServer::onMessage, this, _1, _2, _3));
  }

private:
  void onMessage(const InetAddress& peerAddr, Buffer* buf, TimeStamp receivedTime)
  {
    std::string str = buf->retrieveAsString();
    LOG_TRACE << "received " << str;
    server_.sendto(str, peerAddr);
  }

private:
  UdpServer server_;
};

int main()
{
  EventLoop loop;
  InetAddress addr(9981);
  UdpEchoServer server(&loop, addr);

  loop.loop();
}