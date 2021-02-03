#include "kcp/KcpServer.h"
#include "reactor/EventLoop.h"
#include "logging/Logging.h"
#include "net/InetAddress.h"

#include <string>

using namespace libcpp;

class KcpServerEcho : public KcpServer
{
public:
  KcpServerEcho(EventLoop* loop, const InetAddress& addr)
    : KcpServer(loop, addr)
  {}
  ~KcpServerEcho() {}

private:
  void onConnection() override
  {
  }

  void onMessage(DatagramBuffer* buffer) override 
  {
    LOG_TRACE << buffer->string();
    send(std::string("Hello! I'm KCP server"));
  }
};

int main() 
{
  EventLoop loop;

  InetAddress addr(9981);
  KcpServerEcho server(&loop, addr);
  server.start();

  loop.loop();
}