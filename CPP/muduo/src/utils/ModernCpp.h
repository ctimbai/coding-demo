#ifndef LIBCPP_MODERNCPP_H_
#define LIBCPP_MODERNCPP_H_

#include <memory>

namespace libcpp
{

/* make_unique is a C++14 feature, here is C++11 implementation */
template<typename T, typename... Ts>
std::unique_ptr<T> make_unique(Ts... params)
{
  return std::unique_ptr<T>(new T(std::forward<Ts>(params)...));
}

}

#endif