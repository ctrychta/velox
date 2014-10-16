#ifndef VELOX_HTML_REPORTER_H_INCLUDED
#define VELOX_HTML_REPORTER_H_INCLUDED

#include "reporter.h"

namespace velox {
#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wweak-vtables"
#endif
struct HtmlReporter : Reporter {
  HtmlReporter(std::ostream &os) : os_(os), num_benchmarks(0) {}

  HtmlReporter &operator=(const HtmlReporter &rhs) = delete;

  void suite_starting(const std::string &, bool) override { os_ << template_begin() << "\n"; }

  void benchmark_starting(const std::string &name) override { current_benchmark_ = name; }

  void warm_up_ended(const ItersForDurationNs &) override {
    os_ << "benchmark_" << ++num_benchmarks << " : {\n";
    os_ << "    name : '" << current_benchmark_ << "',\n";
  }

  void measurement_collection_ended(const Measurements &measurements,
                                    const Times &times,
                                    const Outliers &outliers) override {
    assert(!measurements.empty() && !times.empty() && "Measurements are required");
    assert(measurements.size() == times.size() && "Times should be derived from measurements");

    output_summary(times, outliers);
    output_kde(times);
    output_times(times, outliers);
    output_raw_measurements(measurements);
  }

  void estimate_statistics_ended(const EstimatedStatistics &statistics) override {
    auto format_estimate =
        [this](const char *const name, const Estimate<FpNs> &e) { format(name, e, format_time); };

    format_estimate("mean", statistics.mean().estimate());
    format_estimate("median", statistics.median().estimate());
    format_estimate("sd", statistics.std_dev().estimate());
    format_estimate("mad", statistics.median_abs_dev().estimate());
    format_estimate("lls", statistics.linear_least_squares().estimate());
    format("r2", statistics.r_squared().estimate(), format_r2);
    os_ << "},\n";
  }

  void suite_ended() override { os_ << template_end() << "\n"; }

private:
  template <class E, class F>
  void format(const char *const name, const E &e, F &&f) {
    os_ << "    " << name << " : {\n";

    os_ << "        lowerBound : '";
    f(os_, e.lower_bound());
    os_ << "',\n";

    os_ << "        estimate : '";
    f(os_, e.point());
    os_ << " &plusmn; ";
    f(os_, e.standard_error());
    os_ << "',\n";

    os_ << "        upperBound : '";
    f(os_, e.upper_bound());
    os_ << "'\n    },\n";
  }

  void output_summary(const Times &times, const Outliers &outliers) {
    const auto mm = std::minmax_element(times.begin(), times.end());

    os_ << "    summary : {\n";

    os_ << "        min : '";
    format_time(os_, *mm.first);
    os_ << "',\n";

    os_ << "        q1 : '";
    format_time(os_, outliers.quartiles().q1());
    os_ << "',\n";

    os_ << "        mean : '";
    format_time(os_, FpNs(mean(FpRange(times))));
    os_ << "',\n";

    os_ << "        median : '";
    format_time(os_, median(times));
    os_ << "',\n";

    os_ << "        q3 : '";
    format_time(os_, outliers.quartiles().q3());
    os_ << "',\n";

    os_ << "        max : '";
    format_time(os_, *mm.second);
    os_ << "'\n";

    os_ << "    },\n";
  }

