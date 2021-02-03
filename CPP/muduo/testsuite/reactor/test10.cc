#include "reactor/Connector.h"
#include "reactor/EventLoop.h"

#include <stdio.h>

libcpp::EventLoop* g_loop;

void connectCallback(int sockfd)
{
  printf("connected.\n");
  g_loop->quit();
}

int main(int argc, char* argv[])
{
  libcpp::EventLoop loop;
  g_loop = &loop;
  libcpp::InetAddress addr("127.0.0.1", 9981);
  libcpp::ConnectorPtr connector(new libcpp::Connector(&loop, addr));
  connector->setNewConnectionCallback(connectCallback);
  connector->start();

  loop.loop();
}