#include "kcp/KcpClient.h"
#include "reactor/EventLoop.h"
#include "logging/Logging.h"
#include "net/InetAddress.h"

#include <string>

using namespace libcpp;

class KcpClientEcho : public KcpClient
{
public:
  KcpClientEcho(EventLoop* loop, const InetAddress& addr)
    : KcpClient(loop, addr)
  {}
  ~KcpClientEcho() {}

private:
  void onConnection() override
  {
    send(std::string("Hello! I'm KCP Client"));
    LOG_TRACE << "Client send MSG";
  }

  void onMessage(DatagramBuffer* buffer) override 
  {
    LOG_TRACE << buffer->string();
    send(buffer->string());
  }
};

int main() 
{
  EventLoop loop;

  InetAddress addr("127.0.0.1", 9981);
  KcpClientEcho client(&loop, addr);
  client.connect();

  loop.loop();
}