/*
 * Description: test timer queue
 */
#include "reactor/EventLoop.h"


using namespace libcpp;

EventLoop* g_loop;
int cnt = 0;

void print(const char* msg)
{
  printf("msg %s %s\n", TimeStamp::now().toFormattedString().c_str(), msg);
  if (++cnt == 20) {
    g_loop->quit();
  }
}

int main()
{
  print("main loop");
  EventLoop loop;
  g_loop = &loop;
  
  loop.runAfter(1, std::bind(print, "once 1"));
  loop.runAfter(1.5, std::bind(print, "once 1.5"));
  loop.runAfter(2, std::bind(print, "once 2"));
  loop.runEvery(2, std::bind(print, "every 2"));
  loop.runEvery(3, std::bind(print, "every 3"));
  
  loop.loop();
}