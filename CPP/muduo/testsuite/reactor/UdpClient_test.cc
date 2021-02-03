#include "net/UdpClient.h"

#include "net/InetAddress.h"
#include "reactor/EventLoop.h"
#include "datetime/TimeStamp.h"
#include "logging/Logging.h"

using namespace libcpp;
using namespace std::placeholders;


class UdpEchoClient
{
public:
  UdpEchoClient(EventLoop* loop, const InetAddress& serverAddr)
    : serverAddr_(serverAddr),
      client_(loop, serverAddr)

  {
    client_.setRunCallback(
        std::bind(&UdpEchoClient::runFunc, this));
    client_.setMessageCallback(
        std::bind(&UdpEchoClient::onMessage,this, _1, _2, _3));
  }

  void start() 
  {
    client_.start();
  }

private:
  void runFunc()
  {
    LOG_TRACE << "runFunc";
    client_.sendto("Hello World!", serverAddr_);
  }

  void onMessage(const InetAddress& peerAddr, Buffer* buf, TimeStamp receivedTime)
  {
    LOG_TRACE << "received " << buf->retrieveAsString();
  }

private:
  InetAddress serverAddr_;
  UdpClient client_;
};

int main()
{
  EventLoop loop;
  InetAddress addr("127.0.0.1", 9981);
  UdpEchoClient client(&loop, addr);
  client.start();
  loop.loop();
}