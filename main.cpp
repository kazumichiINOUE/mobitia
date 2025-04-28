#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <memory>
#include <SDL.h>
#include <signal.h>

#include "common.h"
#include "OrientalMotor.h"
#include "ODOMETRY.h"
#include "LSP.h"
#include "Lidar2D.h"
#include "STATUS.h"

void sigcatch(int);

std::atomic<bool> running(true);

std::thread th_motor;
std::thread th_lidar;

SDL_Joystick* joystick;

void thread_motor(std::shared_ptr<STATUS> st, std::shared_ptr<ODOMETRY> odo) {
  // スレッド起動メッセージ
  {
    LockGuard lock(mtx);
    std::cout << "Motor thread started." << std::endl;
  }
  try {
    OrientalMotor om;
    while (running.load()) {
      om.run(odo);
      sleep_for(milliseconds(10));
    }
    om.shutdown();
  } catch (const std::exception& e) {
    std::cerr << "Error in motor thread: " << e.what() << std::endl;
  }
}

void thread_lidar(std::shared_ptr<STATUS> st) {
  // スレッド起動メッセージ
  {
    LockGuard lock(mtx);
    std::cout << "LiDAR thread started." << std::endl;
  }
  try {
    Lidar2D l2d;
    while (running.load()) {
      std::vector<LSP> lsps = l2d.run();
      sleep_for(milliseconds(500));
    }
    l2d.shutdown();
  } catch (const std::exception& e) {
    std::cerr << "Error in lidar thread: " << e.what() << std::endl;
  }
}

int main(int argc, char* argv[]) {
  // Ctrl+c 対応
  if (SIG_ERR == signal(SIGINT, sigcatch)) {
    std::printf("failed to set signal handler\n");
    exit(0);
  }

  // Joystick setup
  std::atomic<bool> use_keyboard(false);
  if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_EVENTS) < 0) {
    std::cerr << "Failure SDL initialize. " << SDL_GetError() << std::endl;
    exit(0);
  }
  SDL_Window* window = SDL_CreateWindow("Keyboard Control", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, 0);
  joystick = SDL_JoystickOpen(0);
  if (!joystick) {
    std::cerr << "Joystick not detected. Use keyboard." << std::endl;
    use_keyboard.store(true);
  } else {
    std::cerr << "Joystick detected:" << SDL_JoystickName(joystick) << std::endl;
    std::cerr << SDL_JoystickNumAxes(joystick) << " axis" << std::endl;
    std::cerr << SDL_JoystickNumButtons(joystick) << " buttons" << std::endl << std::endl;
    std::cerr << "Joypad ready completed" << std::endl;
  }

  // 共有オブジェクト
  auto state = std::make_shared<STATUS>();
  auto odo = std::make_shared<ODOMETRY>();

  // スレッド起動
  th_motor = std::thread(thread_motor, state, odo);
  th_lidar = std::thread(thread_lidar, state);

  // 終了待機
  SDL_Event e;
  while (running) {
    while (SDL_PollEvent(&e)) {
      if (use_keyboard.load()) {
        if (e.type == SDL_KEYDOWN) {
          running.store(false);    // thread実行フラグを停止側にセット
        }
      }
    }
    sleep_for(milliseconds(100));  // 時間調整待ち時間
  }

  // スレッドの終了待機
  if (th_motor.joinable()) {
    th_motor.join();
  }
  if (th_lidar.joinable()) {
    th_lidar.join();
  }

  // 終了処理
  std::cout << *state << std::endl;
  std::cout << *odo << std::endl;
  std::cout << "[In main] Bye." << std::endl;

  SDL_DestroyWindow(window);
  SDL_JoystickClose(joystick);
  SDL_Quit();

  return 0;
}

void sigcatch(int sig) {
  std::cerr << "\033[31m" << "\nSignal " << sig << " received." << "\033[0m" << std::endl;
  running.store(false);
}
