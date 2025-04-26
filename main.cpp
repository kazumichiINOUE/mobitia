#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <memory>

using std::chrono::milliseconds;
using std::chrono::seconds;
using std::this_thread::sleep_for;
using LockGuard = std::lock_guard<std::mutex>;

std::mutex mtx; 
bool running = true;

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

struct ODOMETRY {
  double x, y, a;

  ODOMETRY() {
    x = 0.0;
    y = 0.0;
    a = 0.0;
  };
};

void thread_motor(std::shared_ptr<STATUS> st, std::shared_ptr<ODOMETRY> odo) {
  // スレッド起動メッセージ
  {
    LockGuard lock(mtx);
    std::cout << "Motor thread started." << std::endl;
  }
  // 何かしらの処理
  for (int i = 0; i < 10000; ++i) {
    LockGuard lock(mtx); 
    st->x += 1.0;
  }
  // 待機
  while (running) {
    sleep_for(milliseconds(500));
  }
}

void thread_lidar(std::shared_ptr<STATUS> st) {
  // スレッド起動メッセージ
  {
    LockGuard lock(mtx);
    std::cout << "LiDAR thread started." << std::endl;
  }
  // 何かしらの処理
  for (int i = 0; i < 10000; ++i) {
    LockGuard lock(mtx);
    st->x += 1.0;
  }
  // 待機
  while (running) {
    sleep_for(milliseconds(500));
  }
}

int main(int argc, char* argv[]) {
  // 共有オブジェクト
  auto state = std::make_shared<STATUS>();
  auto odo = std::make_shared<ODOMETRY>();

  // スレッド起動
  std::thread th_motor(thread_motor, state, odo);
  std::thread th_lidar(thread_lidar, state);

  sleep_for(seconds(1));    // 1秒待ってからスレッドを停止する
  running = false;          // threadを停止

  // スレッドの終了待機
  th_motor.join();
  th_lidar.join();

  // 終了処理
  std::cout << "shared_data = " << state->x << std::endl;
  std::cout << "Bye." << std::endl;
  return 0;
}
