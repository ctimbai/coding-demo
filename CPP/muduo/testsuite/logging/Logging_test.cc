#include "logging/Logging.h"
#include "logging/LogFile.h"

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <functional>
#include <sys/time.h>
#include <time.h>

using std::string;

long g_total;
FILE* g_file;
std::unique_ptr<libcpp::LogFile> g_logFile;

void dummyOutput(const char* msg, int len)
{
  g_total += len;
  if (g_file)
  {
    fwrite(msg, 1, len, g_file);
  }
  else if (g_logFile)
  {
    g_logFile->append(msg, len);
  }
}

void bench()
{
  //libcpp::Logger::OutputFunc output = dummyOutput;
  libcpp::Logger::setOutputFunc(dummyOutput);
  g_total = 0;

  const int batch = 1000*1000;
  const bool kLongLog = false;
  string empty = " ";
  string longStr(3000, 'X');
  longStr += " ";
  
  // start
  struct timeval start;
  gettimeofday(&start, NULL);
  for (int i = 0; i < batch; ++i)
  {
    LOG_INFO << "Hello 0123456789" << " abcdefghijklmnopqrstuvwxyz "
             << (kLongLog ? longStr : empty)
             << i;
  }
  struct timeval end;
  gettimeofday(&end, NULL);
  // end
  time_t usenconds = (end.tv_sec * 1000 * 1000 + end.tv_usec) - (start.tv_sec * 1000 * 1000 + start.tv_usec);
  double seconds = (double)usenconds/(1000*1000);
  printf("%f seconds, %ld bytes, %.2f msg/s, %.2f MiB/s\n",
         seconds, g_total, batch / seconds, g_total / seconds / 1024 / 1024);
}

int main()
{
  getppid(); // for ltrace and strace
  LOG_DEBUG << "debug";
  LOG_INFO << "Hello";
  LOG_WARN << "World";
  LOG_ERROR << "Error";
  LOG_INFO << sizeof(libcpp::Logger);

  bench();

  char buffer[64*1024];

  g_file = fopen("/dev/null", "w");
  setbuffer(g_file, buffer, sizeof buffer);
  bench();
  fclose(g_file);

  g_file = fopen("/tmp/log", "w");
  setbuffer(g_file, buffer, sizeof buffer);
  bench();
  fclose(g_file);

  g_file = NULL;
  g_logFile.reset(new libcpp::LogFile("test_log", 500*1000*1000));
  bench();

  g_logFile.reset(new libcpp::LogFile("test_log_mt", 500*1000*1000, true));
  bench();
  g_logFile.reset();
}
