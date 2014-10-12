#ifndef VELOX_MSVC_UTIL_H_INCLUDED
#define VELOX_MSVC_UTIL_H_INCLUDED

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// This is a hack because MSVC doesn't handle decltype in template aliases
// correctly
#define VELOX_RVT(r) Unqual<decltype(*adl::adl_begin(std::declval<r>()))>

namespace velox {
// On windows XP and up these functions are documented to never fail so the
// return values are not checked
namespace {
  const double QPC_NANOS_PER_TICK = []() -> double {
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    return 1000000000.0 / frequency.QuadPart;
  }();
}

struct WindowsHighResolutionClock {
  using rep = std::int64_t;
  using period = std::nano;
  using duration = std::chrono::duration<rep, period>;
  using time_point = std::chrono::time_point<WindowsHighResolutionClock>;
  static const bool is_steady = true;

  static time_point now() {
    LARGE_INTEGER count;
    QueryPerformanceCounter(&count);
    return time_point(duration(static_cast<rep>(count.QuadPart * QPC_NANOS_PER_TICK)));
  }
};

using DefaultClock = WindowsHighResolutionClock;

namespace detail {
  template <class T>
  std::string unmangled_name() {
    return typeid(T).name();
  }
}
}

#endif // VELOX_MSVC_UTIL_H_INCLUDED
