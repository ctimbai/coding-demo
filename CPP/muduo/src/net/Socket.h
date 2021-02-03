#ifndef LIBCPP_REACTOR_SOCKET_H_
#define LIBCPP_REACTOR_SOCKET_H_

#include "InetAddress.h"
#include <netinet/in.h>

namespace libcpp
{

class Socket
{
public:
  explicit Socket(int sockfd) : sockfd_(sockfd)
  {}
  
  ~Socket();
  
  void bind(const InetAddress& laddr);
  void listen();
  
  int accept(InetAddress* peerAddr);
  void shutdownWrite();
  
  void setReuseAddr(bool on);
  
  int fd() const { return sockfd_; }
  
private:
  const int sockfd_;
};

/*
 * sockets operations utilities
 */
namespace sockets
{
inline uint16_t ntoh16(uint16_t net16)
{
  return ::ntohs(net16);
}

inline uint16_t hton16(uint16_t host16)
{
  return ::htons(host16);
}

inline uint32_t ntoh32(uint32_t net32)
{
  return ::ntohl(net32);
}

inline uint32_t hton32(uint32_t host32)
{
  return ::htonl(host32);
}

enum Protocol
{
  SOCK_TYPE_TCP,
  SOCK_TYPE_UDP
};

/* create tcp/udp socket */
int createSockfdNonBlockingOrDie(Protocol pro);
int connect(int sockfd, const struct sockaddr_in& addr);
void bindOrDie(int sockfd, const struct sockaddr_in& addr);
void listenOrDie(int sockfd);
int accept(int sockfd, struct sockaddr_in* addr);
void close(int sockfd);
void shutdownWrite(int sockfd);

void fromHostPort(const char* ip, uint16_t port,
                  struct sockaddr_in* addr);
void toHostPort(char* buf, size_t size, 
                const struct sockaddr_in& addr);

struct sockaddr_in getLocalAddr(int sockfd);
struct sockaddr_in getPeerAddr(int sockfd);

int getSocketError(int sockfd);
bool isSelfConnect(int sockfd);
}

}

#endif