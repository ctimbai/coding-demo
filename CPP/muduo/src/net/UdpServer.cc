#include "UdpServer.h"
#include "logging/Logging.h"

#include <sys/types.h>
#include <sys/socket.h>

using namespace libcpp;

UdpServer::UdpServer(EventLoop* loop, const InetAddress& addr,
                      const std::string& name)
  : loop_(loop),
    name_(name),
    udpSocket_(sockets::createSockfdNonBlockingOrDie(sockets::SOCK_TYPE_UDP)),
    channel_(loop, udpSocket_.fd())
{
  using namespace std::placeholders;
  udpSocket_.setReuseAddr(true);
  udpSocket_.bind(addr);
  channel_.setReadCallback(
          std::bind(&UdpServer::handleRead, this, _1));
  channel_.enableReading();
  //channel_.enableWriting(); // we can't always enable writing
  LOG_TRACE << "[UdpServer] start listening at " << addr.toHostPort();
}

UdpServer::~UdpServer()
{

}

void UdpServer::sendto(const std::string& msg, const InetAddress& peerAddr)
{
  /*
   * ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
   *            const struct sockaddr *dest_addr, socklen_t addrlen);
   */
  // FIXME : if send more data than peer can receive?
  ssize_t n = ::sendto(udpSocket_.fd(), msg.data(), msg.size(), 0,
                      (const struct sockaddr*) &peerAddr.getSockAddrInet(), 
                      sizeof peerAddr);
  if (n >= 0) {
    if (static_cast<size_t>(n) < msg.size()) {
      LOG_TRACE << "[UdpServer] There is more data to be sent";
    }
  }
  else {
    LOG_ERROR << "[UdpServer] sendto error";
  }
}


void UdpServer::handleRead(TimeStamp receivedTime)
{
  // FIXME: if udp server receives data more than it can handle, how to deal with?
  struct sockaddr_in cliAddr;
  memset(&cliAddr, 0, sizeof cliAddr);
  socklen_t addrLen;

  // FIXME: if remaining space doesn't enough?
  const size_t writable = inputBuffer_.writableBytes();
  const ssize_t n = ::recvfrom(udpSocket_.fd(), inputBuffer_.beginWrite(), writable, 
                              MSG_WAITALL, (struct sockaddr*) &cliAddr, &addrLen);
  InetAddress peerAddr(cliAddr);
  
  if (n < 0) {
    LOG_SYSERR << "[UdpServer] handleRead error";
  } else if (static_cast<size_t>(n) <= writable) {
    inputBuffer_.hasWritten(n);
    messageCallback_(peerAddr, &inputBuffer_, receivedTime);
  }
}

void UdpServer::handleWrite()
{
  if (channel_.isWriting()) {

  }
}