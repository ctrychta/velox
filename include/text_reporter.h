#ifndef VELOX_TEXT_REPORTER_H_INCLUDED
#define VELOX_TEXT_REPORTER_H_INCLUDED

#include "reporter.h"

namespace velox {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#endif
struct TextReporter : Reporter {
  TextReporter(std::ostream &os) : os_(os) {}

  TextReporter &operator=(const TextReporter &rhs) = delete;

  void suite_starting(const std::string &clock, bool is_steady) override {
    os_ << "Benchmarking with `" << clock << "` which is " << (is_steady ? "steady" : "unsteady")
        << "\n";
  }

  void estimate_clock_cost_starting() override { os_ << "Estimating the cost of the clock\n"; }

  void estimate_clock_cost_ended(FpNs cost) override {
    os_ << "> Median: " << cost.count() << " ns\n\n";
  }

  void benchmark_starting(const std::string &name) override {
    os_ << "Benchmarking " << name << "\n";
  }

  void warm_up_starting(Ms ms) override { os_ << "> Warming up for " << ms.count() << " ms\n"; }

  void measurement_collection_starting(std::uint32_t sample_size, FpNs measurement_time) override {
    os_ << "> Collecting " << sample_size << " measurements in estimated ";

    format_time(os_, measurement_time);
    os_ << "\n";
  }

  void measurement_collection_ended(const Measurements &,
                                    const Times &,
                                    const Outliers &outliers) override {
    const auto num_high_severe = outliers.high_severe().size();
    const auto num_high_mild = outliers.high_mild().size();
    const auto num_low_mild = outliers.low_mild().size();
    const auto num_low_severe = outliers.low_severe().size();
    const auto total = num_high_severe + num_high_mild + num_low_mild + num_low_severe;

    const auto sample_size = total + outliers.normal().size();

    const auto percent = [sample_size](decltype(total) n) {
      return 100.0 * static_cast<double>(n) / static_cast<double>(sample_size);
    };

    os_ << "> Found " << total << " outliers among " << sample_size << " measurements ("
        << percent(total) << "%)\n";

    if (total == 0) {
      return;
    }

    const auto print = [this, &percent](decltype(total) n, const char *const type) {
      if (n == 0) {
        return;
      }

      os_ << "  > " << n << " (" << percent(n) << "%) " << type << "\n";
    };

    print(num_low_severe, "low severe");
    print(num_low_mild, "low mild");
    print(num_high_mild, "high mild");
    print(num_high_severe, "high severe");
  }

  void estimate_statistics_starting(std::uint32_t num_resamples) override {
    os_ << "> estimating statistics\n";
    os_ << "  > bootstrapping sample with " << num_resamples << " resamples\n";
  }

  void estimate_statistics_ended(const EstimatedStatistics &statistics) override {
    auto format_estimate = [this](const Estimate<FpNs> &e) { format(e, format_time); };

    os_ << "  > mean   ";
    format_estimate(statistics.mean().estimate());
    os_ << "  > median ";
    format_estimate(statistics.median().estimate());
    os_ << "  > SD     ";
    format_estimate(statistics.std_dev().estimate());
    os_ << "  > MAD    ";
    format_estimate(statistics.median_abs_dev().estimate());
    os_ << "  > LLS    ";
    format_estimate(statistics.linear_least_squares().estimate());
    os_ << "  > r^2    ";
    format(statistics.r_squared().estimate(), format_r2);
    os_ << "\n";
  }

private:
  template <class E, class F>
  void format(const E &e, F &&f) {
    f(os_, e.point());
    os_ << " +/- ";
    f(os_, e.standard_error());
    os_ << " [";
    f(os_, e.lower_bound());
    os_ << " ";
    f(os_, e.upper_bound());
    os_ << "] ";
    os_ << e.confidence_level() * 100 << "% CI\n";
  }

private:
  std::ostream &os_;
};
#ifdef __clang__
#pragma clang diagnostic pop
#endif
}

#endif // VELOX_TEXT_REPORTER_H_INCLUDED
