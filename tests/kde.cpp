#include "kde.h"
#include "test_helpers.h"

using namespace velox;

TEST_CASE("standard normal probability density function") {
  REQUIRE(0.0001338 == Approx(snpdf(4)));
  REQUIRE(0.3959359 == Approx(snpdf(.123)));
  REQUIRE(0.3985736 == Approx(snpdf(-.043)));
  REQUIRE(0.03543559 == Approx(snpdf(2.2005)));
}

TEST_CASE("linspace none") {
  std::vector<double> expected;
  std::vector<double> result;

  linspace(1.0, 100.0, 0, [&](double d) { result.push_back(d); });

  REQUIRE(expected == result);
}

TEST_CASE("linspace one") {

  std::vector<double> expected{50};
  std::vector<double> result;

  linspace(50.0, 100.0, 1, [&](double d) { result.push_back(d); });

  REQUIRE(expected == result);
}

TEST_CASE("linspace many") {
  std::vector<double> expected{1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  std::vector<double> result;

  linspace(1.0, 10.0, 10, [&](double d) { result.push_back(d); });

  REQUIRE(expected == result);
}

TEST_CASE("bandwidth") {
  std::vector<FpNs> sample{FpNs{1}, FpNs{4}};
  REQUIRE(1.032967 == Approx(bandwidth_scott(sample)));

  sample = {FpNs{50}, FpNs{500}, FpNs{1500}, FpNs{.5}};
  REQUIRE(427.0687 == Approx(bandwidth_scott(sample)));

  sample = {FpNs{.5}, FpNs{.75}, FpNs{.8}, FpNs{.9}};
  REQUIRE(0.08243123 == Approx(bandwidth_scott(sample)));

  sample = {FpNs{1}, FpNs{1}, FpNs{1}};
  REQUIRE(0.0 == Approx(bandwidth_scott(sample)));

  sample = {FpNs{10}, FpNs{11}, FpNs{15}, FpNs{16}, FpNs{17}};
  REQUIRE(2.392752 == Approx(bandwidth_scott(sample)));
}

TEST_CASE("kde") {
  const std::vector<FpNs> sample{FpNs{1}, FpNs{2}, FpNs{3}, FpNs{4}, FpNs{5}};
  const auto points = kde(sample, 8);

  // R: density(c(1, 2, 3, 4, 5), bw="nrd", n=8)
  const std::vector<Point> expected{{-2.43999, .0008212},
                                    {-.88571, .02124},
                                    {.66857, .11217},
                                    {2.2228, .18763},
                                    {3.7771, .187766},
                                    {5.3314, .11217},
                                    {6.8857, .02124},
                                    {8.4399, .00082}};
  REQUIRE(expected == points);
}
