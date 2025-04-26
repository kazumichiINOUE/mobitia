#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

using std::chrono::milliseconds;
using std::chrono::seconds;
using std::this_thread::sleep_for;
using LockGuard = std::lock_guard<std::mutex>;

std::mutex mtx; 
bool running = true;

void thread_motor(int& shared_data) {
  std::cout << "Motor thread started." << std::endl;
  for (int i = 0; i < 10000; ++i) {
    LockGuard lock(mtx); 
    shared_data++;
  }
  while (running) {
    sleep_for(milliseconds(500));
  }
}

void thread_lidar(int& shared_data) {
  std::cout << "LiDAR thread started." << std::endl;
  for (int i = 0; i < 10000; ++i) {
    LockGuard lock(mtx);
    shared_data++;
  }
  while (running) {
    sleep_for(milliseconds(500));
  }
}

int main(int argc, char* argv[]) {
  int shared_data = 0;

  std::thread th_motor(thread_motor, std::ref(shared_data));
  std::thread th_lidar(thread_lidar, std::ref(shared_data));

  sleep_for(seconds(3));   // 3秒待ってからスレッドを停止する
  running = false;          // threadを停止

  th_motor.join();
  th_lidar.join();

  std::cout << "shared_data = " << shared_data << std::endl;

  std::cout << "Bye." << std::endl;
  return 0;
}
