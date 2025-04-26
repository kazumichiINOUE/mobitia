#include "LSP.h"

class Lidar2D {
private:
  ;
public:
  Lidar2D();
  std::vector<LSP> run();
  void shutdown();
};

Lidar2D::Lidar2D() {
  std::cout << "Setup Lidar2D" << std::endl;
}

std::vector<LSP> Lidar2D::run() {
  std::vector<LSP> lsps;
  return lsps;
}

void Lidar2D::shutdown() {
  std::cout << "Shutdown Lidar2D" << std::endl;
}
