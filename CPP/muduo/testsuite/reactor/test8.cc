#include "reactor/TcpServer.h"
#include "reactor/EventLoop.h"
#include "reactor/InetAddress.h"
#include "datetime/TimeStamp.h"
#include <stdio.h>
#include <iostream>
#include <thread>

void onConnection(const libcpp::TcpConnPtr& conn)
{
  if (conn->connected())
  {
    std::cout << "thread id = " << std::this_thread::get_id() << " ";
    printf("onConnection(): new connection [%s] from %s\n",
           conn->name().c_str(),
           conn->peerAddress().toHostPort().c_str());
  }
  else
  {
    printf("onConnection(): connection [%s] is down\n",
           conn->name().c_str());
  }
}

void onMessage(const libcpp::TcpConnPtr& conn,
               libcpp::Buffer* buf,
               libcpp::TimeStamp receiveTime)
{
  std::cout << "thread id = " << std::this_thread::get_id() << " ";
  printf("onMessage(): received %zd bytes from connection [%s]\n",
         buf->readableBytes(), conn->name().c_str());
}

int main()
{
  printf("main()\n");

  libcpp::InetAddress listenAddr(9981);
  libcpp::EventLoop loop;

  libcpp::TcpServer server(&loop, listenAddr);
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  server.setThreadNum(4);
  server.start();

  loop.loop();
}
