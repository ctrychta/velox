#include "util.h"
#include "test_helpers.h"

using namespace velox;

TEST_CASE("vector_with_capacity") {
  std::vector<int> v;
  REQUIRE(v.capacity() < 1000);

  auto v2 = vector_with_capacity<int>(1000);
  REQUIRE(v2.capacity() >= 1000);
}
