#ifndef VELOX_UTIL_H_INCLUDED
#define VELOX_UTIL_H_INCLUDED

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <vector>
#include <cassert>
#include <chrono>
#include <string>

#ifdef _MSC_VER
#include "msvc_util.h"
#else
#include "gcc_clang_util.h"
#endif

namespace velox {

template <class T>
using Invoke = typename T::type;

template <class Clock>
using TimePoint = typename Clock::time_point;

template <class C>
using Duration = typename C::duration;

template <class T>
using SizeType = typename T::size_type;

template <class T>
using RemoveReference = Invoke<std::remove_reference<T>>;

template <class T>
using RemoveCv = Invoke<std::remove_cv<T>>;

template <class T>
using Unqual = RemoveCv<RemoveReference<T>>;

template <class... Ts>
void unused(Ts &&...) {
}

namespace adl {
  using std::begin;
  template <class Range, class Ret = decltype(begin(std::declval<Range>()))>
  Ret adl_begin(Range &&r) {
    return begin(std::forward<Range>(r));
  }

  using std::end;
  template <class Range, class Ret = decltype(end(std::declval<Range>()))>
  Ret adl_end(Range &&r) {
    return end(std::forward<Range>(r));
  }
}

template <class Range>
using RangeValueType = Unqual<decltype(*adl::adl_begin(std::declval<Range>()))>;

struct CallTester {
  template <class F, class... Args>
  static decltype(std::declval<F>()(std::declval<Args>()...), std::true_type()) test(int);

  template <class F, class... Args>
  static std::false_type test(...);
};

template <class F, class... Args>
struct IsCallable : decltype(CallTester::test<F, Args...>(0)) {};

using Ns = std::chrono::nanoseconds;
using FpNs = std::chrono::duration<double, std::nano>;
using Ms = std::chrono::milliseconds;
using Times = std::vector<FpNs>;

template <class T>
std::vector<T> vector_with_capacity(const SizeType<std::vector<T>> n) {
  std::vector<T> v;
  v.reserve(n);
  return v;
}

template <class T>
std::string type_name() {
  using TR = RemoveReference<T>;

  std::string name = detail::unmangled_name<TR>();

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4127)
#endif
  if (std::is_const<TR>::value) {
    name += " const";
  }

  if (std::is_volatile<TR>::value) {
    name += " volatile";
  }

  if (std::is_lvalue_reference<T>::value) {
    name += "&";
  } else if (std::is_rvalue_reference<T>::value) {
    name += "&&";
  }
#ifdef _MSC_VER
#pragma warning(pop)
#endif
  return name;
}
}

#endif // VELOX_UTIL_H_INCLUDED
