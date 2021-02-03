#include "net/TcpServer.h"
#include "reactor/EventLoop.h"
#include "net/InetAddress.h"
#include <stdio.h>
#include <unistd.h>

void onConnection(const libcpp::TcpConnPtr& conn)
{
  if (conn->connected())
  {
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
  printf("onMessage(): received %zd bytes from connection [%s] at %s\n",
         buf->readableBytes(),
         conn->name().c_str(),
         receiveTime.toFormattedString().c_str());

  conn->send(buf->retrieveAsString());
}

int main()
{
  printf("main(): pid = %d\n", getpid());

  libcpp::InetAddress listenAddr(9981);
  libcpp::EventLoop loop;

  libcpp::TcpServer server(&loop, listenAddr);
  server.setConnectionCallback(onConnection);
  server.setMessageCallback(onMessage);
  server.start();

  loop.loop();
}
