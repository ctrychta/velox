#ifndef VELOX_VELOX_H_INCLUDED
#define VELOX_VELOX_H_INCLUDED

#include "stats.h"
#include "bootstrap.h"
#include "reporter.h"
#include "velox_config.h"
#include "benchmark.h"
#include "text_reporter.h"
#include "html_reporter.h"
#include "multi_reporter.h"

#include <chrono>
#include <cstdint>
#include <cassert>
#include <vector>
#include <initializer_list>
#include <tuple>

namespace velox {

template <class C = DefaultClock>
struct Velox {
  Velox(Reporter &reporter, const VeloxConfig &config = VeloxConfig())
      : config_(config), reporter_(reporter) {
    reporter_.suite_starting(type_name<C>(), C::is_steady);
    if (config.estimate_clock_cost()) {
      estimate_clock_cost<C>(config_, reporter_);
    }
  }

  Velox &operator=(const Velox &rhs) = delete;

  ~Velox() { reporter_.suite_ended(); }

  template <class F>
  Velox &bench(const std::string &name, F &&f) {
    benchmark<C>(name, std::forward<F>(f), config_, reporter_);
    return *this;
  }

  template <class F, class A>
  Velox &bench_with_arg(const std::string &name, F &&f, std::initializer_list<A> args) {
    static_assert(IsStreamInsertable<A>::value,
                  "Arg does not have operator<<.  A custom formatter must be provided.");

    bench_with_arg(name, std::forward<F>(f), args, Formatter());
    return *this;
  }

  template <class F, class A, class Formatter>
  Velox &bench_with_arg(const std::string &name,
                        F &&f,
                        std::initializer_list<A> args,
                        Formatter &&formatter) {
    for (auto &a : args) {
      std::stringstream ss;
      ss << name << " / ";
      formatter(ss, a);

      auto arg = std::tie(a);
      bench_with_args_impl(ss.str(), f, arg);
    }

    return *this;
  }

  template <class F, class... As>
  Velox &
  bench_with_args(const std::string &name, F &&f, std::initializer_list<std::tuple<As...>> args) {
    static_assert(All<IsStreamInsertable<As>...>::value,
                  "One or more args do not have operator<<.  A custom formatter must be provided.");

    bench_with_args(name, std::forward<F>(f), args, TupleFormatter());
    return *this;
  }

  template <class F, class... As, class Formatter>
  Velox &bench_with_args(const std::string &name,
                         F &&f,
                         std::initializer_list<std::tuple<As...>> args,
                         Formatter &&formatter) {
    for (auto &a : args) {
      std::stringstream ss;
      ss << name << " / ";
      formatter(ss, a);

      bench_with_args_impl(ss.str(), f, a);
    }

    return *this;
  }

private:
  struct Formatter {
    template <class T>
    void operator()(std::ostream &os, const T &t) const {
      os << t;
    }
  };

  struct TupleFormatter {
    template <class... Ts>
    void operator()(std::ostream &os, const std::tuple<Ts...> &t) const {
      operator()(os, t, MakeSeq<sizeof...(Ts)>());
    }

    template <class Tuple, std::size_t... Is>
    void operator()(std::ostream &os, const Tuple &t, Seq<Is...>) const {
      unused({0, (os << (Is == 0 ? "" : ", ") << std::get<Is>(t), void(), 0)...});
    }
  };

  template <class F, class... As>
  void bench_with_args_impl(const std::string &name, F &f, const std::tuple<As...> &args) {
    bench_with_args_impl(
        name, f, args, MakeSeq<sizeof...(As)>(), IsCallable<F, Stopwatch &, As...>());
  }

  template <class F, class TupledArgs, std::size_t... Is>
  void bench_with_args_impl(
      const std::string &name, F &f, const TupledArgs &args, Seq<Is...>, std::true_type) {
    static_assert(
        IsCallable<F, Stopwatch &, decltype(std::get<Is>(args))...>::value,
        "Function not callable with args.  Perhaps the function is taking non-const references?");

    benchmark<C>(name,
                 [&f, &args](Stopwatch &sw) { return f(sw, std::get<Is>(args)...); },
                 config_,
                 reporter_);
  }

  template <class F, class TupledArgs, std::size_t... Is>
  void bench_with_args_impl(
      const std::string &name, F &f, const TupledArgs &args, Seq<Is...>, std::false_type) {
    static_assert(
        IsCallable<F, decltype(std::get<Is>(args))...>::value,
        "Function not callable with args.  Perhaps the function is taking non-const references?");

    benchmark<C>(name, [&f, &args] { return f(std::get<Is>(args)...); }, config_, reporter_);
  }

private:
  VeloxConfig config_;
  Reporter &reporter_;
};
}

#endif // VELOX_VELOX_H_INCLUDED
