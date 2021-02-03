#ifndef LIBCPP_NONCOPYABLE_H_
#define LIBCPP_NONCOPYABLE_H_

namespace libcpp
{
namespace utils
{

class noncopyable
{
public:
  noncopyable() = default;
  ~noncopyable() = default;
  
  noncopyable(const noncopyable&) = delete;
  noncopyable& operator= (const noncopyable&) = delete;
};

}
}

#endif