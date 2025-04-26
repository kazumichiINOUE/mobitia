#include <iostream>
#include <thread>
#include <mutex>

std::mutex mtx; 

void thread_motor(int& shared_data) {
  std::cout << "Motor thread started." << std::endl;
  for (int i = 0; i < 10000; ++i) {
    std::lock_guard<std::mutex> lock(mtx); 
    shared_data++;
  }
}

void thread_lidar(int& shared_data) {
  std::cout << "LiDAR thread started." << std::endl;
  for (int i = 0; i < 10000; ++i) {
    std::lock_guard<std::mutex> lock(mtx);
    shared_data++;
  }
}

int main(int argc, char* argv[]) {
  int shared_data = 0;

  std::thread th_motor(thread_motor, std::ref(shared_data));
  std::thread th_lidar(thread_lidar, std::ref(shared_data));

  th_motor.join();
  th_lidar.join();

  std::cout << "shared_data = " << shared_data << std::endl;

  std::cout << "Bye." << std::endl;
  return 0;
}
