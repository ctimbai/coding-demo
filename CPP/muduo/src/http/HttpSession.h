#ifndef LIBCPP_HTTPSESISON_H_
#define LIBCPP_HTTPSESISON_H_

#include "http_parser.h"
#include "utils/noncopyable.h"
#include "net/TcpConnection.h"
#include "HttpMessage.h"

#include <memory>
#include <map>

namespace libcpp
{
class HttpSession;
using HttpSessionSptr = std::shared_ptr<HttpSession>;
using HttpCallback = std::function<int(const HttpSessionSptr& sess, HttpMessage* msg)>;
/*
 * HttpSession:
 *  a HttpSession is over a TcpConnection.
 */
class HttpSession : public utils::noncopyable,
                    public std::enable_shared_from_this<HttpSession>
{
  friend class HttpParserSettings;
public:
  /*
   * @param parserType
   * - HTTP_REQUEST means parse http request messages
   * - HTTP_RESPONSE means parse http response messages
   * - HTTP_BOTH means parse both messages
   */
  HttpSession(const TcpConnSptr& tcpConn, http_parser_type parserType);
  ~HttpSession();

  size_t execute(const char *buf, size_t length);
  void send(const std::string& msg)
  { tcpConn_->send(msg); }
  void shutdown()
  { tcpConn_->shutdown(); }

  void setOnHeadersCallback(const HttpCallback& cb)
  { onHeadersCallback_ = cb; }
  void setOnMessageCallback(const HttpCallback& cb)
  { onMessageCallback_ = cb; }

  int shouldKeepAlive()
  { return http_should_keep_alive(&parser_) == 0; }

protected:
  int onMessageBegin();
  int onUrl(const char*, size_t);
  int onStatus(const char*, size_t);
  int onHeaderField(const char*, size_t);
  int onHeaderValue(const char*, size_t);
  int onHeadersComplete();
  int onBody(const char*, size_t);
  int onMessageComplete();

  int onChunkHeader() { return 0; }
  int onChunkComplete() { return 0; }

private:
  TcpConnSptr tcpConn_;
  http_parser_type parserType_;
  http_parser parser_;

  std::unique_ptr<HttpMessage> httpMessage_;

  std::string curField_;
  std::string curValue_;
  bool lastIsValue_;

  HttpCallback onHeadersCallback_;
  HttpCallback onMessageCallback_;
};

}

#endif
