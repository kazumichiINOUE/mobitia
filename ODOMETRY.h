#ifndef __ODOMETRY_H__
#define __ODOMETRY_H__

#include <iostream>

struct ODOMETRY {
  double x, y, a;

  ODOMETRY() {
    x = 0.0;
    y = 0.0;
    a = 0.0;
  };
};

std::ostream& operator<<(std::ostream& os, const ODOMETRY& s) {
    os << "[ODOMETRY] " << "(x=" << s.x << ", y=" << s.y << ", a=" << s.a << ")";
    return os;
}
#endif
