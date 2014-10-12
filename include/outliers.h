#ifndef VELOX_OUTLIERS_H_INCLUDED
#define VELOX_OUTLIERS_H_INCLUDED

#include "util.h"
#include "stats.h"

#include <vector>

namespace velox {

struct Thresholds {
  Thresholds(const Quartiles<FpNs> &qs)
      : high_severe_(qs.q3() + 3.0 * qs.iqr()), high_mild_(qs.q3() + 1.5 * qs.iqr()),
        low_mild_(qs.q1() - 1.5 * qs.iqr()), low_severe_(qs.q1() - 3.0 * qs.iqr()) {}

  FpNs high_severe() const { return high_severe_; }

  FpNs high_mild() const { return high_mild_; }

  FpNs low_mild() const { return low_mild_; }

  FpNs low_severe() const { return low_severe_; }

private:
  FpNs high_severe_;
  FpNs high_mild_;
  FpNs low_mild_;
  FpNs low_severe_;
};

struct Outliers {
  Outliers(const Times &times) : quartiles_(::velox::quartiles(times)), thresholds_(quartiles_) {
    for (const auto &t : times) {
      if (t < thresholds_.low_severe()) {
        low_severe_.push_back(t);
      } else if (t < thresholds_.low_mild()) {
        low_mild_.push_back(t);
      } else if (t > thresholds_.high_severe()) {
        high_severe_.push_back(t);
      } else if (t > thresholds_.high_mild()) {
        high_mild_.push_back(t);
      } else {
        normal_.push_back(t);
      }
    }
  }

  const Times &high_severe() const { return high_severe_; }

  const Times &high_mild() const { return high_mild_; }

  const Times &low_mild() const { return low_mild_; }

  const Times &low_severe() const { return low_severe_; }

  const Times &normal() const { return normal_; }

  const Quartiles<FpNs> &quartiles() const { return quartiles_; }

  const Thresholds &thresholds() const { return thresholds_; }

private:
  Times high_severe_;
  Times high_mild_;
  Times low_mild_;
  Times low_severe_;
  Times normal_;
  Quartiles<FpNs> quartiles_;
  Thresholds thresholds_;
};
}

#endif // VELOX_OUTLIERS_H_INCLUDED
