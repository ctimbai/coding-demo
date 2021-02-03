#include "LogFile.h"

#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

using namespace libcpp;

class LogFile::File
{
public:
  File(const std::string& filename)
      // 'e' : The underlying file descriptor will be closed
      //      if you use any of the exec… functions
    : fp_(::fopen(filename.data(), "ae")),
      writtenBytes_(0)
  {
    assert(fp_);
    // why set buffer?
    ::setbuffer(fp_, buffer_, sizeof(buffer_));
  }

  ~File() {
    ::fclose(fp_);
  }

  void append(const char* logline, int len) {
    size_t n = write(logline, len);
    size_t remain = len - n;

    while (remain > 0) {
      size_t x = write(logline + n, len);
      if (x == 0) {
        int err = ferror(fp_);
        if (err) {
          char buf[128];
          char* ret = strerror_r(err, buf, sizeof(buf));
          (void)ret;
          fprintf(stderr, "LogFile::File::append() failed %s\n", buf);
        }
        break;
      }
      n += x;
      remain -= x;
    }

    writtenBytes_ += len;
  }

  void flush() {
    ::fflush(fp_);
  }

  size_t writtenBytes() const { return writtenBytes_; }

private:
  size_t write(const char* logline, size_t len) {
    return ::fwrite_unlocked(logline, 1, len, fp_);
  }

  FILE* fp_;
  char buffer_[64*1024];
  size_t writtenBytes_;
};



LogFile::LogFile(const std::string& basename, size_t rollSize,
                    bool threadSafe, int flushInterval)
  : flushInterval_(flushInterval),
    basename_(basename),
    rollSize_(rollSize),
    startDay_(0),
    lastFlushTime_(0),
    mutex_(threadSafe? new std::mutex : NULL)
{
  assert(basename_.find('/') == std::string::npos);
  // create new file when construct
  rollFile();
}

LogFile::~LogFile()
{}

void LogFile::append(const char* logline, int len) {
  if (mutex_) {
    std::lock_guard<std::mutex> lock(*mutex_);
    append_unlocked(logline, len);
  }
  else {
    append_unlocked(logline, len);
  }
}

void LogFile::flush() {
  if (mutex_) {
    std::lock_guard<std::mutex> lock(*mutex_);
    file_->flush();
  }
  else {
    file_->flush();
  }
}

void LogFile::append_unlocked(const char* logline, int len) {
  file_->append(logline, len);

  // when current file is filled, write next file
  if (file_->writtenBytes() > rollSize_) {
    rollFile();
  }
  else {
    time_t now = ::time(NULL);
    time_t thisDay = now / kSecondsPerDay_;
    // roll per day
    if (thisDay != startDay_) {
      rollFile();
    }
    // flush per interval
    else if (now - lastFlushTime_ > flushInterval_) {
      lastFlushTime_ = now;
      flush();
    }
  }
}

void LogFile::rollFile() {
  time_t now = 0;
  std::string filename = getLogFileName(basename_, &now);

  startDay_ = now / kSecondsPerDay_;
  lastFlushTime_ = now;
  file_.reset(new File(filename));
}

std::string LogFile::getLogFileName(const std::string& basename, time_t* now) {
  std::string filename;
  filename.reserve(basename.size() + 128);
  filename = basename;

  char timebuf[32];
  struct tm tm;
  *now = time(NULL);
  gmtime_r(now, &tm); // FIXME: localtime_r ?
  strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
  filename += timebuf;

  char hostname[128];
  ::gethostname(hostname, sizeof(hostname));
  filename += hostname;

  char pidbuf[32];
  snprintf(pidbuf, sizeof pidbuf, ".%d", ::getpid()); // FIXME: ProcessInfo::pid();
  filename += pidbuf;
  filename += ".log";

  return filename;
}