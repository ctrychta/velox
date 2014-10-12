#ifndef VELOX_STATS_H_INCLUDED
#define VELOX_STATS_H_INCLUDED

#include "util.h"

#include <cassert>
#include <iterator>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace velox {

// These functions do not handle non-finite values

template <class Range>
VELOX_RVT(Range) sum(Range &&r) {
  // The kerbechuk algorithm is quite slow and shouldn't be needed for
  // the kinds of data velox will be interacting with so just use accumulate for
  // now
  auto first = adl::adl_begin(r), last = adl::adl_end(r);
  return std::accumulate(first, last, 0.0);
}

template <class Range>
VELOX_RVT(Range) mean(Range &&r) {
  auto first = adl::adl_begin(r), last = adl::adl_end(r);
  const auto count = last - first;
  assert(count && "Mean calculation requires at least one value");
  return sum(r) / static_cast<double>(count);
}

template <class Range>
VELOX_RVT(Range) percentile_of_sorted(Range &&samples, const double percentile) {
  auto first = adl::adl_begin(samples), last = adl::adl_end(samples);

  assert(first != last && "Samples requires at least one value");
  assert(percentile > 0.0 && percentile <= 100.0 && "Percentile must be between 0 and 100");

  if ((last - first) == 1) {
    return *first;
  }
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wfloat-equal"
#elif defined __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif
  if (percentile == 100.0) {
    return *(--last);
  }
#ifdef __clang__
#pragma clang diagnostic pop
#elif defined __GNUC__
#pragma GCC diagnostic pop
#endif
  const auto len = last - first - 1;
  const auto rank = (percentile / 100.0) * len;
  const auto lrank = std::floor(rank);
  const auto d = rank - lrank;
  const auto n = static_cast<std::uint32_t>(lrank);
  const auto lo = first[n];
  const auto hi = first[n + 1];
  return lo + (hi - lo) * d;
}

template <class Range>
VELOX_RVT(Range) percentile(Range &&r, const double percentile) {
  std::vector<VELOX_RVT(Range)> temp(adl::adl_begin(r), adl::adl_end(r));
  std::sort(temp.begin(), temp.end());
  return percentile_of_sorted(temp, percentile);
}

template <class Range>
VELOX_RVT(Range) median(Range &&r) {
  return percentile(std::forward<Range>(r), 50);
}

template <class Range>
VELOX_RVT(Range) median_of_sorted(Range &&r) {
  return percentile_of_sorted(std::forward<Range>(r), 50);
}

template <class Range>
VELOX_RVT(Range) median_destructive(Range &&r) {
  auto first = adl::adl_begin(r), last = adl::adl_end(r);
  assert(first != last && "r must have at least one value");

  const auto len = last - first;
  const auto middle_index = len / 2;
  const auto mid = first + middle_index;
  std::nth_element(first, mid, last);

  if (len % 2 != 0) {
    return *mid;
  } else {
    auto m = *mid;
    auto n = std::max_element(first, mid);
    return (m + *n) / 2.0;
  }
}

template <class Range>
VELOX_RVT(Range) variance(Range &&r) {
  auto first = adl::adl_begin(r), last = adl::adl_end(r);
  const auto len = last - first;

  if (len < 2) {
    return 0;
  }

  const auto avg = mean(r);
  VELOX_RVT(Range) v = 0;
  for (const auto &s : r) {
    const auto x = s - avg;
    v += x * x;
  }

  return v / (len - 1);
}

template <class Range>
VELOX_RVT(Range) std_dev(Range &&r) {
  return std::sqrt(variance(std::forward<Range>(r)));
}

template <class Range>
VELOX_RVT(Range)
    median_abs_dev_of_sorted_destructive(Range &&r,
                                         std::vector<VELOX_RVT(Range)> &abs_devs_buffer) {
  const auto med = median_of_sorted(r);

  abs_devs_buffer.clear();
  for (const auto &v : r) {
    abs_devs_buffer.push_back(std::abs(med - v));
  }

  // This seems to be the generally accepted constant and is used
  // in a few different implementation I looked at
  // http://en.wikipedia.org/wiki/Median_absolute_deviation
  return median_destructive(abs_devs_buffer) * 1.4826;
}

template <class T>
struct Quartiles {
  Quartiles(T quartile1, T quartile2, T quartile3)
      : q1_(quartile1), q2_(quartile2), q3_(quartile3) {}

  T q1() const { return q1_; }

  T q2() const { return q2_; }

  T q3() const { return q3_; }

  T iqr() const { return q3_ - q1_; }

private:
  T q1_;
  T q2_;
  T q3_;
};

template <class Range>
Quartiles<VELOX_RVT(Range)> quartiles(Range &&r) {
  std::vector<VELOX_RVT(Range)> temp(adl::adl_begin(r), adl::adl_end(r));
  std::sort(temp.begin(), temp.end());

  const auto q1 = percentile_of_sorted(temp, 25);
  const auto q2 = percentile_of_sorted(temp, 50);
  const auto q3 = percentile_of_sorted(temp, 75);

  return Quartiles<VELOX_RVT(Range)>(q1, q2, q3);
}
}

#endif // VELOX_STATS_H_INCLUDED
