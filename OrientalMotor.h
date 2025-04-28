#include "common.h"
#include "ODOMETRY.h"

class OrientalMotor {
private:
  ;
public:
  OrientalMotor();
  void run(std::shared_ptr<ODOMETRY> &odo);
  void shutdown();
};

OrientalMotor::OrientalMotor() {
  std::cout << "\033[32m" << "Setup Oriental Motors" << "\033[0m" << std::endl;
}

void OrientalMotor::run(std::shared_ptr<ODOMETRY> &odo) {
  ;
}

void OrientalMotor::shutdown() {
  std::cout << "\033[32m" << "Shutdown Oriental Motors" << "\033[0m" << std::endl;
}
