/*
 * Description: test Acceptor
 * 
 * another terminal use 'netcat <ip> <port>'
 */
#include "reactor/Acceptor.h"
#include "reactor/EventLoop.h"
#include "reactor/InetAddress.h"
#include "reactor/Socket.h"
#include <stdio.h>
#include <unistd.h>


void newConnection(int sockfd, const libcpp::InetAddress& peerAddr)
{
  printf("newConnection(): accepted a new connection from %s\n",
         peerAddr.toHostPort().c_str());
  ::write(sockfd, "How are you?\n", 13);
  libcpp::sockets::close(sockfd);
}

int main()
{
  libcpp::InetAddress listenAddr(9981);
  libcpp::EventLoop loop;

  libcpp::Acceptor acceptor(&loop, listenAddr);
  acceptor.setNewConnCallback(newConnection);
  acceptor.listen();
  
  loop.loop();
}