#ifndef VELOX_MEASUREMENT_H_INCLUDED
#define VELOX_MEASUREMENT_H_INCLUDED

#include "point.h"

namespace velox {

struct Measurement {

  Measurement(std::uint64_t iterations, Ns time) : iters_(iterations), duration_(time) {}

  std::uint64_t iters() const { return iters_; }

  Ns duration() const { return duration_; }

private:
  std::uint64_t iters_;
  Ns duration_;
};

using Measurements = std::vector<Measurement>;

inline Points measurements_to_points(const Measurements &measurements) {
  auto ps = vector_with_capacity<Point>(measurements.size());
  for (const auto &m : measurements) {
    ps.emplace_back(static_cast<double>(m.iters()), static_cast<double>(m.duration().count()));
  }

  return ps;
}
}

#endif // VELOX_MEASUREMENT_H_INCLUDED
