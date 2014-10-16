#ifndef VELOX_MULTI_REPORTER_H_INCLUDED
#define VELOX_MULTI_REPORTER_H_INCLUDED

#include "reporter.h"

namespace velox {
struct MultiReporter : Reporter {
  template <class... Rs>
  MultiReporter(Rs &... rs) {
    static_assert(sizeof...(rs), "At least one reporter is required");
    unused((reporters_.push_back(&rs), 0)...);
  }

  void suite_starting(const std::string &clock, bool is_steady) override {
    call(fp(&Reporter::suite_starting), clock, is_steady);
  }

  void estimate_clock_cost_starting() override {
    call(fp(&Reporter::estimate_clock_cost_starting));
  }

  void estimate_clock_cost_ended(FpNs cost) override {
    call(fp(&Reporter::estimate_clock_cost_ended), cost);
  }

  void warm_up_starting(Ms ms) override { call(fp(&Reporter::warm_up_starting), ms); }

  void warm_up_ended(const ItersForDurationNs &wu) override {
    call(fp(&Reporter::warm_up_ended), wu);
  }

  void warm_up_failed(const ItersForDurationNs &wu) override {
    call(fp(&Reporter::warm_up_failed), wu);
  }

  void benchmark_starting(const std::string &name) override {
    call(fp(&Reporter::benchmark_starting), name);
  }

  void benchmark_ended() override { call(fp(&Reporter::benchmark_ended)); }

  void measurement_collection_starting(std::uint32_t sample_size, FpNs measurement_time) override {
    call(fp(&Reporter::measurement_collection_starting), sample_size, measurement_time);
  }

  void measurement_collection_ended(const Measurements &measurements,
                                    const Times &times,
                                    const Outliers &outliers) override {
    call(fp(&Reporter::measurement_collection_ended), measurements, times, outliers);
  }

  void estimate_statistics_starting(std::uint32_t num_resamples) override {
    call(fp(&Reporter::estimate_statistics_starting), num_resamples);
  }

  void estimate_statistics_ended(const EstimatedStatistics &statistics) override {
    call(fp(&Reporter::estimate_statistics_ended), statistics);
  }

  void suite_ended() override { call(fp(&Reporter::suite_ended)); }

private:
  template <class F>
  struct FuncPtr {
    FuncPtr(F f) : f_(f) {}

    template <class... Args>
    void operator()(Reporter *reporter, const Args &... args) {
      (reporter->*f_)(args...);
    }

  private:
    F f_;
  };

  template <class F>
  FuncPtr<F> fp(F f) {
    return FuncPtr<F>(f);
  }

  template <class F, class... Args>
  void call(FuncPtr<F> f, const Args &... args) {
    for (auto r : reporters_) {
      f(r, args...);
    }
  }

private:
  std::vector<Reporter *> reporters_;
};
}

#endif // VELOX_MULTI_REPORTER_H_INCLUDED
