#ifndef VELOX_FP_RANGE_H_INCLUDED
#define VELOX_FP_RANGE_H_INCLUDED

#include "util.h"
#include "iterator_base.h"

namespace velox {

struct FpIterator : IteratorBase<FpIterator,
                                 double,
                                 std::random_access_iterator_tag,
                                 double,
                                 Times::difference_type> {
  FpIterator(const Times::const_iterator it) : it_(it) {}

  void increment() { ++it_; }

  void decrement() { --it_; }

  void advance(const Times::difference_type n) { it_ += n; }

  bool equal(const FpIterator &rhs) const { return it_ == rhs.it_; }

  bool less(const FpIterator &rhs) const { return it_ < rhs.it_; }

  double dereference() const { return it_->count(); }

  Times::difference_type distance(const FpIterator &rhs) const { return rhs.it_ - it_; }

private:
  Times::const_iterator it_;
};

struct FpRange {
  FpRange(const Times &v) : v_(v) {}

  FpRange &operator=(const FpRange &rhs) = delete;

  FpIterator begin() const { return FpIterator(v_.begin()); }

  FpIterator end() const { return FpIterator(v_.end()); }

private:
  const Times &v_;
};
}

#endif // VELOX_FP_RANGE_H_INCLUDED
