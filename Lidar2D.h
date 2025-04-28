#include "common.h"
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
  std::cout << "\033[32m" << "Setup Lidar2D" << "\033[0m" << std::endl;
}

std::vector<LSP> Lidar2D::run() {
  std::vector<LSP> lsps;
  return lsps;
}

void Lidar2D::shutdown() {
  std::cout << "\033[32m" << "Shutdown Lidar2D" << "\033[0m" << std::endl;
}
