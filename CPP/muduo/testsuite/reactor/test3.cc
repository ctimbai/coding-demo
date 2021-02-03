/*
 * Description: create two eventloop in one thread
 */
#include "reactor/EventLoop.h"
#include <iostream>

using namespace libcpp;


int main()
{
  std::cout << "main(): tid=" << std::this_thread::get_id() << '\n';
  EventLoop loop1;
  EventLoop loop2;

}