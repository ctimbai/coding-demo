#ifndef LIBCPP_APP_HTTPCLIENT_H_
#define LIBCPP_APP_HTTPCLIENT_H_

#include "utils/noncopyable.h"
#include "net/TcpClient.h"
#include "HttpSession.h"

#include <memory>

namespace libcpp
{
class EventLoop;

class HttpClient : public utils::noncopyable
{
public:
  HttpClient(EventLoop* loop, const InetAddress& serverAddr);
  virtual ~HttpClient();

  void start();

  void setOnHeadersCallback(const HttpCallback& cb)
  { onHeadersCallback_ = cb; }
  void setOnMessageCallback(const HttpCallback& cb)
  { onMessageCallback_ = cb; }

private:
  virtual void onConnection(const TcpConnSptr& conn);
  virtual void onMessage(const TcpConnSptr&, Buffer*, TimeStamp);

protected:
  std::shared_ptr<HttpSession> session_;
private:
  TcpClient client_;
  
  HttpCallback onHeadersCallback_;
  HttpCallback onMessageCallback_;
};

}

#endif
