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

private:
  VeloxConfig config_;
  Reporter &reporter_;
};
}

#endif // VELOX_VELOX_H_INCLUDED
