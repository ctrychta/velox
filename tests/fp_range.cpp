#include "fp_range.h"
#include "test_helpers.h"

using namespace velox;

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wfloat-equal"
#elif defined __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wfloat-equal"
#endif

TEST_CASE("FpRange") {
  SECTION("empty") {
    std::vector<FpNs> v;
    FpRange r(v);
    auto b = r.begin(), e = r.end();
    REQUIRE(b == e);
    REQUIRE_FALSE(b != e);
    REQUIRE_FALSE(b < e);
    REQUIRE(b <= e);
    REQUIRE_FALSE(b > e);
    REQUIRE(b >= e);
  }

  SECTION("non empty") {
    std::vector<FpNs> v{FpNs{1.0}};
    FpRange r(v);
    auto b = r.begin(), e = r.end();
    REQUIRE_FALSE(b == e);
    REQUIRE(b != e);
    REQUIRE(b < e);
    REQUIRE(b <= e);
    REQUIRE_FALSE(b > e);
    REQUIRE_FALSE(b >= e);
  }

  SECTION("iteration") {
    std::vector<FpNs> v{FpNs{1.0}, FpNs{2.0}, FpNs{3.0}};
    FpRange r(v);

    REQUIRE(*r.begin()++ == 1.0);
    REQUIRE(*++r.begin() == 2.0);

    auto it = r.begin();
    REQUIRE(*it == 1.0);

    const auto diff = r.end() - it;
    REQUIRE(diff == 3);

    ++it;
    REQUIRE(*--it == 1.0);
    ++it;
    REQUIRE(*--it == 1.0);

    it += 2;
    REQUIRE(*it == 3.0);

    it -= 1;
    REQUIRE(*it == 2.0);

    REQUIRE(it[1] == 3.0);

    auto first = it - 1;
    REQUIRE(*first == 1.0);

    auto last = it + 1;
    REQUIRE(*last == 3.0);
    REQUIRE(++last == r.end());

    auto end = 2 + it;
    REQUIRE(end == r.end());
  }
}

#ifdef __clang__
#pragma clang diagnostic pop
#elif defined __GNUC__
#pragma GCC diagnostic pop
#endif
