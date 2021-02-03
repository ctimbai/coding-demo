#include "HttpServer.h"
#include "logging/Logging.h"
#include "reactor/EventLoop.h"

#include <functional>

using namespace libcpp;

HttpServer::HttpServer(EventLoop* loop, const InetAddress& addr,
                        const std::string& name)
 :  tcpServer_(loop, addr, name)
{
  using namespace std::placeholders;
  tcpServer_.setConnectionCallback(
      std::bind(&HttpServer::onConnection, this, _1));
  tcpServer_.setMessageCallback(
      std::bind(&HttpServer::onMessage, this, _1, _2, _3));
}

HttpServer::~HttpServer()
{

}

void HttpServer::start()
{
  tcpServer_.start();
}

void HttpServer::stop()
{
}

void HttpServer::onConnection(const TcpConnSptr& conn)
{
  if (conn->connected())
  {
    sessions_[conn] = std::make_shared<HttpSession>(conn, HTTP_REQUEST);
    sessions_[conn]->setOnHeadersCallback(onHeadersCallback_);
    sessions_[conn]->setOnMessageCallback(onMessageCallback_);
    LOG_TRACE << "connection established";
  }
  else
  {
    LOG_TRACE << "connection down";
    sessions_.erase(conn);
  }
}

void HttpServer::onMessage(const TcpConnSptr& conn,
                          Buffer* buf, TimeStamp receivedTime)
{
  (void)receivedTime;
  HttpSession *session = sessions_[conn].get();
  session->execute(buf->data(), buf->readableBytes());
  buf->retrieveAll();
}

