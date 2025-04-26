#ifndef __LSP_H__
#define __LSP_H__
struct LSP {
  long d;
  double r;
  double th;
  double x;
  double y;

  LSP() {;};
  LSP(long d, double r, double th, double cs, double sn) {
    this->d  = d;       // [mm]
    this->r  = r;       // [m]
    this->th = th;      // [rad]
    this->x  = r * cs;  // [m]
    this->y  = r * sn;  // [m]
  };
  LSP(const LSP &lp) {
    d  = lp.d;
    r  = lp.r;
    th = lp.th;
    x  = lp.x;
    y  = lp.y;
  };
};
#endif
