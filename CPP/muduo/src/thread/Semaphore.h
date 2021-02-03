#ifndef LIBCPP_SEMAPHORE_H_
#define LIBCPP_SEMAPHORE_H_

#include <mutex>
#include <condition_variable>

namespace libcpp
{

/*
 * Semaphore:
 *  1. initialized with number of resources
 *  2. when there are resources available, it's non-blocking;
 *      otherwise, it wait for wakeup signal
 *  @param count - number of resources
 */
class Semaphore
{
public:
  Semaphore(int count = 0)
   : count_(count)
  {}

  ~Semaphore();

  void wait()
  {
    std::unique_lock<std::mutex> lock(mutex_);
    while (!count_) {
      cond_.wait(mutex_);
    }
    count_--;
  }

  void notify()
  {
    std::lock_guard<std::mutex> lock(mutex_);
    count_++;
    cond_.notify_one();
  }

  bool try_wait()
  {
    std::lock_guard<std::mutex> lock(mutex_);
    if (count_) {
      count_--;
      return true;
    }
    else return false;
  }

private:
  int count_;
  std::mutex mutex_;
  std::condition_variable cond_;
};

}

#endif