#ifndef LIBCPP_REACTOR_INETADDRESS_H_
#define LIBCPP_REACTOR_INETADDRESS_H_

#include <string>
#include <netinet/in.h>

namespace libcpp
{

class InetAddress
{
public:
  /* server side use */
  explicit InetAddress(uint16_t port);
  
  /* 
   * IP Address - in dotted decimal format "1.2.3.4"
   */
  InetAddress(const std::string& ip, uint16_t port);
  
  InetAddress(const struct sockaddr_in& addr) 
    : addr_(addr)
  {}
  
  std::string toHostPort() const;
  void setSockAddrInet(const struct sockaddr_in& addr) { addr_ = addr; }
  const struct sockaddr_in& getSockAddrInet() const { return addr_; }

private:
  struct sockaddr_in addr_;
};

}

#endif