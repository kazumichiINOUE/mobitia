#ifndef __STATUS_H__
#define __STATUS_H__

#include <iostream> 

struct STATUS {
  double x, y, a;
  double cmd_v, cmd_w;

  STATUS() {
    x = 0.0;
    y = 0.0;
    a = 0.0;
    cmd_v = 0.0;
    cmd_w = 0.0;
  };
};

std::ostream& operator<<(std::ostream& os, const STATUS& s) {
    os << "[STATUS] " << "(x=" << s.x << ", y=" << s.y << ", a=" << s.a
       << ", cmd_v=" << s.cmd_v << ", cmd_w=" << s.cmd_w << ")";
    return os;
}

#endif
