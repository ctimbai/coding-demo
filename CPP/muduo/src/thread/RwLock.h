#ifndef LIBCPP_RWLOCK_H_
#define LIBCPP_RWLOCK_H_

#include <mutex>
#include <condition_variable>

namespace libcpp
{

/*
 * C++ 17 has <shared_mutex> can perform the same:
 *  {
 *    using rwlock = std::shared_mutex;
 *    using ReadLock = std::shared_lock<rwlock>;
 *    using WriteLock = std::unique_lock<rwlock>; 
 *  }
 *
 * Read-Write Lock:
 *  1. when there are readers, writer should be blocked
 *  2. when there is a writer, readers should be blocked
 *  3. when there is writing request, the coming readers should be blocked
 *  4. Only one writer at a time
 *
 * Simply a std::mutex is often enough !!!
 * Here some 6 questions, if you can answer 'no' to any of them, rwlock is a worse choice:
 *  ref: https://stackoverflow.com/questions/16774469/a-rw-lock-for-c11-threads
 *  1. Is my object const?
 *  2. Are my critical sections really long?
 *  3. Should my critical sections be that long?
 *  4. Is lock contention really my performance problem?
 *  5. Could I reduce my lock contention by switching to a finer-grain locking scheme?
 *  6. Is the ratio of readers to writers significantly greater than 1:1?
 */
class RwLock
{
public:
  RwLock();
  ~RwLock();

  void read_lock()
  {
    std::unique_lock<std::mutex> lock(mutex_);
    if (writers_ || write_waiters_) {
      read_waiters_++;
      rcond_.wait(lock, [this]() { return (!writers_ && !write_waiters_); });
      read_waiters_--;
    }
    readers_++;
  }

  void read_unlock()
  {
    std::lock_guard<std::mutex> lock(mutex_);
    readers_--;
    if (write_waiters_) {
      wcond_.notify_one();
    }
  }

  void write_lock()
  {
    std::unique_lock<std::mutex> lock(mutex_);
    if (readers_ || writers_) {
      write_waiters_++;
      /* a new writer can get lock only when there are no readers and writer */
      wcond_.wait(lock, [this]() { return !readers_ && !writers_; });
      write_waiters_--;
    }
    writers_++;
  }

  void write_unlock()
  {
    std::lock_guard<std::mutex> lock(mutex_);
    writers_--;
    /* if there are any writers wait, wakeup them first */
    if (write_waiters_) {
      wcond_.notify_one();
    }
    else {
      rcond_.notify_all();
    }
  }

private:
  std::mutex mutex_;
  std::condition_variable rcond_, wcond_;
  unsigned int readers_, read_waiters_;
  unsigned int writers_, write_waiters_;
};

}

#endif