#ifndef LIBCPP_LOGFILE_H_
#define LIBCPP_LOGFILE_H_

#include <string>
#include <memory>
#include <mutex>

namespace libcpp
{

class LogFile
{
public:
    LogFile(const std::string& basename, size_t rollSize, 
            bool threadSafe = false, int flushInterval = 3);
    ~LogFile();
    
    void append(const char* logling, int len);
    void flush();
    
private:
    void append_unlocked(const char* logline, int len);
    static std::string getLogFileName(const std::string& basename, time_t* now);
    void rollFile();
    
    const int flushInterval_;
    const std::string basename_;
    
    size_t rollSize_;
    time_t startDay_;
    time_t lastFlushTime_;
    std::unique_ptr<std::mutex> mutex_;
    class File;
    std::unique_ptr<File> file_;
    
    const static int kSecondsPerDay_ = 60*60*24;
};

} // libcpp

#endif