  void output_kde(const Times &times) {
    auto points = kde(times, DEFAULT_KDE_POINTS);

    const auto max_x =
        std::max_element(points.begin(), points.end(), [](const Point &lhs, const Point &rhs) {
                           return lhs.x() < rhs.x();
                         })->x();

    const auto scaler = scaler_for_time(FpNs(max_x));
    const auto recip_scale = 1 / scaler.scale();

    for (auto &p : points) {
      p = Point(p.x() * scaler.scale(), p.y() * recip_scale);
    }

    const auto min_max_y =
        std::minmax_element(points.begin(), points.end(), [](const Point &lhs, const Point &rhs) {
          return lhs.y() < rhs.y();
        });

    const double min_y = min_max_y.first->y(), max_y = min_max_y.second->y();
    const double mu = mean(FpRange(times)) * scaler.scale();
    const double med = median(FpRange(times)) * scaler.scale();

    os_ << "    kde : {\n";
    os_ << "        units : '" << scaler.units() << "',\n";
    os_ << "        meanData : [[" << mu << ", " << min_y << "], [" << mu << ", " << max_y
        << "]],\n";
    os_ << "        medianData : [[" << med << ", " << min_y << "], [" << med << ", " << max_y
        << "]],\n";
    os_ << "        data : [";

    const char *sep = "";
    for (const auto &p : points) {
      os_ << sep << "[" << p.x() << "," << p.y() << "]";
      sep = ", ";
    }
    os_ << "]\n    },\n";
  }

  void output_times(const Times &times, const Outliers &outliers) {
    const auto max_time = *std::max_element(times.begin(), times.end());
    const auto scaler = scaler_for_time(max_time);

    auto format_outlier_line =
        [&](const char *const name, const Times &outs, const FpNs threshold) {
      os_ << "        " << name << " : [";
      if (!outs.empty()) {
        os_ << "[1, " << scaler.scale(threshold) << "], ";
        os_ << "[" << times.size() << ", " << scaler.scale(threshold) << "]";
      }
      os_ << "],\n";
    };

    os_ << "    samples : {\n";
    os_ << "        units : '" << scaler.units() << "',\n";
    format_outlier_line(
        "highSevereData", outliers.high_severe(), outliers.thresholds().high_severe());
    format_outlier_line("highMildData", outliers.high_mild(), outliers.thresholds().high_mild());
    format_outlier_line("lowMildData", outliers.low_mild(), outliers.thresholds().low_mild());
    format_outlier_line("lowSevereData", outliers.low_severe(), outliers.thresholds().low_severe());
    os_ << "        data : [";

    const char *sep = "";
    for (std::size_t i = 0; i < times.size(); ++i) {
      os_ << sep << "[" << i + 1 << "," << scaler.scale(times[i]) << "]";
      sep = ", ";
    }
    os_ << "]\n    },\n";
  }

  void output_raw_measurements(const Measurements &measurements) {
    const auto scaler = scaler_for_time(measurements.back().duration());
    const auto points = measurements_to_points(measurements);

    const auto sl = slope(points);

    os_ << "    rawMeasurements : {\n";
    os_ << "        units : '" << scaler.units() << "',\n";
    os_ << "        regression : [[0, 0], [" << measurements.back().iters() << ", "
        << scaler.scale(FpNs{static_cast<double>(measurements.back().iters()) * sl}) << "]],\n";
    os_ << "        data : [";

    const char *sep = "";
    for (const auto &m : measurements) {
      os_ << sep << "[" << m.iters() << "," << scaler.scale(m.duration()) << "]";
      sep = ", ";
    }
    os_ << "]\n    },\n";
  }

private:
  template <std::size_t N>
  static std::string string_from_cstrs(const char *const (&cstrs)[N]) {
    std::string s;
    for (auto cstr : cstrs) {
      s += cstr;
    }
    return s;
  }

  static const std::string &template_begin() {
    static const char *const pieces[] = {
#include "html_template_begin.tpl"
    };

    static const std::string s = string_from_cstrs(pieces);
    return s;
  }

  static const std::string &template_end() {
    static const char *const pieces[] = {
#include "html_template_end.tpl"
    };

    static const std::string s = string_from_cstrs(pieces);
    return s;
  }

private:
  std::ostream &os_;
  std::string current_benchmark_;
  std::uint32_t num_benchmarks;
};
#ifdef __clang__
#pragma clang diagnostic pop
#endif
}

#endif // VELOX_HTML_REPORTER_H_INCLUDED
