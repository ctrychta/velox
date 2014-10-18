#include "format.h"
#include "test_helpers.h"

#include <sstream>

using namespace velox;

TEST_CASE("format_r2") {
  {
    std::stringstream ss;
    format_r2(ss, .1234);
    REQUIRE("0.1234" == ss.str());
  }

  {
    std::stringstream ss;
    format_r2(ss, .123456789);
    REQUIRE("0.1234568" == ss.str());
  }
}

TEST_CASE("format_short") {
  {
    std::stringstream ss;
    format_short(ss, 0.1234567);
    REQUIRE("0.1235" == ss.str());
  }

  {
    std::stringstream ss;
    format_short(ss, 10.1234567);
    REQUIRE("10.123" == ss.str());
  }

  {
    std::stringstream ss;
    format_short(ss, 100.1234567);
    REQUIRE("100.12" == ss.str());
  }

  {
    std::stringstream ss;
    format_short(ss, 1000.1234567);
    REQUIRE("1000.1" == ss.str());
  }
}

TEST_CASE("format_time") {
  {
    std::stringstream ss;
    format_time(ss, FpNs{.85});
    REQUIRE("850.00 ps" == ss.str());
  }

  {
    std::stringstream ss;
    format_time(ss, FpNs{256.432});
    REQUIRE("256.43 ns" == ss.str());
  }

  {
    std::stringstream ss;
    format_time(ss, FpNs{5050.937});
    REQUIRE("5.0509 us" == ss.str());
  }

  {
    std::stringstream ss;
    format_time(ss, FpNs{2534921.412});
    REQUIRE("2.5349 ms" == ss.str());
  }

  {
    std::stringstream ss;
    format_time(ss, FpNs{87678348746.2295});
    REQUIRE("87.678 s" == ss.str());
  }
}

TEST_CASE("scaler_for_time") {
  {
    const auto scaler = scaler_for_time(FpNs{.9});
    REQUIRE(1000 == Approx(scaler.scale()));
    REQUIRE("ps" == scaler.units());
    REQUIRE(450 == Approx(scaler.scale(FpNs{.45})));
  }

  {
    const auto scaler = scaler_for_time(FpNs{987.12});
    REQUIRE(1 == Approx(scaler.scale()));
    REQUIRE("ns" == scaler.units());
    REQUIRE(212.67 == Approx(scaler.scale(FpNs{212.67})));
  }

  {
    const auto scaler = scaler_for_time(FpNs{367000});
    REQUIRE(.001 == Approx(scaler.scale()));
    REQUIRE("us" == scaler.units());
    REQUIRE(.76898 == Approx(scaler.scale(FpNs{768.98})));
  }

  {
    const auto scaler = scaler_for_time(FpNs{776221667.554});
    REQUIRE(.000001 == Approx(scaler.scale()));
    REQUIRE("ms" == scaler.units());
    REQUIRE(866.5216 == Approx(scaler.scale(FpNs{866521689})));
  }

  {
    const auto scaler = scaler_for_time(FpNs{9484613689});
    REQUIRE(.000000001 == Approx(scaler.scale()));
    REQUIRE("s" == scaler.units());
    REQUIRE(9.4846 == Approx(scaler.scale(FpNs{9484613623.59})));
  }
}

TEST_CASE("js_string_escape") {
  const std::string unescaped("a'b\"c\\d");
  const std::string expected("a\\'b\\\"c\\\\d");
  REQUIRE(expected == js_string_escape(unescaped));
}
