#include "datetime/TimerQueue.h"
#include "datetime/TimerWheel.h"
#include "reactor/EventLoop.h"
#include "logging/Logging.h"
#include <chrono>
#include <random>
#include <iostream>

using namespace libcpp;

const int k = 1000 * 1000;
const double unit = 0.01;
const double base = 3600;
std::vector<int> distribution;
std::unique_ptr<TimerQueue> tq;
std::unique_ptr<TimerWheel> tw;
std::vector<TimerId> timers(k, TimerId());

void func() {}

void benchTimerQueue()
{
  TimeStamp current(TimeStamp::now());

  auto begin = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < k; ++i) {
    timers[i] = tq->addTimer(func,
            addTime(current, base + distribution[i] * unit), 0);
  }

  for (int i = 0; i < k; ++i) {
    tq->cancel(timers[i]);
  }
  auto end = std::chrono::high_resolution_clock::now();

  std::cout << "TimerQueue insert/delete " << k << " times: "
        << std::chrono::duration_cast<std::chrono::nanoseconds>
               (end - begin).count() << "ns" << std::endl;
}

void benchTimerWheel()
{
  TimeStamp current(TimeStamp::now());

  auto begin = std::chrono::high_resolution_clock::now();
  for (int i = 0; i < k; ++i) {
    timers[i] = tw->addTimer(func,
            addTime(current, base + distribution[i] * unit), 0);
  }

  for (int i = 0; i < k; ++i) {
    tw->cancel(timers[i]);
  }
  auto end = std::chrono::high_resolution_clock::now();

  std::cout << "TimerWheel insert/delete " << k << " times: "
        << std::chrono::duration_cast<std::chrono::nanoseconds>
               (end - begin).count() << "ns" << std::endl;
}

int main()
{
  EventLoop loop(0);
  /* generate timer task distribution */
  std::default_random_engine generator;
  std::uniform_int_distribution<int> dist(0, k);
  for (int i = 0; i < k; ++i) {
    distribution.push_back(dist(generator));
  }

  /* generate TimerQueue and TimerWheel */
  tq.reset(new TimerQueue(&loop));
  tw.reset(new TimerWheel(&loop, unit, 100));

  benchTimerQueue();
  benchTimerWheel();
}