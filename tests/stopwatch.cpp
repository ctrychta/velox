#include "stopwatch.h"
#include "test_helpers.h"

using namespace velox;

TEST_CASE("stopwatch implicit measure") {
  detail::StopwatchModel<AdjustableClock> sm(1);
  Stopwatch sw(sm, [] { AdjustableClock::add_ticks(10); });

  REQUIRE(sm.iters() == 1);
  REQUIRE(sm.elapsed().count() == 10);
}

TEST_CASE("stopwatch explicit measure") {
  detail::StopwatchModel<AdjustableClock> sm(10);
  Stopwatch sw(sm, [](Stopwatch &s) { s.measure([] { AdjustableClock::add_ticks(5); }); });

  REQUIRE(sm.iters() == 10);
  REQUIRE(sm.elapsed().count() == 50);
}
