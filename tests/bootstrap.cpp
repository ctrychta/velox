#include "bootstrap.h"
#include "test_helpers.h"

#include <sstream>
#include <algorithm>

using namespace velox;

template <class T>
struct TestDistribution {
  TestDistribution(T a, T b) : a_(a), b_(b), cur_(a), counter_(0) {}

  template <class G>
  T operator()(G &) {
    if (++counter_ % 3 == 0)
      return a_ + (counter_ % (b_ - a_));

    return b_ - (counter_ % (b_ - a_));
  }

private:
  T a_;
  T b_;
  T cur_;
  std::uint32_t counter_;
};

TEST_CASE("resample") {
  const std::vector<FpNs> sample{FpNs{1}, FpNs{2}, FpNs{3}, FpNs{4}, FpNs{5}};
  const std::vector<FpNs> expected{FpNs{4}, FpNs{3}, FpNs{4}, FpNs{5}, FpNs{4}};

  resample<TestDistribution>(
      sample, 1, 0, [&](const std::vector<FpNs> &s) { REQUIRE(expected == s); });
}

TEST_CASE("estimate_statistics") {
  const Measurements measurements{{1, Ns{5}}, {2, Ns{50}}, {3, Ns{67}}, {4, Ns{71}}, {5, Ns{81}}};
  const Times sample{FpNs{5}, FpNs{50}, FpNs{67}, FpNs{71}, FpNs{80}, FpNs{81}};

  // 80, 71, 71, 50, 81, 50
  // 71, 67, 80, 81, 80, 67
  // 67, 50, 5, 80, 71, 71
  const auto e = estimate_statistics<TestDistribution>(measurements, sample, 3, .95);

  const auto &mean = e.mean();
  const auto &median = e.median();
  const auto &std_dev = e.std_dev();
  const auto &mad = e.median_abs_dev();
  const auto &lls = e.linear_least_squares();
  const auto &r2 = e.r_squared();

  const std::vector<FpNs> expected_mean{FpNs{67.16667}, FpNs{74.33333}, FpNs{57.33333}};
  const std::vector<FpNs> expected_median{FpNs{71}, FpNs{75.5}, FpNs{69}};
  const std::vector<FpNs> expected_std_dev{FpNs{13.96305}, FpNs{6.742897}, FpNs{27.4712}};
  const std::vector<FpNs> expected_mad{FpNs{14.0847}, FpNs{7.413}, FpNs{9.6369}};
  const std::vector<FpNs> expected_lls{FpNs{17.78048}, FpNs{19.745}, FpNs{19}};
  const std::vector<double> expected_r2{0.99048, 0.96807, 0.97133};

  REQUIRE(FpRange(expected_mean) == FpRange(mean.distribution()));
  REQUIRE(FpRange(expected_median) == FpRange(median.distribution()));
  REQUIRE(FpRange(expected_std_dev) == FpRange(std_dev.distribution()));
  REQUIRE(FpRange(expected_mad) == FpRange(mad.distribution()));
  REQUIRE(FpRange(expected_lls) == FpRange(lls.distribution()));
  REQUIRE(expected_r2.size() == r2.distribution().size());
  REQUIRE(expected_r2[0] == Approx(r2.distribution()[0]));
  REQUIRE(expected_r2[1] == Approx(r2.distribution()[1]));
  REQUIRE(expected_r2[2] == Approx(r2.distribution()[2]));

  REQUIRE(59.0 == Approx(mean.estimate().point().count()));
  REQUIRE(8.534788 == Approx(mean.estimate().standard_error().count()));
  REQUIRE(57.825 == Approx(mean.estimate().lower_bound().count()));
  REQUIRE(73.975 == Approx(mean.estimate().upper_bound().count()));
  REQUIRE(0.95 == Approx(mean.estimate().confidence_level()));

  REQUIRE(69.0 == Approx(median.estimate().point().count()));
  REQUIRE(3.32916 == Approx(median.estimate().standard_error().count()));
  REQUIRE(69.1 == Approx(median.estimate().lower_bound().count()));
  REQUIRE(75.275 == Approx(median.estimate().upper_bound().count()));
  REQUIRE(0.95 == Approx(median.estimate().confidence_level()));

  REQUIRE(28.74021 == Approx(std_dev.estimate().point().count()));
  REQUIRE(10.5219 == Approx(std_dev.estimate().standard_error().count()));
  REQUIRE(7.1039 == Approx(std_dev.estimate().lower_bound().count()));
  REQUIRE(26.79578 == Approx(std_dev.estimate().upper_bound().count()));
  REQUIRE(0.95 == Approx(std_dev.estimate().confidence_level()));

  REQUIRE(17.0499 == Approx(mad.estimate().point().count()));
  REQUIRE(3.39706 == Approx(mad.estimate().standard_error().count()));
  REQUIRE(7.52419 == Approx(mad.estimate().lower_bound().count()));
  REQUIRE(13.86231 == Approx(mad.estimate().upper_bound().count()));
  REQUIRE(0.95 == Approx(mad.estimate().confidence_level()));

  REQUIRE(18.0909 == Approx(lls.estimate().point().count()));
  REQUIRE(0.9918 == Approx(lls.estimate().standard_error().count()));
  REQUIRE(17.84146 == Approx(lls.estimate().lower_bound().count()));
  REQUIRE(19.70784 == Approx(lls.estimate().upper_bound().count()));
  REQUIRE(0.95 == Approx(lls.estimate().confidence_level()));

  REQUIRE(.96693 == Approx(r2.estimate().point()));
  REQUIRE(.0121 == Approx(r2.estimate().standard_error()));
  REQUIRE(.96823 == Approx(r2.estimate().lower_bound()));
  REQUIRE(.98952 == Approx(r2.estimate().upper_bound()));
  REQUIRE(0.95 == Approx(r2.estimate().confidence_level()));
}
