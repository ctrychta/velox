#ifndef VELOX_UTIL_H_INCLUDED
#define VELOX_UTIL_H_INCLUDED

#include <cstdint>
#include <cstdlib>
#include <memory>
#include <vector>
#include <cassert>
#include <chrono>
#include <string>
#include <sstream>
#include <initializer_list>
#include <atomic>

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

template <class T>
void unused(std::initializer_list<T> &&) {
}

template <std::size_t...>
struct Seq {
  using type = Seq;
};

template <class S1, class S2>
struct SeqBuilder;

template <std::size_t... I1, std::size_t... I2>
struct SeqBuilder<Seq<I1...>, Seq<I2...>> : Seq<I1..., (sizeof...(I1)+I2)...> {};

template <class S1, class S2>
using BuildSeq = Invoke<SeqBuilder<S1, S2>>;

template <std::size_t N>
struct SeqMaker;

template <std::size_t N>
using MakeSeq = Invoke<SeqMaker<N>>;

template <std::size_t N>
struct SeqMaker : BuildSeq<MakeSeq<N / 2>, MakeSeq<N - N / 2>> {};

template <>
struct SeqMaker<0> : Seq<> {};
template <>
struct SeqMaker<1> : Seq<0> {};

template <bool B, class...>
struct DependentBool : std::integral_constant<bool, B> {};

template <bool B, class... Ts>
using Bool = Invoke<DependentBool<B, Ts...>>;

template <class If, class Then, class Else>
using Conditional = Invoke<std::conditional<If::value, Then, Else>>;

template <class... Ts>
struct All : Bool<true> {};

template <class Head, class... Tail>
struct All<Head, Tail...> : Conditional<Head, All<Tail...>, Bool<false>> {};

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

struct StreamInsertTester {
  template <class T>
  static decltype(std::declval<std::ostream &>() << std::declval<T>(), std::true_type()) test(int);

  template <class T>
  static std::false_type test(...);
};

template <class T>
struct IsStreamInsertable : decltype(StreamInsertTester::test<T>(0)) {};

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
