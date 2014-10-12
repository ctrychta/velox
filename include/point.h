#ifndef VELOX_POINT_H_INCLUDED
#define VELOX_POINT_H_INCLUDED

#include <vector>

namespace velox {
struct Point {
  Point(const double x_coord, const double y_coord) : x_(x_coord), y_(y_coord) {}

  double x() const { return x_; }

  double y() const { return y_; }

private:
  double x_;
  double y_;
};

using Points = std::vector<Point>;
}

#endif // VELOX_POINT_H_INCLUDED
