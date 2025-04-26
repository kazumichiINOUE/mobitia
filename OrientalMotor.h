#include "ODOMETRY.h"

class OrientalMotor {
private:
  ;
public:
  OrientalMotor();
  ODOMETRY run();
  void shutdown();
};

OrientalMotor::OrientalMotor() {
  std::cout << "Setup oriental motors" << std::endl;
}

ODOMETRY OrientalMotor::run() {
  ODOMETRY odo;
  return odo;
}

void OrientalMotor::shutdown() {
  std::cout << "Shutdown oriental motors" << std::endl;
}
