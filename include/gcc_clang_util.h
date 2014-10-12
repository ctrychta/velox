#ifndef VELOX_GCC_CLANG_UTIL_H_INCLUDED
#define VELOX_GCC_CLANG_UTIL_H_INCLUDED

#include <string>
#include <cxxabi.h>

#define VELOX_RVT(r) RangeValueType<r>

namespace velox {
using DefaultClock = std::chrono::high_resolution_clock;

template <class T>
void optimization_barrier(T &&t) {
  __asm__ __volatile__("" : "+r"(t));
}

namespace detail {
  template <class T>
  std::string unmangled_name() {
    std::unique_ptr<char, void (*)(void *)> n(
        abi::__cxa_demangle(typeid(T).name(), nullptr, nullptr, nullptr), std::free);
    return n != nullptr ? n.get() : typeid(T).name();
  }
}
}

#endif // VELOX_GCC_CLANG_UTIL_H_INCLUDED
