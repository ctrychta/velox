#ifndef VELOX_KDE_H_INCLUDED
#define VELOX_KDE_H_INCLUDED

#include "fp_range.h"
#include "stats.h"
#include "point.h"

#include <cmath>
#include <cstdint>

namespace velox {

namespace {
  const std::uint32_t DEFAULT_KDE_POINTS = 400;
}

inline double snpdf(const double x) {
  // Don't require users to #define _USE_MATH_DEFINES when using msvc so ...
  const auto PI = 3.14159265358979323846;
  return std::exp(-x * x / 2.0) / std::sqrt(2.0 * PI);
}

template <class F>
void linspace(const double start, const double stop, const std::uint32_t n, F &&f) {
  if (n == 0) {
    return;
  } else if (n == 1) {
    f(start);
    return;
  }

  const auto step = (stop - start) / static_cast<double>(n - 1);

  for (std::uint32_t i = 0; i < n - 1; ++i) {
    f(i * step + start);
  }

  f(stop);
}

inline double bandwidth_scott(const Times &times) {
  const auto sd = std_dev(FpRange(times));
  const auto adjusted_iqr = quartiles(FpRange(times)).iqr() / 1.34;
  return 1.06 * (std::min)(sd, adjusted_iqr) * std::pow(static_cast<double>(times.size()), -.2);
}

inline Points kde(const Times &times, const std::uint32_t num_points) {
  assert(!times.empty() && "times must not be empty");

  const auto n = static_cast<double>(times.size());
  const auto bw = bandwidth_scott(times);

  const auto adjustment = 3.0;
  const auto mm = std::minmax_element(times.begin(), times.end());
  const auto start = mm.first->count() - adjustment * bw;
  const auto stop = mm.second->count() + adjustment * bw;

  const auto kde_calc = [&times, &n, &bw](double x) -> double {
    double sum = 0.0;
    for (const auto &t : times) {
      sum += snpdf((x - t.count()) / bw);
    }

    return sum / n / bw;
  };

  auto points = vector_with_capacity<Point>(num_points);

  linspace(start, stop, num_points, [&kde_calc, &points](double x) {
    points.emplace_back(x, kde_calc(x));
  });

  return points;
}
}

#endif // VELOX_KDE_H_INCLUDED
