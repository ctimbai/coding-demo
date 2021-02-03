#include "HttpSession.h"
#include "logging/Logging.h"


namespace libcpp
{
http_parser_settings g_parser_settings;

class HttpParserSettings
{
public:
  HttpParserSettings()
  {
    g_parser_settings.on_message_begin = &onMessageBegin;
    g_parser_settings.on_url = &onUrl;
    g_parser_settings.on_status = &onStatus;
    g_parser_settings.on_header_field = &onHeaderField;
    g_parser_settings.on_header_value = &onHeaderValue;
    g_parser_settings.on_headers_complete = &onHeadersComplete;
    g_parser_settings.on_body = &onBody;
    g_parser_settings.on_message_complete = &onMessageComplete;
    g_parser_settings.on_chunk_header = &onChunkHeader;
    g_parser_settings.on_chunk_complete = &onChunkComplete;
  }

  /*
   * process callbacks
   */
  static int onMessageBegin(http_parser *parser);
  static int onUrl(http_parser *parser, const char *at, size_t length);
  static int onStatus(http_parser *parser, const char *at, size_t length);
  static int onHeaderField(http_parser *parser, const char *at, size_t length);
  static int onHeaderValue(http_parser *parser, const char *at, size_t length);
  static int onHeadersComplete(http_parser *parser);
  static int onBody(http_parser *parser, const char *at, size_t length);
  static int onMessageComplete(http_parser *parser);
  static int onChunkHeader(http_parser *parser);
  static int onChunkComplete(http_parser *parser);
};

int HttpParserSettings::onMessageBegin(http_parser *parser)
{
  HttpSession *p = static_cast<HttpSession*>(parser->data);
  return p->onMessageBegin();
}

int HttpParserSettings::onUrl(http_parser *parser, const char *at, size_t length)
{
  HttpSession *p = static_cast<HttpSession*>(parser->data);
  return p->onUrl(at, length);
}

int HttpParserSettings::onStatus(http_parser *parser, const char *at, size_t length)
{
  HttpSession *p = static_cast<HttpSession*>(parser->data);
  return p->onStatus(at, length);
}

int HttpParserSettings::onHeaderField(http_parser *parser, const char *at, size_t length)
{
  HttpSession *p = static_cast<HttpSession*>(parser->data);
  return p->onHeaderField(at, length);
}

int HttpParserSettings::onHeaderValue(http_parser *parser, const char *at, size_t length)
{
  HttpSession *p = static_cast<HttpSession*>(parser->data);
  return p->onHeaderValue(at, length);
}

int HttpParserSettings::onHeadersComplete(http_parser *parser)
{
  HttpSession *p = static_cast<HttpSession*>(parser->data);
  return p->onHeadersComplete();
}

int HttpParserSettings::onBody(http_parser *parser, const char *at, size_t length)
{
  HttpSession *p = static_cast<HttpSession*>(parser->data);
  return p->onBody(at, length);
}

int HttpParserSettings::onMessageComplete(http_parser *parser)
{
  HttpSession *p = static_cast<HttpSession*>(parser->data);
  return p->onMessageComplete();
}

int HttpParserSettings::onChunkHeader(http_parser *parser)
{
  HttpSession *p = static_cast<HttpSession*>(parser->data);
  return p->onChunkHeader();
}

int HttpParserSettings::onChunkComplete(http_parser *parser)
{
  HttpSession *p = static_cast<HttpSession*>(parser->data);
  return p->onChunkComplete();
}

HttpParserSettings initObj;

} /* namespace libcpp */



using namespace libcpp;

HttpSession::HttpSession(const TcpConnSptr& conn, http_parser_type parserType)
 :  tcpConn_(conn),
    parserType_(parserType)
{
  http_parser_init(&parser_, parserType);
  /* use http_parser's data field to store 'this' pointer for callbacks */
  parser_.data = this;
}

HttpSession::~HttpSession()
{
}

size_t HttpSession::execute(const char *buf , size_t length)
{
  /*
   * it will call the callbacks
   * Returns number of parsed bytes. Sets parser->http_errno` on error
   */
  size_t n = http_parser_execute(&parser_, &g_parser_settings, buf, length);

  if (parser_.upgrade) {
    /* handle new protocol */
  }
  else if (n != length) {
    /* handle error. Usually close the connection */
    LOG_ERROR << http_errno_name(static_cast<http_errno>(parser_.http_errno)) << ": "
              << http_errno_description(static_cast<http_errno>(parser_.http_errno));
  }
  return 0;
}


int HttpSession::onMessageBegin()
{
  LOG_TRACE << "Message Begin";
  /* initialize */
  curField_.clear();
  curValue_.clear();
  lastIsValue_ = true;
  /* create message buffer */
  httpMessage_.reset(new HttpMessage);
  return 0;
}

int HttpSession::onUrl(const char* at, size_t length)
{
  LOG_TRACE << std::string(at, length);
  httpMessage_->appendUrl(std::string(at, length));
  return 0;
}

int HttpSession::onStatus(const char* at, size_t length)
{
  LOG_TRACE << std::string(at, length);
  httpMessage_->setStatus(static_cast<http_status>(parser_.status_code));
  return 0;
}

int HttpSession::onHeaderField(const char* at, size_t length)
{
  LOG_TRACE << std::string(at, length);

  if (lastIsValue_) {
    if (!curField_.empty()) {
      httpMessage_->addHeader(curField_, curValue_);
    }
    curField_.clear();
    curValue_.clear();
  }

  curField_.append(at, length);
  lastIsValue_ = false;
  return 0;
}

int HttpSession::onHeaderValue(const char* at, size_t length)
{
  LOG_TRACE << std::string(at, length);
  curValue_.append(at, length);
  lastIsValue_ = true;
  // httpMessage_->addHeader(curField_, curValue_);
  return 0;
}

int HttpSession::onHeadersComplete()
{
  LOG_TRACE << "Header Complete";
  httpMessage_->setMethod(static_cast<http_method>(parser_.method));
  if (lastIsValue_)  {
    if (!curField_.empty()) {
      httpMessage_->addHeader(curField_, curValue_);
    }
    curField_.clear();
    curValue_.clear();
  }
  /*
   * URL format:
   *  protocol://hostname[:port]/path/[;parameters][?query]#fragment
   *
   * Meaning:
   *  protocol: http / https
   *  hostname: IP / domain name
   *  port: default 80
   *  path: location of source
   *  parameter: params passed to server
   *  query: what you want to query
   *  fragment: which part of web to goto
   */
  struct http_parser_url url;
  http_parser_url_init(&url);
  std::string urlStr = httpMessage_->getUrl();
  int ret = http_parser_parse_url(urlStr.data(), urlStr.size(), 0, &url);
  if (ret != 0) {
    LOG_ERROR << "parse url error";
    return ret;
  }

  if (url.field_set & (1 << UF_PATH)) {
    httpMessage_->setPath(
        urlStr.substr(url.field_data[UF_PATH].off, url.field_data[UF_PATH].len));
  }
  if (onHeadersCallback_)
    return onHeadersCallback_(shared_from_this(), httpMessage_.get());
  return 0;
}

int HttpSession::onBody(const char* at, size_t length)
{
  LOG_TRACE << "on body";
  httpMessage_->appendBody(std::string(at, length));
  return 0;
}

int HttpSession::onMessageComplete()
{
  LOG_TRACE << "Message Complete";
  if (onMessageCallback_)
    return onMessageCallback_(shared_from_this(), httpMessage_.get());
  return 0;
}

