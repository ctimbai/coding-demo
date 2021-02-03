#ifndef LIBCPP_APP_HIMCLIENT_H_
#define LIBCPP_APP_HIMCLIENT_H_

#include "utils/noncopyable.h"
#include "http/HttpClient.h"
#include "reactor/Channel.h"

#include <string>
#include <map>

class HimClient : public libcpp::HttpClient
{
public:
  HimClient(libcpp::EventLoop* loop, std::string& uname, std::string& pwd);
  ~HimClient();

  void printGuidance();

private:
  void onConnection(const libcpp::TcpConnSptr& conn) override;
  void sendLogin();

  int onHeaders(const libcpp::HttpSessionSptr& sess, libcpp::HttpMessage* msg);
  int onBody(const libcpp::HttpSessionSptr& sess, libcpp::HttpMessage* msg);

  void handleStdinRead();

private:
  libcpp::Channel userInputChannel_;
  std::string username_;
  std::string password_;
  bool chatting_;
  libcpp::Buffer userInputBuffer_;
  std::unique_ptr<libcpp::HttpMessage> userMsg_;
};

#endif
