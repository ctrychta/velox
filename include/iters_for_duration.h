#ifndef VELOX_ITERS_FOR_DURATION_H_INCLUDED
#define VELOX_ITERS_FOR_DURATION_H_INCLUDED

#include "util.h"

namespace velox {

template <class D>
struct ItersForDuration {
  ItersForDuration(const std::uint64_t iterations, const D d) : iters_(iterations), duration_(d) {}

  std::uint64_t iters() const { return iters_; }

  D duration() const { return duration_; }

private:
  std::uint64_t iters_;
  D duration_;
};

using ItersForDurationNs = ItersForDuration<Ns>;
}

#endif // VELOX_ITERS_FOR_DURATION_H_INCLUDED
