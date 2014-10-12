#include "outliers.h"
#include "test_helpers.h"

using namespace velox;

TEST_CASE("outliers") {
  // quantile:
  //         0%         25%         50%         75%        100%
  //-12.8823451   0.4202205   0.8126168   1.6315957   9.8627910
  // IQR: 1.211375
  // Q3 + 3 * IQR: 5.2657207
  // Q3 + 1.5 * IQR: 3.4486582
  // Q1 - 1.5 * IQR: -1.396842
  // Q1 - 3 * IQR: -3.2139045
  const std::vector<FpNs> high_severe{FpNs(9.862791)};
  const std::vector<FpNs> high_mild{FpNs(4.3542092)};
  const std::vector<FpNs> low_mild{FpNs(-3.1220713)};
  const std::vector<FpNs> low_severe{FpNs(-12.8823451)};
  const std::vector<FpNs> normal{FpNs(0.3852786),
                                 FpNs(0.5250462),
                                 FpNs(0.7523035),
                                 FpNs(0.8729301),
                                 FpNs(1.3526604),
                                 FpNs(1.7245741)};

  std::vector<FpNs> sample;
  sample.insert(sample.end(), high_severe.begin(), high_severe.end());
  sample.insert(sample.end(), high_mild.begin(), high_mild.end());
  sample.insert(sample.end(), low_mild.begin(), low_mild.end());
  sample.insert(sample.end(), low_severe.begin(), low_severe.end());
  sample.insert(sample.end(), normal.begin(), normal.end());

  Outliers outliers(sample);

  const auto &quartiles = outliers.quartiles();
  REQUIRE(quartiles.q1().count() == Approx(0.4202205));
  REQUIRE(quartiles.q3().count() == Approx(1.6315957));

  const auto &thresholds = outliers.thresholds();
  REQUIRE(thresholds.high_severe().count() == Approx(5.2657207));
  REQUIRE(thresholds.high_mild().count() == Approx(3.4486582));
  REQUIRE(thresholds.low_mild().count() == Approx(-1.396842));
  REQUIRE(thresholds.low_severe().count() == Approx(-3.2139045));

  REQUIRE(outliers.high_severe() == high_severe);
  REQUIRE(outliers.high_mild() == high_mild);
  REQUIRE(outliers.low_mild() == low_mild);
  REQUIRE(outliers.low_severe() == low_severe);
  REQUIRE(outliers.normal() == normal);
}
