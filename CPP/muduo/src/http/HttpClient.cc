#include "HttpClient.h"
#include "reactor/EventLoop.h"

using namespace libcpp;

HttpClient::HttpClient(EventLoop* loop, const InetAddress& serverAddr)
  : client_(loop, serverAddr)
{
  using namespace std::placeholders;
  client_.setConnectionCallback(
      std::bind(&HttpClient::onConnection, this, _1));
  client_.setMessageCallback(
      std::bind(&HttpClient::onMessage, this, _1, _2, _3));
}

HttpClient::~HttpClient()
{
}

void HttpClient::start()
{
  client_.enableRetry();
  client_.connect();
}

void HttpClient::onMessage(const TcpConnSptr& conn, Buffer* buffer, 
                            TimeStamp receivedTime)
{
  session_->execute(buffer->data(), buffer->readableBytes());
  buffer->retrieveAll();
  (void)conn;
  (void)receivedTime;
}

void HttpClient::onConnection(const TcpConnSptr& conn)
{
  if (conn->connected()) {
    session_.reset(new HttpSession(conn, HTTP_RESPONSE));
    session_->setOnHeadersCallback(onHeadersCallback_);
    session_->setOnMessageCallback(onMessageCallback_);
  }
}
