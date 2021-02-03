/*
 * Description: normal funtion test
 */

#include "reactor/EventLoop.h"
#include <iostream>

void threadFunc()
{
  std::cout << "threadFunc(): tid=" << std::this_thread::get_id() << '\n';
  libcpp::EventLoop loop;
  loop.loop();
}

int main()
{
  std::cout << "main(): tid=" << std::this_thread::get_id() << '\n';
  libcpp::EventLoop loop;
  
  std::thread t(threadFunc);
  t.join();
  
  loop.loop();
}