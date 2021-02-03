#ifndef LIBCPP_HTTPMESSAGE_H_
#define LIBCPP_HTTPMESSAGE_H_

#include "http_parser.h"

#include <string>
#include <map>
#include <sstream>
#include <iostream>

namespace libcpp
{
/*
 * generate http messages
 */
class HttpMessage
{
public:
  HttpMessage() {}
  ~HttpMessage() {}

  void setMethod(http_method method) {
    method_.clear();
    method_.append(http_method_str(method));
  }

  void appendUrl(const std::string& url) { url_.append(url); }

  void setStatus(http_status status) {
    status_ = status;
    hint_ += http_status_str(status);
  }

  void setVersion(const std::string& version) { version_ = version; }

  void addHeader(const std::string& field, const std::string& value) {
    headers_[field] = value;
  }

  void appendBody(const std::string& body) { body_.append(body); }

  void setPath(const std::string& path) { path_ = path; }

  std::string getMethod() const { return method_; }
  std::string getUrl() const { return url_; }
  std::string getPath() const { return path_; }
  std::string getBody() const {return body_; }
  std::string getValueByHeader(const std::string& header) const
  {
    auto it = headers_.find(header);
    return it == headers_.end() ? "" : it->second;
  }

  void printHeaders() const
  {
    for (auto& pss : headers_) {
      std::cout << pss.first << " : " << pss.second << std::endl;
    }
  }

  void resetBody() { body_.clear(); }
  void delHeaderByField(const std::string& field)
  { headers_.erase(field); }

  std::string getResponseAsString() {
    std::stringstream ss;
    /* start line */
    hint_.append(http_status_str(status_));
    ss << version_ << " " << static_cast<int>(status_) << " " << hint_ << "\r\n";
    /* headers */
    for (auto& kv : headers_) {
      ss << kv.first << ": " << kv.second << "\r\n";
    }
    ss << "\r\n";
    /* body */
    ss << body_;
    return ss.str();
  }

  std::string getRequestAsString() {
    std::stringstream ss;
    /* start line */
    ss << method_ << " " << url_ << " " << version_ << "\r\n";
    /* headers */
    for (auto& kv : headers_) {
      ss << kv.first << ": " << kv.second << "\r\n";
    }
    ss << "\r\n";
    /* body */
    ss << body_;
    return ss.str();
  }

private:
  /* request */
  std::string method_;
  std::string url_;
  /* response */
  http_status status_ = HTTP_STATUS_OK;
  std::string hint_;
  /* shared */
  std::string version_ = "HTTP/1.1";
  std::map<std::string, std::string> headers_;
  std::string body_;

  /* utils */
  std::string path_;
};

}

#endif
