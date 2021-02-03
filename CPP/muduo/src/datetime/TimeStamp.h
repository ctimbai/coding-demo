#ifndef LIBCPP_TimeStamp_H_
#define LIBCPP_TimeStamp_H_

#include <stdint.h>
#include <string>

namespace libcpp
{

class TimeStamp
{
public:
    TimeStamp();
    explicit TimeStamp(int64_t microSecondsSinceEpoch);
    
    std::string toString() const;
    std::string toFormattedString() const;
    
    int64_t microSecondsSinceEpoch() const { 
        return microSecondsSinceEpoch_; 
    }
    
    bool valid() const { return microSecondsSinceEpoch_ > 0; }
    
    static TimeStamp now();
    static TimeStamp invalid() { return TimeStamp(); }
    static const int kMicroSecondsPerSecond = 1000 * 1000;
    
private:
    int64_t microSecondsSinceEpoch_;
};

inline bool operator< (TimeStamp lhs, TimeStamp rhs)
{
  return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator== (TimeStamp lhs, TimeStamp rhs)
{
  return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

inline double timeInterval(TimeStamp high, TimeStamp low)
{
  int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
  return static_cast<double>(diff) / TimeStamp::kMicroSecondsPerSecond;
}

inline TimeStamp addTime(TimeStamp time, double seconds)
{
  int64_t delta = static_cast<int64_t>(seconds * TimeStamp::kMicroSecondsPerSecond);
  return TimeStamp(time.microSecondsSinceEpoch() + delta);
}

} // libcpp

#endif