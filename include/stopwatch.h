#ifndef VELOX_STOPWATCH_H_INCLUDED
#define VELOX_STOPWATCH_H_INCLUDED

#include "util.h"

namespace velox {

namespace detail {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#endif
  struct StopwatchConcept {
    virtual ~StopwatchConcept() = default;
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual std::uint64_t iters() const = 0;
  };
#ifdef __clang__
#pragma clang diagnostic pop
#endif
  template <class C>
  struct StopwatchModel final : StopwatchConcept {

    StopwatchModel(const std::uint64_t iterations) : iters_(iterations) {
      assert(iters_ && "Must iterate at least once");
    }

    void start() override { start_time_ = C::now(); }

    void stop() override { stop_time_ = C::now(); }

    std::uint64_t iters() const override { return iters_; }

    Ns elapsed() const { return std::chrono::duration_cast<Ns>(stop_time_ - start_time_); }

  private:
    TimePoint<C> start_time_;
    TimePoint<C> stop_time_;
    std::uint64_t iters_;
  };
}

struct Stopwatch {
  template <class F>
  Stopwatch(detail::StopwatchConcept &sw, F &&f)
      : sw_(sw)
#ifndef NDEBUG
        ,
        measure_called_(false)
#endif
  {
    run(std::forward<F>(f), IsCallable<F, Stopwatch &>());
  }

  Stopwatch &operator=(const Stopwatch &rhs) = delete;

  template <class F>
  void measure(F &&f) {
#ifndef NDEBUG
    assert(!measure_called_ && "Measure should only be called once");
#endif
    sw_.start();
    for (auto i = sw_.iters(); i != 0; --i) {
      f();
    }
    sw_.stop();
#ifndef NDEBUG
    measure_called_ = true;
#endif
  }

private:
  template <class F>
  void run(F &&f, std::false_type) {
    sw_.start();
    for (auto i = sw_.iters(); i != 0; --i) {
      f();
    }
    sw_.stop();
  }

  template <class F>
  void run(F &&f, std::true_type) {
    std::forward<F>(f)(*this);
#ifndef NDEBUG
    assert(measure_called_ && "The callable being timed must call measure");
#endif
  }

private:
  detail::StopwatchConcept &sw_;
#ifndef NDEBUG
  bool measure_called_;
#endif
};
}

#endif // VELOX_STOPWATCH_H_INCLUDED
