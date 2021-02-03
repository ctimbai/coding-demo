/*
 * Description: call eventloop in another thread
 */
#include "reactor/EventLoop.h"
#include <iostream>

using namespace libcpp;

EventLoop *g_loop;

void threadFunc()
{
  std::cout << "threadFunc(): tid=" << std::this_thread::get_id() << '\n';
  g_loop->loop();
}

int main()
{
  std::cout << "main(): tid=" << std::this_thread::get_id() << '\n';
  EventLoop loop;
  g_loop = &loop;
  std::thread t(threadFunc);
  t.join();
}