#ifndef VELOX_BOOTSTRAP_H_INCLUDED
#define VELOX_BOOTSTRAP_H_INCLUDED

#include "util.h"
#include "fp_range.h"
#include "stats.h"
#include "regression.h"
#include "measurement.h"

#include <random>
#include <future>

namespace velox {

template <class T>
struct Estimate {
  Estimate(const T p, const T sd, const T lb, const T ub, const double cl)
      : point_(p), standard_error_(sd), lower_bound_(lb), upper_bound_(ub), confidence_level_(cl) {
    assert(cl > 0.0 && cl < 1.0 && "Confidence level must be between 0 and 1");
  }

  T point() const { return point_; }

  T standard_error() const { return standard_error_; }

  T lower_bound() const { return lower_bound_; }

  T upper_bound() const { return upper_bound_; }

  double confidence_level() const { return confidence_level_; }

private:
  T point_;
  T standard_error_;
  T lower_bound_;
  T upper_bound_;
  double confidence_level_;
};

inline Estimate<FpNs> make_estimate(const FpNs p, Times bootstrap, const double cl) {
  std::sort(bootstrap.begin(), bootstrap.end());
  const FpRange r(bootstrap);

  return Estimate<FpNs>(p,
                        FpNs(std_dev(r)),
                        FpNs(percentile_of_sorted(r, 50.0 * (1.0 - cl))),
                        FpNs(percentile_of_sorted(r, 50.0 * (1.0 + cl))),
                        cl);
}

inline Estimate<double>
make_estimate(const double p, std::vector<double> bootstrap, const double cl) {
  std::sort(bootstrap.begin(), bootstrap.end());

  return Estimate<double>(p,
                          std_dev(bootstrap),
                          percentile_of_sorted(bootstrap, 50.0 * (1.0 - cl)),
                          percentile_of_sorted(bootstrap, 50.0 * (1.0 + cl)),
                          cl);
}

template <class T>
struct EstimateAndDistribution {
  EstimateAndDistribution(const Estimate<T> &est, std::vector<T> &&dist)
      : estimate_(est), distribution_(std::move(dist)) {}

  const Estimate<T> &estimate() const { return estimate_; }

  const std::vector<T> distribution() const { return distribution_; }

private:
  Estimate<T> estimate_;
  std::vector<T> distribution_;
};

struct EstimatedStatistics {
  EstimatedStatistics(EstimateAndDistribution<FpNs> &&means,
                      EstimateAndDistribution<FpNs> &&medians,
                      EstimateAndDistribution<FpNs> &&std_devs,
                      EstimateAndDistribution<FpNs> &&mads,
                      EstimateAndDistribution<FpNs> &&lls,
                      EstimateAndDistribution<double> &&r2s)
      : mean_(std::move(means)), median_(std::move(medians)), std_dev_(std::move(std_devs)),
        median_abs_dev_(std::move(mads)), linear_least_squares_(std::move(lls)),
        r_squared_(std::move(r2s)) {}

  const EstimateAndDistribution<FpNs> &mean() const { return mean_; }

  const EstimateAndDistribution<FpNs> &median() const { return median_; }

  const EstimateAndDistribution<FpNs> &std_dev() const { return std_dev_; }

  const EstimateAndDistribution<FpNs> &median_abs_dev() const { return median_abs_dev_; }

  const EstimateAndDistribution<FpNs> &linear_least_squares() const {
    return linear_least_squares_;
  }

  const EstimateAndDistribution<double> &r_squared() const { return r_squared_; }

private:
  EstimateAndDistribution<FpNs> mean_;
  EstimateAndDistribution<FpNs> median_;
  EstimateAndDistribution<FpNs> std_dev_;
  EstimateAndDistribution<FpNs> median_abs_dev_;
  EstimateAndDistribution<FpNs> linear_least_squares_;
  EstimateAndDistribution<double> r_squared_;
};

template <template <class> class D, class T, class S, class F>
void
resample(const std::vector<T> &sample, const std::uint32_t num_resamples, const S seed, F &&f) {
  std::mt19937 rng(static_cast<std::mt19937::result_type>(seed));
  D<std::size_t> distribution(0, sample.size() - 1);
  auto resample = sample;

  for (std::uint32_t i = 0; i < num_resamples; ++i) {
    std::generate(resample.begin(), resample.end(), [&] { return sample[distribution(rng)]; });

    f(resample);
  }
}

template <template <class> class D = std::uniform_int_distribution>
inline EstimatedStatistics estimate_statistics(const Measurements &measurements,
                                               const Times &times,
                                               const std::uint32_t num_resamples,
                                               const double cl) {
  auto means = vector_with_capacity<FpNs>(num_resamples);
  auto medians = vector_with_capacity<FpNs>(num_resamples);
  auto std_devs = vector_with_capacity<FpNs>(num_resamples);
  auto mads = vector_with_capacity<FpNs>(num_resamples);

  const auto points = measurements_to_points(measurements);
  auto lls = vector_with_capacity<FpNs>(num_resamples);
  auto r2s = vector_with_capacity<double>(num_resamples);

  auto mad_buffer = vector_with_capacity<double>(times.size());

  const auto seed = std::random_device{}();

  auto sorted_stat_calcs = std::async(std::launch::async, [&] {
    resample<D>(times, num_resamples, seed, [&](Times &s) {
      std::sort(s.begin(), s.end());
      FpRange r(s);

      medians.push_back(FpNs(median_of_sorted(r)));
      mads.push_back(FpNs(median_abs_dev_of_sorted_destructive(r, mad_buffer)));
    });
  });

  auto stat_calcs = std::async(std::launch::async, [&] {
    resample<D>(times, num_resamples, seed, [&](const Times &s) {
      FpRange r(s);
      means.push_back(FpNs(mean(r)));
      std_devs.push_back(FpNs(std_dev(r)));
    });
  });

  resample<D>(points, num_resamples, seed, [&](const Points &ps) {
    const auto s = slope(ps);
    lls.push_back(FpNs{s});
    r2s.push_back(r_squared(ps, s));
  });

  sorted_stat_calcs.get();
  stat_calcs.get();

  const auto sorted_sample = [&times]() -> Times {
    auto temp = times;
    std::sort(temp.begin(), temp.end());
    return temp;
  }();
  const FpRange r(sorted_sample);
  const auto mean_point = FpNs{mean(r)};
  const auto median_point = FpNs{median_of_sorted(r)};
  const auto std_dev_point = FpNs{std_dev(r)};
  const auto mad_point = FpNs{median_abs_dev_of_sorted_destructive(r, mad_buffer)};

  const auto lls_point = FpNs{slope(points)};
  const auto r2_point = r_squared(points, lls_point.count());

  return EstimatedStatistics(
      EstimateAndDistribution<FpNs>(make_estimate(mean_point, means, cl), std::move(means)),
      EstimateAndDistribution<FpNs>(make_estimate(median_point, medians, cl), std::move(medians)),
      EstimateAndDistribution<FpNs>(make_estimate(std_dev_point, std_devs, cl),
                                    std::move(std_devs)),
      EstimateAndDistribution<FpNs>(make_estimate(mad_point, mads, cl), std::move(mads)),
      EstimateAndDistribution<FpNs>(make_estimate(lls_point, lls, cl), std::move(lls)),
      EstimateAndDistribution<double>(make_estimate(r2_point, r2s, cl), std::move(r2s)));
}
}

#endif // VELOX_BOOTSTRAP_H_INCLUDED
