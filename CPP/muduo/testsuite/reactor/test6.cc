/*
 * Description: test timer queue
 */
#include "reactor/EventLoop.h"
#include <iostream>
#include <thread>

using namespace libcpp;
using namespace std;

EventLoop* g_loop;

void run4()
{
  cout << "run4\n";
  g_loop->runInLoop(std::bind(&EventLoop::quit, g_loop));
}

void run1()
{
  cout << "thread id = " << this_thread::get_id() << " run1\n";
  g_loop->runInLoop(run4);
}

int main()
{
  cout << "thread id = " << this_thread::get_id() << " main\n";
  EventLoop loop;
  g_loop = &loop;
  
  thread t(std::bind(&EventLoop::runInLoop, &loop, run1));
  t.join();
  loop.loop();
}