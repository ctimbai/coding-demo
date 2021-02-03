#include "InetAddress.h"
#include "Socket.h"
#include <string.h>

// refer to http://man7.org/linux/man-pages/man7/ip.7.html
//     /* Structure describing an Internet socket address.  */
//     struct sockaddr_in {
//         sa_family_t    sin_family; /* address family: AF_INET */
//         uint16_t       sin_port;   /* port in network byte order */
//         struct in_addr sin_addr;   /* internet address */
//     };

//     /* Internet address. */
//     typedef uint32_t in_addr_t;
//     struct in_addr {
//         in_addr_t       s_addr;     /* address in network byte order */
//     };

using namespace libcpp;

static const in_addr_t kInetAddrAny = INADDR_ANY;

InetAddress::InetAddress(uint16_t port)
{
  bzero(&addr_, sizeof(addr_));
  addr_.sin_family = AF_INET;
  addr_.sin_addr.s_addr = sockets::hton32(kInetAddrAny);
  addr_.sin_port = sockets::hton16(port);
}

InetAddress::InetAddress(const std::string& ip, uint16_t port)
{
  bzero(&addr_, sizeof addr_);
  sockets::fromHostPort(ip.c_str(), port, &addr_);
}


std::string InetAddress::toHostPort() const
{
  char buf[32];
  sockets::toHostPort(buf, sizeof buf, addr_);
  return buf;
}



