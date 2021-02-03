#include "UdpClient.h"
#include "reactor/EventLoop.h"
#include "logging/Logging.h"

#include <sys/types.h>
#include <sys/socket.h>

using namespace libcpp;
using namespace std::placeholders;

UdpClient::UdpClient(EventLoop* loop, const InetAddress& serverAddr)
  : loop_(loop),
    udpSocket_(sockets::createSockfdNonBlockingOrDie(sockets::SOCK_TYPE_UDP)),
    channel_(loop, udpSocket_.fd()),
    serverAddr_(serverAddr)
{
  channel_.setReadCallback(
                std::bind(&UdpClient::handleRead, this, _1));
  channel_.enableReading();
  //channel_.enableWriting();
}

UdpClient::~UdpClient()
{

}

void UdpClient::start()
{
  if (runCallback_)
    loop_->runInLoop(runCallback_);
}


void UdpClient::send(const std::string& msg)
{
  LOG_TRACE << serverAddr_.toHostPort() << " " << msg;
  /*
   * ssize_t sendto(int sockfd, const void *buf, size_t len, int flags,
   *            const struct sockaddr *dest_addr, socklen_t addrlen);
   */
  // FIXME : if send more data than peer can receive?
  ssize_t n = ::sendto(udpSocket_.fd(), msg.data(), msg.size(), 0,
                      (const struct sockaddr*) &serverAddr_.getSockAddrInet(), 
                      sizeof serverAddr_);
  if (n >= 0) {
    if (static_cast<size_t>(n) < msg.size()) {
      LOG_TRACE << "[UdpServer] There is more data to be sent";
    }
  }
  else {
    LOG_SYSERR << "[UdpServer] sendto error";
  }
}


void UdpClient::handleRead(TimeStamp receivedTime)
{
  struct sockaddr_in addr;
  memset(&addr, 0, sizeof addr);
  socklen_t addrLen;

  // FIXME: if remaining space doesn't enough?
  const size_t writable = inputBuffer_.writableBytes();
  const ssize_t n = ::recvfrom(udpSocket_.fd(), inputBuffer_.beginWrite(), writable, 
                              MSG_WAITALL, (struct sockaddr*) &addr, &addrLen);
  
  if (n < 0) {
    LOG_SYSERR << "[UdpClient] handleRead error";
  } else if (static_cast<size_t>(n) <= writable) {
    inputBuffer_.hasWritten(n);
    messageCallback_(&inputBuffer_, receivedTime);
  }
}



