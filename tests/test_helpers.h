#ifndef TEST_HELPERS_H_INCLUDED
#define TEST_HELPERS_H_INCLUDED

#include "util.h"
#include "fp_range.h"
#include "point.h"
#include "catch_without_warnings.h"

namespace Catch {
template <>
inline std::string toString<velox::FpNs>(const velox::FpNs &ns) {
  std::ostringstream oss;
  oss << "FpNs(" << ns.count() << ")";
  return oss.str();
}
}

namespace velox {
inline bool operator==(const FpRange &lhs, const FpRange &rhs) {
  // TODO: replace with C++14 std::equal
  const auto lhs_size = std::distance(lhs.begin(), lhs.end());
  const auto rhs_size = std::distance(rhs.begin(), rhs.end());
  if (lhs_size != rhs_size)
    return false;

  auto itLhs = lhs.begin(), itRhs = rhs.begin();
  for (; itLhs != lhs.end(); ++itLhs, ++itRhs) {
    if (*itLhs != Approx(*itRhs))
      return false;
  }

  return true;
}

inline std::ostream &operator<<(std::ostream &os, const FpRange &rhs) {
  const auto p = os.precision();
  os.precision(8);

  os << "{ ";
  const char *sep = "";

  for (const auto &d : rhs) {
    os << sep << d;
    sep = ", ";
  }

  os << " }";

  os.precision(p);
  return os;
}

inline std::ostream &operator<<(std::ostream &os, const Point &rhs) {
  os << "(" << rhs.x() << ", " << rhs.y() << ")";
  return os;
}

inline bool operator==(const Point &lhs, const Point &rhs) {
  return lhs.x() == Approx(rhs.x()).epsilon(.001) && lhs.y() == Approx(rhs.y()).epsilon(.001);
}
}

struct AdjustableClock {
  using duration = std::chrono::nanoseconds;
  using time_point = std::chrono::time_point<AdjustableClock, duration>;
  using rep = duration::rep;
  using period = duration::period;
  static const bool is_steady = true;

  static time_point now() { return time_point(duration(current_tick())); }

  static void add_ticks(std::uint32_t ticks) { current_tick() += ticks; }

private:
  static rep &current_tick() {
    static rep tick = 0;
    return tick;
  }
};

#endif
