#ifndef VELOX_FORMAT_H_INCLUDED
#define VELOX_FORMAT_H_INCLUDED

#include "util.h"

#include <ostream>
#include <string>
#include <iomanip>

namespace velox {

struct StreamFormatRestorer {
  StreamFormatRestorer(std::ostream &os)
      : os_(os), flags_(os.flags()), precision_(os.precision()) {}

  StreamFormatRestorer &operator=(const StreamFormatRestorer &rhs) = delete;

  ~StreamFormatRestorer() {
    os_.flags(flags_);
    os_.precision(precision_);
  }

private:
  std::ostream &os_;
  std::ios_base::fmtflags flags_;
  std::streamsize precision_;
};

inline void format_r2(std::ostream &os, const double n) {
  const StreamFormatRestorer restorer(os);
  os << std::setprecision(7) << n;
}

inline void format_short(std::ostream &os, const double n) {
  const StreamFormatRestorer restorer(os);

  os.setf(std::ios_base::fixed);

  if (n < 10.0) {
    os << std::setprecision(4) << n;
  } else if (n < 100) {
    os << std::setprecision(3) << n;
  } else if (n < 1000) {
    os << std::setprecision(2) << n;
  } else {
    os << std::setprecision(1) << n;
  }
}

inline void format_time(std::ostream &os, const FpNs ns) {
  if (ns < Ns(1)) {
    format_short(os, ns.count() * 1e3);
    os << " ps";
  } else if (ns < std::chrono::microseconds(1)) {
    format_short(os, ns.count());
    os << " ns";
  } else if (ns < std::chrono::milliseconds(1)) {
    format_short(os, ns.count() / 1e3);
    os << " us";
  } else if (ns < std::chrono::seconds(1)) {
    format_short(os, ns.count() / 1e6);
    os << " ms";
  } else {
    format_short(os, ns.count() / 1e9);
    os << " s";
  }
}

struct TimeScaler {
  TimeScaler(const std::string &time_units, const double scale_factor)
      : units_(time_units), scale_(scale_factor) {}

  const std::string &units() const { return units_; }

  double scale() const { return scale_; }

  double scale(FpNs ns) const { return ns.count() * scale_; }

private:
  std::string units_;
  double scale_;
};

inline TimeScaler scaler_for_time(const FpNs ns) {
  if (ns < Ns(1)) {
    return TimeScaler("ps", 1000.);
  } else if (ns < std::chrono::microseconds(1)) {
    return TimeScaler("ns", 1.);
  } else if (ns < std::chrono::milliseconds(1)) {
    return TimeScaler("us", .001);
  } else if (ns < std::chrono::seconds(1)) {
    return TimeScaler("ms", .000001);
  } else {
    return TimeScaler("s", .000000001);
  }
}

inline std::string js_string_escape(const std::string &s) {
  std::string escaped;
  escaped.reserve(s.size());

  for (auto c : s) {
    switch (c) {
    case '\'':
      escaped += "\\'";
      break;
    case '"':
      escaped += "\\\"";
      break;
    case '\\':
      escaped += "\\\\";
      break;
    default:
      escaped += c;
      break;
    }
  }

  return escaped;
}
}

#endif // VELOX_FORMAT_H_INCLUDED
