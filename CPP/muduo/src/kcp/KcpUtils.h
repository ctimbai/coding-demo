#ifndef LIBCPP_KCPUTILS_H_
#define LIBCPP_KCPUTILS_H_

#include <string.h>

namespace libcpp
{

const char* KCP_HANDSHAKE_SYN = "This is a kcp handshake syn";
const char* KCP_CLOSE_MSG = "I want to close the connection";
const char* KCP_HANDSHAKE_ACK = "Your conv is ";

inline bool isHandshakeSyn(const std::string& msg)
{
  return ::memcmp(msg.data(), KCP_HANDSHAKE_SYN, strlen(KCP_HANDSHAKE_SYN)) == 0;
}

inline bool isHandshakeAck(const std::string& msg)
{
  return ::memcmp(msg.data(), KCP_HANDSHAKE_ACK, strlen(KCP_HANDSHAKE_ACK)) == 0;
}

inline bool isClosePacket(const std::string& msg)
{
  return ::memcmp(msg.data(), KCP_CLOSE_MSG, strlen(KCP_CLOSE_MSG)) == 0;
}

}

#endif