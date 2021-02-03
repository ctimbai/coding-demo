#ifndef LIBCPP_LOGGING_H_
#define LIBCPP_LOGGING_H_

#include "datetime/TimeStamp.h"
#include <sstream>
#include <functional>


/*
 * Log Filename Format:
 *     process name.datetime.hostname.process id.log
 * 
 * Log Format:
 *      date time tid loglevel message - sourcefile: line
 */
namespace libcpp
{

// C++ stream style
class Logger
{
public:
  enum LogLevel
  {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    NUM_LOG_LEVELS,
  };
  
  Logger(const char* file, const char* func, int line, LogLevel level);
  Logger(const char* file, const char* func, int line, bool toAbort);
  ~Logger();
  
  static LogLevel logLevel();
  static void setLogLevel(LogLevel level);
  
  //std::ostringstream& stream() { return impl_.stream_; }
  std::ostringstream& stream();
  
  using OutputFunc = std::function<void(const char*, int)>;
  using FlushFunc = std::function<void()>;
  static void setOutputFunc(OutputFunc);
  static void setFlushFunc(FlushFunc);
    
private:
  class Impl
  {
  public:
    using LogLevel = Logger::LogLevel;
    Impl(LogLevel level, int savedErrno, const char* file, const char* func, int line);
    void recordFormatTime();
    
    TimeStamp timestamp_;
    //std::ostringstream stream_;
    LogLevel level_;
    int line_;
    const char* func_;
    const char* fullname_;
    const char* basename_;
  };
  
  Impl impl_;
};


const char* strerror_tl(int savedErrno);
/*
 *  __FILE__: the full path to the source file
 *  __LINE__: the line number of the code
 */
#define LOG_TRACE if (libcpp::Logger::logLevel() <= libcpp::Logger::TRACE) \
    libcpp::Logger(__FILE__, __func__, __LINE__, libcpp::Logger::TRACE).stream()
#define LOG_DEBUG if (libcpp::Logger::logLevel() <= libcpp::Logger::DEBUG) \
    libcpp::Logger(__FILE__, __func__, __LINE__, libcpp::Logger::DEBUG).stream()
#define LOG_INFO if (libcpp::Logger::logLevel() <= libcpp::Logger::INFO) \
    libcpp::Logger(__FILE__, __func__, __LINE__, libcpp::Logger::INFO).stream()
#define LOG_WARN \
    libcpp::Logger(__FILE__, __func__, __LINE__, libcpp::Logger::WARN).stream()

#define LOG_ERROR \
    libcpp::Logger(__FILE__, __func__, __LINE__, libcpp::Logger::ERROR).stream()
#define LOG_FATAL \
    libcpp::Logger(__FILE__, __func__, __LINE__, libcpp::Logger::FATAL).stream()
#define LOG_SYSERR \
    libcpp::Logger(__FILE__, __func__, __LINE__, false).stream()
#define LOG_SYSFATAL \
    libcpp::Logger(__FILE__, __func__, __LINE__, true).stream()



} // libcpp

#endif