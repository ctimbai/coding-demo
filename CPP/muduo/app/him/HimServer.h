#ifndef LIBCPP_APP_HIMSERVER_H_
#define LIBCPP_APP_HIMSERVER_H_

#include "http/HttpServer.h"

class HimServer : public libcpp::HttpServer
{
public:
  HimServer(libcpp::EventLoop* loop, const libcpp::InetAddress& addr);
  ~HimServer();

private:
  int onBody(const libcpp::HttpSessionSptr& sess, libcpp::HttpMessage* msg);
private:
  using UserSessionMap = std::map<std::string, libcpp::HttpSessionSptr>;

  UserSessionMap userSessionMap_;
};

#endif

