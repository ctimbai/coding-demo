#include "HimServer.h"
#include <iostream>

#include <functional>

using namespace libcpp;

HimServer::HimServer(EventLoop* loop, const InetAddress& addr)
  : HttpServer(loop, addr)
{
  using namespace std::placeholders;
  setOnMessageCallback(
      std::bind(&HimServer::onBody, this, _1, _2));
}

HimServer::~HimServer()
{
}

int HimServer::onBody(const HttpSessionSptr& sess, HttpMessage* msg)
{
  std::string name = msg->getValueByHeader("Username");
  if (userSessionMap_.count(name) == 0 && !name.empty()) {
    userSessionMap_[name] = sess;
    return 0;
  }

  std::string chatType = msg->getValueByHeader("ChatType");
  std::string peerName = msg->getValueByHeader("PeerName");
  if (peerName.empty()) return 0;

  if (chatType == "user") {
    if (userSessionMap_.count(peerName) == 0) return 0;
    userSessionMap_[peerName]->send(msg->getRequestAsString());
  }
  else if (chatType == "group") {
    for (auto& pss : userSessionMap_) {
      if (pss.first != name)
        pss.second->send(msg->getRequestAsString());
    }
  }
  return 0;
}
