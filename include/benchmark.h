#ifndef VELOX_BENCHMARK_H_INCLUDED
#define VELOX_BENCHMARK_H_INCLUDED

#include "util.h"
#include "stopwatch.h"
#include "outliers.h"
#include "iters_for_duration.h"

namespace velox {

template <class C, class F>
struct Benchmark {
  Benchmark(F &f) : f_(f) {}

  Benchmark &operator=(const Benchmark &rhs) = delete;

  ItersForDurationNs warm_up(const Duration<C> duration) {
    std::uint64_t iters = 1;

    const auto start = C::now();

    for (;;) {
      const auto elapsed = run(iters);

      if (C::now() - start > duration) {
        return {iters, elapsed};
      }

      iters *= 2;
    }
  }

  Ns run(const std::uint64_t iters) {
    detail::StopwatchModel<C> sm(iters);
    Stopwatch(sm, f_);
    return sm.elapsed();
  }

  Measurements bench(const std::uint32_t num_measurements, const std::uint64_t base_iters) {
    auto measurements = vector_with_capacity<Measurement>(num_measurements);

    std::uint64_t iters = base_iters;
    std::generate_n(std::back_inserter(measurements), num_measurements, [this, base_iters, &iters] {
      iters += base_iters;
      return Measurement(iters, run(iters));
    });

    return measurements;
  }

private:
  F &f_;
};

inline Times times_from_measurements(const Measurements &measurements) {
  auto ts = vector_with_capacity<FpNs>(measurements.size());

  for (const auto &m : measurements) {
    ts.push_back(FpNs{static_cast<double>(m.duration().count()) / static_cast<double>(m.iters())});
  }

  return ts;
}

template <class C, class F>
Measurements measure(F &&f, const VeloxConfig &config, Reporter &reporter) {
  reporter.warm_up_starting(config.warm_up_time());

  Benchmark<C, F> b(f);

  const auto wu = b.warm_up(config.warm_up_time());

  reporter.warm_up_ended(wu);

  const auto mean_execution_time = static_cast<double>(wu.duration().count()) / wu.iters();
  const auto mt =
      static_cast<double>(std::chrono::duration_cast<Ns>(config.measurement_time()).count());
  const auto nm = config.num_measurements();
  const auto base_iters = static_cast<std::uint64_t>(
      std::ceil(2.0 * mt / mean_execution_time / static_cast<double>(nm * (nm + 1))));
  const auto total_iters = [&config, &base_iters]() -> std::uint64_t {
    std::uint64_t sum = 0;
    auto iters = base_iters;
    for (std::uint32_t i = 0; i < config.num_measurements(); ++i) {
      iters += base_iters;
      sum += iters;
    }
    return sum;
  }();

  const auto estimated_time = FpNs{total_iters * mean_execution_time};

  reporter.measurement_collection_starting(nm, estimated_time);

  return b.bench(nm, base_iters);
}

template <class C, class F>
void benchmark(const std::string &name, F &&f, const VeloxConfig &config, Reporter &reporter) {
  reporter.benchmark_starting(name);

  const auto measurements = measure<C>(std::forward<F>(f), config, reporter);
  const auto times = times_from_measurements(measurements);
  const Outliers outliers(times);

  reporter.measurement_collection_ended(measurements, times, outliers);

  reporter.estimate_statistics_starting(config.num_resamples());

  const auto statistics =
      estimate_statistics(measurements, times, config.num_resamples(), config.confidence_level());

  reporter.estimate_statistics_ended(statistics);
  reporter.benchmark_ended();
}

template <class C>
FpNs estimate_clock_cost(const VeloxConfig &config, Reporter &reporter) {
  reporter.estimate_clock_cost_starting();

  const auto measurements = measure<C>([]() { C::now(); }, config, reporter);
  const auto times = times_from_measurements(measurements);
  const auto cost = median(times);

  reporter.estimate_clock_cost_ended(cost);

  return cost;
}
}

#endif // VELOX_BENCHMARK_H_INCLUDED
