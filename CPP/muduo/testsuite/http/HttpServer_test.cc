#include "http/HttpServer.h"
#include "logging/Logging.h"
#include "reactor/EventLoop.h"
#include "net/InetAddress.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

using namespace libcpp;


int onHeaders(const HttpSessionSptr& sess, HttpMessage* msg)
{
  std::string path = ".";
  path += msg->getPath();
  LOG_TRACE << "file path = " << path;
  int fd = open(path.data(), O_RDONLY);

  char buf[512];
  memset(buf, 0, 512);
  ssize_t n = read(fd, buf, 512);

  HttpMessage resp;
  resp.appendBody(buf);
  resp.addHeader("Content-Type", "text/html");
  resp.addHeader("Content-Length", std::to_string(n));
  resp.addHeader("Connection", "close");
  // std::cout << msg->getRequestAsString() << "\n";
  // std::cout << resp.getResponseAsString() << "\n";
  // std::cout << "-------\n";
  sess->send(resp.getResponseAsString());
  sess->shutdown();
  return 0;
}

int main()
{
  Logger::setLogLevel(Logger::FATAL);
  InetAddress addr(9981);
  EventLoop loop;

  HttpServer server(&loop, addr);
  server.setOnHeadersCallback(onHeaders);
  server.setThreadNum(1);
  server.start();

  loop.loop();
}
