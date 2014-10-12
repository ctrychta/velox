#ifndef VELOX_REGRESSION_H_INCLUDED
#define VELOX_REGRESSION_H_INCLUDED

#include "point.h"

#include <cmath>
#include <numeric>

namespace velox {

// Regression through the origin

inline double slope(const Points &points) {
  double xy = 0.0, xx = 0;

  for (const auto &p : points) {
    xy += p.x() * p.y();
    xx += p.x() * p.x();
  }

  const auto n = static_cast<double>(points.size());
  xy /= n;
  xx /= n;

  return xy / xx;
}

inline double r_squared(const Points &points, const double slope) {
  const auto residual_sum_of_squares = [&points, &slope]() -> double {
    double sum = 0.0;
    for (const auto &p : points) {
      const auto diff = p.y() - (slope * p.x());
      sum += diff * diff;
    }
    return sum;
  }();

  const auto total_sum_of_squares = [&points]() -> double {
    double sum = 0.0;
    for (const auto &p : points) {
      sum += p.y() * p.y();
    }
    return sum;
  }();

  return 1.0 - (residual_sum_of_squares / total_sum_of_squares);
}
}

#endif // VELOX_REGRESSION_H_INCLUDED
