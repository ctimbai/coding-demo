#ifndef LIBCPP_SPINLOCK_H_
#define LIBCPP_SPINLOCK_H_

#include <atomic>

namespace libcpp
{

/*
 * SpinLock:
 *  spinlock is a 'busy waiting' lock,
 *  while mutex is a 'sleep and wakeup' lock
 */
class SpinLock
{
public:
  SpinLock();
  ~SpinLock();
  
  void lock()
  {
    while (lck.test_and_set(std::memory_order_require)) {}
  }

  void unlock()
  {
    lck.clear(std::memory_order_release);
  }

private:
  std::atomic_flag lck = ATOMIC_FLAG_INIT;
};

}

#endif