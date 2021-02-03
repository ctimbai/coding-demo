#ifndef LIBCPP_COUNTDOWNLATCH_H_
#define LIBCPP_COUNTDOWNLATCH_H_

#include <mutex>
#include <condition_variable>

namespace libcpp
{

class CountDownLatch
{
public:
  CountDownLatch(int count)
    : mutex_(),
      cond_(),
      count_(count)
  {}
    
  void wait() {
    std::unique_lock<std::mutex> lock(mutex_);
    while (count_ > 0) {
      cond_.wait(lock);
    }
  }

  void countDown() {
    std::unique_lock<std::mutex> lock(mutex_);
    --count_;
    if (count_ == 0) cond_.notify_all();
  }

  int getCount() const {
    std::unique_lock<std::mutex> lock(mutex_);
    return count_;
  }
    
private:
  mutable std::mutex mutex_; // can be modified by const func like getCount()
  std::condition_variable cond_;
  int count_;
};

} // libcpp

#endif