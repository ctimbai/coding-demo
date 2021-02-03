#ifndef LIBCPP_ASYNCLOGGINGDOUBLEBUFFERING_H_
#define LIBCPP_ASYNCLOGGINGDOUBLEBUFFERING_H_

#include "LogFile.h"
#include "io/Buffer.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <memory>
#include <vector>
#include <chrono>
#include <functional>

namespace libcpp
{

class AsyncLoggingDoubleBuffering
{
public:
  using LargeBuffer = FixedBuffer<kLargeBuffer>;
  using BufferPtr = std::unique_ptr<LargeBuffer>;
  using BufferVector = std::vector<BufferPtr>;

  AsyncLoggingDoubleBuffering(const std::string& basename, size_t rollSize, 
                              int flushInterval = 3)
  : basename_(basename),
    rollSize_(rollSize),
    flushInterval_(flushInterval),
    currentBuffer_(new LargeBuffer),
    nextBuffer_(new LargeBuffer)
  {
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);
  }
  
  ~AsyncLoggingDoubleBuffering() {
    if (running_) stop();
  }
  
  void start() {
    running_ = true;
    thread_ = std::thread(std::bind(&AsyncLoggingDoubleBuffering::threadFunc, this));
    // must wait until threadFunc run
    oneShot_.get_future().wait();
  }
  
  void stop() {
    running_ = false;
    cond_.notify_one();
    thread_.join();
  }
  
  // bind to logger's outputFunc
  void append(const char* logline, int len) 
  {
    std::lock_guard<std::mutex> lock(mutex_);
    if (currentBuffer_->avail() > len) {
      currentBuffer_->append(logline, len);
    }
    else {
      buffers_.emplace_back(currentBuffer_.release());
      if (nextBuffer_) {
        currentBuffer_ = std::move(nextBuffer_);
      }
      else {
        currentBuffer_.reset(new LargeBuffer);
      }
      currentBuffer_->append(logline, len);
      
      // notify backend there have filled buffer to save
      cond_.notify_one();
    }
  }
    
private:
  // backend, write to file
  void threadFunc() 
  {
    oneShot_.set_value();
    
    LogFile output(basename_, rollSize_, false);
    BufferPtr newBuffer1(new LargeBuffer);
    BufferPtr newBuffer2(new LargeBuffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    
    while (running_) {
      { // critical zone
        std::unique_lock<std::mutex> lock(mutex_);
        if (buffers_.empty()) {
          cond_.wait_for(lock, std::chrono::seconds(flushInterval_));
        }
        // change ownership, not copy
        buffers_.emplace_back(currentBuffer_.release());
        currentBuffer_ = std::move(newBuffer1);
        buffersToWrite.swap(buffers_);
        if (!nextBuffer_) nextBuffer_ = std::move(newBuffer2);
      }
        
      // write buffersToWrite to file
      for (decltype(buffersToWrite.size()) i = 0; i < buffersToWrite.size(); ++i) 
      {
        output.append((*buffersToWrite[i]).data(), (*buffersToWrite[i]).length());
      }
      buffersToWrite.clear();
        
      // refill newBuffer
      BufferPtr newBuffer1(new LargeBuffer);
      BufferPtr newBuffer2(new LargeBuffer);
      newBuffer1->bzero();
      newBuffer2->bzero();
      
      output.flush();
    }
    
    output.flush();
  }
    
  bool running_;
  std::string basename_;
  size_t rollSize_;
  const int flushInterval_;
  
  std::mutex mutex_;
  std::condition_variable cond_;
  std::thread thread_;
  std::promise<void> oneShot_;
  
  BufferPtr currentBuffer_;
  BufferPtr nextBuffer_;
  BufferVector buffers_;  // filled buffer, ready to save in file
};

} // libcpp


#endif