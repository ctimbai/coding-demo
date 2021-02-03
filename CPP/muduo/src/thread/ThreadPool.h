#ifndef LIBCPP_THREADPOOL_H_
#define LIBCPP_THREADPOOL_H_

#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <functional>
#include <future>

namespace libcpp
{

class ThreadPool 
{
public:
  ThreadPool(size_t threadNum);
  ~ThreadPool();
  
  void stop();
  
  template <class F, class ...Args>
  auto enqueue(F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;
  
private:
  bool stop_;
  std::mutex queueMutex_;
  std::condition_variable cond_;
  
  std::vector<std::thread> threads_;
  std::queue<std::function<void()>> tasks_;
};


inline ThreadPool::ThreadPool(size_t threadNum)
  : stop_(false)
{
  threads_.reserve(threadNum);
  for (size_t i = 0; i < threadNum; ++i) {
    threads_.emplace_back(
        [this]
        {
          std::function<void()> task;
          for (;;) {
            {
              // Note: don't use lock_guard, as condition need lock()/unlock() operation
              //        while lock_guard doesn't provide them
              std::unique_lock<std::mutex> lock(this->queueMutex_);
              this->cond_.wait(lock, [this]{ return this->stop_ || !tasks_.empty() });
              
              if (stop_ && tasks_.empty()) return;
              
              task = std::move(this->tasks_.front());
              this->tasks_.pop();
            }
            
            task();
          }
        }
    );
  }
}


inline ThreadPool::~ThreadPool()
{
  if (!stop_) stop();
}

inline ThreadPool::stop()
{
  {
    std::unique_lock<std::mutex> lock(queueMutex_);
    stop_ = true;
  }
  
  cond_.notify_all();
  for (std::thread& thread : threads_) {
    thread.join();
  }
}

template <class F, class... Args>
auto enqueue(F&& f, Args&&... args)
      -> std::future<typename std::result_of<F(Args...)>::type>
{
  using return_type = typename std::result_of<F(Args...)>::type;
  
  auto task = std::make_shared<std::packaged_task<return_type()>>(
                std::bind(std::forward<F>(f), std::forward<Args>(args)...)
              );
  
  std::future<return_type> res = task->get_future();
  
  {
    std::unique_lock<std::mutex> lock(queueMutex_);
    if (stop) {
      std::runtime_error("[ThreadPool] The ThreadPool has stopped");
    }
    
    // task may have return value res, use lambda wrapper to make task 'void()'
    tasks_.emplace( [task](){ *(task)(); } );
  }
  
  return res;
}


}

#endif