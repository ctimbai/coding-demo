#include "HimClient.h"
#include "reactor/EventLoop.h"
#include "logging/Logging.h"

#include <string>
#include <iostream>

using namespace libcpp;

int main(int argc, char* argv[]) {
  Logger::setLogLevel(Logger::ERROR);

  if (argc > 1) {
    printf("Him tackes none args.%s etc are invalid.", argv[0]);
  }
  
  EventLoop loop;
  
  std::string uname, pwd;
  printf("Please Input Your Username and Password:\n");
  printf("Username: ");
  std::cin >> uname;
  printf("Password: ");
  std::cin >> pwd;
  
  HimClient client(&loop, uname, pwd); 
  client.start();
  loop.loop();
  return 0;
}
