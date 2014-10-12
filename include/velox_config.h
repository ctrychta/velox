#ifndef VELOX_VELOX_CONFIG_H_INCLUDED
#define VELOX_VELOX_CONFIG_H_INCLUDED

namespace velox {

struct VeloxConfig {
  VeloxConfig()
      : confidence_level_(0.95), measurement_time_(10000), num_resamples_(100000),
        num_measurements_(100), warm_up_time_(5000), estimate_clock_cost_(false) {}

  // Used when calculating the https://en.wikipedia.org/wiki/Confidence_interval
  // of the various statistics
  VeloxConfig &confidence_level(const double cl) {
    assert(cl > 0.0 && cl < 1.0 && "Confidence level must be between 0 and 1");
    confidence_level_ = cl;
    return *this;
  }

  double confidence_level() const { return confidence_level_; }

  // The amount of time to use for taking num_measurements
  // This is not a strict upper bound, and the actual measurement time will probably be
  // a bit larger.
  VeloxConfig &measurement_time(const Ms ms) {
    assert(ms.count() > 0 && "Must measure for at least 1 ms");
    measurement_time_ = ms;
    return *this;
  }

  std::chrono::milliseconds measurement_time() const { return measurement_time_; }

  // Number of resamples to use for
  // http://en.wikipedia.org/wiki/Bootstrapping_%28statistics%29
  VeloxConfig &num_resamples(const std::uint32_t n) {
    assert(n && "Must resample at least once");
    num_resamples_ = n;
    return *this;
  }

  std::uint32_t num_resamples() const { return num_resamples_; }

  // The number of measurements to take
  VeloxConfig &num_measurements(const std::uint32_t n) {
    assert(n && "At least one sample must be taken");
    num_measurements_ = n;
    return *this;
  }

  std::uint32_t num_measurements() const { return num_measurements_; }

  // How long to warm up for
  VeloxConfig &warm_up_time(const Ms ms) {
    assert(ms.count() > 0 && "Must warm up for at least 1 ms");
    warm_up_time_ = ms;
    return *this;
  }

  std::chrono::milliseconds warm_up_time() const { return warm_up_time_; }

  // Whether to estimate the clock cost
  // Currently the clock cost is only reported, it is not used in any
  // calculations
  VeloxConfig &estimate_clock_cost(bool estimate) {
    estimate_clock_cost_ = estimate;
    return *this;
  }

  bool estimate_clock_cost() const { return estimate_clock_cost_; }

private:
  double confidence_level_;
  Ms measurement_time_;
  std::uint32_t num_resamples_;
  std::uint32_t num_measurements_;
  Ms warm_up_time_;
  bool estimate_clock_cost_;
};
}

#endif // VELOX_VELOX_CONFIG_H_INCLUDED
