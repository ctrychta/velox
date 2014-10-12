#ifndef VELOX_ITERATOR_BASE_H_INCLUDED
#define VELOX_ITERATOR_BASE_H_INCLUDED

#include <cstddef>
#include <iterator>

namespace velox {

// Simple implementation for now
// Some changes, particularly to dereferencing, would be needed to be generally
// useful
template <class Derived,
          class Value,
          class Category,
          class Reference = Value &,
          class Difference = std::ptrdiff_t>
struct IteratorBase {
  using value_type = Value;
  using reference = Reference;
  using pointer = Value *;
  using difference_type = Difference;
  using iterator_category = Category;

  reference operator*() const { return self()->dereference(); }

  pointer operator->() const { return &self()->dereference(); }

  Derived &operator++() {
    self()->increment();
    return *self();
  }

  Derived operator++(int) {
    Derived temp(*self());
    self()->increment();
    return temp;
  }

  Derived &operator--() {
    self()->decrement();
    return *self();
  }

  Derived operator--(int) {
    Derived temp(*self());
    self()->decrement();
    return temp;
  }

  Derived &operator+=(difference_type n) {
    self()->advance(n);
    return *self();
  }

  Derived &operator-=(difference_type n) {
    self()->advance(-n);
    return *self();
  }

  reference operator[](difference_type n) { return *(*self() + n); }

  friend Derived operator+(Derived lhs, difference_type n) {
    lhs.advance(n);
    return lhs;
  }

  friend Derived operator+(difference_type n, Derived rhs) {
    rhs.advance(n);
    return rhs;
  }

  friend Derived operator-(Derived lhs, difference_type n) {
    lhs.advance(-n);
    return lhs;
  }

  friend difference_type operator-(const Derived &lhs, const Derived &rhs) {
    return rhs.distance(lhs);
  }

  friend bool operator==(const Derived &lhs, const Derived &rhs) { return lhs.equal(rhs); }

  friend bool operator!=(const Derived &lhs, const Derived &rhs) { return !lhs.equal(rhs); }

  friend bool operator<(const Derived &lhs, const Derived &rhs) { return lhs.less(rhs); }

  friend bool operator>(const Derived &lhs, const Derived &rhs) { return rhs.less(lhs); }

  friend bool operator<=(const Derived &lhs, const Derived &rhs) { return !(rhs.less(lhs)); }

  friend bool operator>=(const Derived &lhs, const Derived &rhs) { return !(lhs.less(rhs)); }

private:
  Derived *self() { return static_cast<Derived *>(this); }
  const Derived *self() const { return static_cast<const Derived *>(this); }
};
}

#endif // VELOX_ITERATOR_BASE_H_INCLUDED
