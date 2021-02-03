#include "HimServer.h"
#include "reactor/EventLoop.h"
#include "logging/Logging.h"

using namespace libcpp;
using namespace std;

int main()
{
  Logger::setLogLevel(Logger::ERROR);
  EventLoop loop;
  InetAddress addr(9981);
  HimServer server(&loop, addr);
  server.start();
  loop.loop();
  return 0;
}
