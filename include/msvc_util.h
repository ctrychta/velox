#ifndef VELOX_MSVC_UTIL_H_INCLUDED
#define VELOX_MSVC_UTIL_H_INCLUDED

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <process.h>

// This is a hack because MSVC doesn't handle decltype in template aliases correctly
#define VELOX_RVT(r) Unqual<decltype(*adl::adl_begin(std::declval<r>()))>

namespace velox {
template <class T>
void optimization_barrier(T &&t) {
  const char *p = reinterpret_cast<const char *>(&t);
  std::atomic_signal_fence(std::memory_order_seq_cst);

  if (volatile bool b = false) {
    _exit(*p);
  }
}

// On windows XP and up QueryPerformanceFrequency and QueryPerformanceCounter are documented to
// never fail so the return values are not checked
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

struct ProcessCPUClock {
  using rep = std::int64_t;
  using period = std::nano;
  using duration = std::chrono::duration<rep, period>;
  using time_point = std::chrono::time_point<ProcessCPUClock>;
  static const bool is_steady = true;

  static time_point now() {
    FILETIME creation, exit, kernel, user;
    if (GetProcessTimes(GetCurrentProcess(), &creation, &exit, &kernel, &user)) {
      auto to_nanos = [](FILETIME ft) -> ULONGLONG {
        ULARGE_INTEGER li;
        li.LowPart = ft.dwLowDateTime;
        li.HighPart = ft.dwHighDateTime;
        // ft has units of 100ns
        return li.QuadPart * 100;
      };

      return time_point(duration(static_cast<rep>(to_nanos(kernel) + to_nanos(user))));
    }

    assert(false && "GetProcessTimes failed");
    return time_point();
  }
};

namespace detail {
  template <class T>
  std::string unmangled_name() {
    return typeid(T).name();
  }
}
}

#endif // VELOX_MSVC_UTIL_H_INCLUDED
