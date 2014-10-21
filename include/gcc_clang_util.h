#ifndef VELOX_GCC_CLANG_UTIL_H_INCLUDED
#define VELOX_GCC_CLANG_UTIL_H_INCLUDED

#include <cxxabi.h>
#include <time.h>

#define VELOX_RVT(r) RangeValueType<r>

namespace velox {
using DefaultClock = std::chrono::high_resolution_clock;

template <class T>
void optimization_barrier(T &&t) {
  __asm__ __volatile__("" : "+r"(t));
}

struct ProcessCPUClock {
  using rep = std::int64_t;
  using period = std::nano;
  using duration = std::chrono::duration<rep, period>;
  using time_point = std::chrono::time_point<ProcessCPUClock>;
  static const bool is_steady = true;

  static time_point now() {
    timespec ts;

    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) == 0) {
      rep d = ts.tv_sec;
      d *= 1000000000;
      d += ts.tv_nsec;
      return time_point(duration(d));
    }

    assert(false && "clock_gettime failed");
    return time_point();
  }
};

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
