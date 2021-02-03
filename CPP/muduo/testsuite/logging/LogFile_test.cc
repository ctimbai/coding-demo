#include "../LogFile.h"
#include "../Logging.h"

#include <memory>
#include <unistd.h>

std::unique_ptr<libcpp::LogFile> g_logFile;

void outputFunc(const char* msg, int len)
{
  g_logFile->append(msg, len);
}

void flushFunc()
{
  g_logFile->flush();
}

int main(int argc, char* argv[])
{
  char name[256];
  strncpy(name, argv[0], 256);
  g_logFile.reset(new libcpp::LogFile(::basename(name), 256*1024));
  libcpp::Logger::setOutputFunc(outputFunc);
  libcpp::Logger::setFlushFunc(flushFunc);

  std::string line = "1234567890 abcdefghijklmnopqrstuvwxyz ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  for (int i = 0; i < 10000; ++i)
  {
    LOG_INFO << line;

    usleep(1000);
  }
}