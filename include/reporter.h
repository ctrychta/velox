#ifndef VELOX_REPORTER_H_INCLUDED
#define VELOX_REPORTER_H_INCLUDED

#include "iters_for_duration.h"
#include "outliers.h"
#include "measurement.h"
#include "kde.h"
#include "format.h"
#include "point.h"

namespace velox {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#endif
struct Reporter {
  virtual ~Reporter() = 0;

  virtual void suite_starting(const std::string &clock, bool is_steady) {
    unused(clock, is_steady);
  }

  virtual void estimate_clock_cost_starting() {}
  virtual void estimate_clock_cost_ended(FpNs cost) { unused(cost); }

  virtual void warm_up_starting(Ms ms) { unused(ms); }
  virtual void warm_up_ended(const ItersForDurationNs &wu) { unused(wu); }
  virtual void warm_up_failed(const ItersForDurationNs &wu) { unused(wu); }

  virtual void benchmark_starting(const std::string &name) { unused(name); }
  virtual void benchmark_ended() {}

  virtual void measurement_collection_starting(std::uint32_t num_measurements,
                                               FpNs measurement_time) {
    unused(num_measurements, measurement_time);
  }

  virtual void measurement_collection_ended(const Measurements &measurements,
                                            const Times &times,
                                            const Outliers &outliers) {
    unused(measurements, times, outliers);
  }

  virtual void estimate_statistics_starting(std::uint32_t num_resamples) { unused(num_resamples); }

  virtual void estimate_statistics_ended(const EstimatedStatistics &statistics) {
    unused(statistics);
  }

  virtual void suite_ended() {}
};

inline Reporter::~Reporter() {
}
#ifdef __clang__
#pragma clang diagnostic pop
#endif
}

#endif // VELOX_REPORTER_H_INCLUDED
