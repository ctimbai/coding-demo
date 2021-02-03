#include "logging/Logging.h"
#include "reactor/EventLoop.h"
#include "HimClient.h"

#include <iostream>
#include <functional>

using namespace libcpp;
using namespace std::placeholders;

#ifdef DEBUG
static InetAddress serverAddr("127.0.0.1", 9981);
#else
static InetAddress serverAddr("106.52.193.215", 9981);
#endif

HimClient::HimClient(EventLoop* loop, std::string& uname, std::string& pwd)
  : HttpClient(loop, serverAddr), 
    userInputChannel_(loop, 0),
    username_(uname),
    password_(pwd),
    chatting_(false),
    userMsg_(new HttpMessage)
{
  userInputChannel_.setReadCallback(
      std::bind(&HimClient::handleStdinRead, this));
  userInputChannel_.enableReading();
  
  userMsg_->appendUrl("http://www.dummyUrl.com/tools.html");
  userMsg_->addHeader("Username", username_);
}

HimClient::~HimClient()
{
}

void HimClient::onConnection(const TcpConnSptr& conn)
{
  if (conn->connected()) {
    session_.reset(new HttpSession(conn, HTTP_BOTH));
    session_->setOnHeadersCallback(
        std::bind(&HimClient::onHeaders, this, _1, _2));
    session_->setOnMessageCallback(
        std::bind(&HimClient::onBody, this, _1, _2));
    sendLogin();
  }
}

void HimClient::sendLogin()
{
  HttpMessage login;
  login.setMethod(HTTP_POST);
  login.appendUrl("http://www.dummyUrl.com/tools.html");
  login.addHeader("Password", password_);
  login.addHeader("Username", username_);
  login.addHeader("Content-Length", std::to_string(0));
  session_->send(login.getRequestAsString());
}

int HimClient::onHeaders(const HttpSessionSptr& sess, HttpMessage* msg)
{
  (void)sess;
  (void)msg;
  return 0;
}

int HimClient::onBody(const HttpSessionSptr& sess, HttpMessage* msg)
{
  TimeStamp now(TimeStamp::now());
  std::cout << msg->getValueByHeader("Username") 
            << "(" << now.toFormattedString() 
            << "): " << msg->getBody() << std::endl;
  (void)sess;
  return 0;
}

void HimClient::handleStdinRead()
{
  int savedErrno = 0;
  ssize_t n = userInputBuffer_.readFd(userInputChannel_.fd(), &savedErrno);
  (void)n;
  std::stringstream ss(userInputBuffer_.retrieveAsString());
  std::string cmd;
  ss >> cmd;
 
  if (cmd == "help") {
    printGuidance();
  }
  else if (cmd == "show") {

  }
  else if (cmd == "chat") {
    chatting_ = true;
    std::string target;
    ss >> target;
    std::string id;
    ss >> id;
    userMsg_->addHeader("ChatType", target);
    userMsg_->addHeader("PeerName", id);
  }
  else if (cmd == "quit") {
    if (!chatting_) {
      std::cout << "[HIM] There is no active chat\n";
      return;
    }
    userMsg_->delHeaderByField("ChatType");
    userMsg_->delHeaderByField("PeerName");
    chatting_ = false;
  }
  else if (cmd == "exit") {
    std::cout << "[HIM] Good Bye\n";
    exit(0);
  }
  else if (chatting_) {
    userMsg_->resetBody();
    userMsg_->setMethod(HTTP_POST);
    userMsg_->delHeaderByField("Content-Length");
    userMsg_->addHeader("Content-Length", std::to_string(cmd.size()));
    userMsg_->appendBody(cmd);
    session_->send(userMsg_->getRequestAsString());
  }
  else {
    std::cout << "WRONG command!\n";
    printGuidance();
  }
}

void HimClient::printGuidance()
{
  printf("You can execute cmds as follows:\n"
          "show [user/group]: Display online/offline"
          " users or existing group\n"
          "chat [user/group] <username/groupname>: Send/receive msgs to/from" 
          "specific user or group\n"
          "quit: Quit current chat operation. If non-chat operation, equivalent"
          " to exit\n"
          "exit: Exit the client\n");
}
