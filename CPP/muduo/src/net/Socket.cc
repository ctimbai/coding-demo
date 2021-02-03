#include "Socket.h"
#include "logging/Logging.h"


#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

using namespace libcpp;

Socket::~Socket()
{
  sockets::close(sockfd_);
}

void Socket::bind(const InetAddress& addr)
{
  sockets::bindOrDie(sockfd_, addr.getSockAddrInet());
}

void Socket::listen()
{
  sockets::listenOrDie(sockfd_);
}

int Socket::accept(InetAddress* peerAddr)
{
  struct sockaddr_in addr;
  ::bzero(&addr, sizeof(addr));
  // FIXME: wrapper connfd for RAII
  int connfd = sockets::accept(sockfd_, &addr);
  if (connfd > 0) {
    peerAddr->setSockAddrInet(addr);
  }
  return connfd;
}


void Socket::shutdownWrite()
{
  sockets::shutdownWrite(sockfd_);
}

void Socket::setReuseAddr(bool on)
{
  /* refer to 
   * https://www.cnblogs.com/mydomain/archive/2011/08/23/2150567.html 
   */
  int optval = on ? 1 : 0;
  ::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
               &optval, sizeof optval);
}


/* sockets operation utilities ********************************/

namespace
{
typedef struct sockaddr SA;

const SA* sockaddr_cast(const struct sockaddr_in* addr)
{
  return static_cast<const SA*>((const void*) addr);
}

SA* sockaddr_cast(struct sockaddr_in* addr)
{
  return static_cast<SA*>((void*) addr);
}

}

int sockets::createSockfdNonBlockingOrDie(Protocol pro)
{
  /*
   * refer to http://man7.org/linux/man-pages/man2/socket.2.html
   * int socket(int domain, int type, int protocol);
   */
  int sockfd;
  if (pro == SOCK_TYPE_TCP) {
    sockfd = ::socket(AF_INET,
                    SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                    IPPROTO_TCP);
  }
  else {
    sockfd = ::socket(AF_INET,
                    SOCK_DGRAM | SOCK_NONBLOCK | SOCK_CLOEXEC,
                    0);
  }
  
  if (sockfd < 0) {
    LOG_FATAL << "sockets::createSockfdNonBlockingOrDie";
  }
  return sockfd;
}

void sockets::bindOrDie(int sockfd, const struct sockaddr_in& addr)
{
  /*
   * int bind(int sockfd, const struct sockaddr *addr,
   *            socklen_t addrlen);
   */
  // FIXME: sockaddr type cast
  int ret = ::bind(sockfd, sockaddr_cast(&addr), sizeof(addr));
  if (ret < 0) {
    LOG_FATAL << "sockets::bindOrDie";
  }
}

void sockets::listenOrDie(int sockfd)
{
  /*
   * int listen(int sockfd, int backlog);
   * SOMAXCONN - 128
   */
  int ret = ::listen(sockfd, SOMAXCONN);
  if (ret < 0) {
    LOG_FATAL << "sockets::listenOrDie";
  }
}

int sockets::connect(int sockfd, const struct sockaddr_in& addr)
{
  return ::connect(sockfd, sockaddr_cast(&addr), sizeof addr);
}

int sockets::accept(int sockfd, struct sockaddr_in* addr)
{
  socklen_t addrlen = sizeof(*addr);
  /*
   * int accept4(int sockfd, struct sockaddr *addr,
   *               socklen_t *addrlen, int flags);
   */
  // FIXME: sockaddr type cast
  int connfd = ::accept4(sockfd, sockaddr_cast(addr), &addrlen,
                          SOCK_NONBLOCK | SOCK_CLOEXEC);
                          
  if (connfd < 0) {
    int savedErrno = errno;
    LOG_ERROR << "sockets::accept";
    switch (savedErrno) {
      case EAGAIN:
      case ECONNABORTED:
      case EINTR:
      case EPROTO: // ???
      case EPERM:
      case EMFILE: // per-process lmit of open file desctiptor ???
        // expected errors
        errno = savedErrno;
        break;
      case EBADF:
      case EFAULT:
      case EINVAL:
      case ENFILE:
      case ENOBUFS:
      case ENOMEM:
      case ENOTSOCK:
      case EOPNOTSUPP:
        // unexpected errors
        LOG_WARN << "unexpected error of ::accept " << savedErrno;
        break;
      default:
        LOG_WARN << "unknown error of ::accept " << savedErrno;
        break;
    }
  }
  return connfd;
}

void sockets::close(int sockfd)
{
  if (::close(sockfd) < 0) {
    LOG_ERROR << "sockets::close";
  }
}

void sockets::shutdownWrite(int sockfd)
{
  if (::shutdown(sockfd, SHUT_WR) < 0)
  {
    LOG_ERROR << "sockets::shutdownWrite";
  }
}


void sockets::fromHostPort(const char* ip, uint16_t port,
                  struct sockaddr_in* addr)
{
  addr->sin_family = AF_INET;
  addr->sin_port = hton16(port);
  /*
   * int inet_pton(int af, const char *src, void *dst);
   * This function converts the character string src into a network
   *   address structure in the af address family, then copies the network
   *   address structure to dst in network byte order
   * 
   * 1 - success, 0 - src invalid, -1 - af invalid
   */
  if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0) {
    LOG_ERROR << "sockets::fromHostPort";
  }
}

void sockets::toHostPort(char* buf, size_t size, 
                        const struct sockaddr_in& addr)
{
  char host[INET_ADDRSTRLEN] = "INVALID";
  /*
   * const char *inet_ntop(int af, const void *src,
   *                         char *dst, socklen_t size);
   */
  ::inet_ntop(AF_INET, &addr.sin_addr, host, sizeof(host));
  uint16_t port = sockets::ntoh16(addr.sin_port);
  ::snprintf(buf, size, "%s:%u", host, port);
}


struct sockaddr_in sockets::getLocalAddr(int sockfd)
{
  struct sockaddr_in localaddr;
  bzero(&localaddr, sizeof localaddr);
  socklen_t addrlen = sizeof(localaddr);
  /*
   * getsockname() returns the current address to which the socket sockfd
   *   is bound
   */
  if (::getsockname(sockfd, sockaddr_cast(&localaddr), &addrlen) < 0)
  {
    LOG_ERROR << "sockets::getLocalAddr";
  }
  return localaddr;
}

struct sockaddr_in sockets::getPeerAddr(int sockfd)
{
  struct sockaddr_in peeraddr;
  bzero(&peeraddr, sizeof peeraddr);
  socklen_t addrlen = sizeof(peeraddr);
  if (::getpeername(sockfd, sockaddr_cast(&peeraddr), &addrlen) < 0)
  {
    LOG_ERROR << "sockets::getPeerAddr";
  }
  return peeraddr;
}

int sockets::getSocketError(int sockfd)
{
  int optval;
  socklen_t optlen = sizeof optval;

  if (::getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &optval, &optlen) < 0) {
    return errno;
  }
  else {
    return optval;
  }
}

bool sockets::isSelfConnect(int sockfd)
{
  struct sockaddr_in localaddr = getLocalAddr(sockfd);
  struct sockaddr_in peeraddr = getPeerAddr(sockfd);
  return localaddr.sin_port == peeraddr.sin_port
      && localaddr.sin_addr.s_addr == peeraddr.sin_addr.s_addr;
}

