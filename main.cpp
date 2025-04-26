#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <memory>

#include "OrientalMotor.h"
#include "ODOMETRY.h"
#include "LSP.h"
#include "Lidar2D.h"
#include "STATUS.h"

using std::chrono::milliseconds;
using std::chrono::seconds;
using std::this_thread::sleep_for;
using LockGuard = std::lock_guard<std::mutex>;

std::mutex mtx; 
bool running = true;

void thread_motor(std::shared_ptr<STATUS> st, std::shared_ptr<ODOMETRY> odo) {
  // スレッド起動メッセージ
  {
    LockGuard lock(mtx);
    std::cout << "Motor thread started." << std::endl;
  }
  OrientalMotor om;
  while (running) {
    ODOMETRY odo = om.run();
    sleep_for(milliseconds(10));
  }
  om.shutdown();
}

void thread_lidar(std::shared_ptr<STATUS> st) {
  // スレッド起動メッセージ
  {
    LockGuard lock(mtx);
    std::cout << "LiDAR thread started." << std::endl;
  }
  Lidar2D l2d;
  while (running) {
    std::vector<LSP> lsps = l2d.run();
    sleep_for(milliseconds(500));
  }
  l2d.shutdown();
}

int main(int argc, char* argv[]) {
  // 共有オブジェクト
  auto state = std::make_shared<STATUS>();
  auto odo = std::make_shared<ODOMETRY>();

  // スレッド起動
  std::thread th_motor(thread_motor, state, odo);
  std::thread th_lidar(thread_lidar, state);

  // 終了待機
  while (true) {
    sleep_for(seconds(1));  // ダミー処理
    break;
  }
  running = false;          // threadを停止

  // スレッドの終了待機
  th_motor.join();
  th_lidar.join();

  // 終了処理
  std::cout << *state << std::endl;
  std::cout << *odo << std::endl;
  std::cout << "Bye." << std::endl;
  return 0;
}
