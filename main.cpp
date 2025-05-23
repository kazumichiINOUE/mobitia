#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <memory>
#include <SDL.h>
#include <SDL_image.h>
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

/**************************************************
 * 並行スレッドの定義
 ****************************************************/
// Oriental Motor 
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

// 2D-LiDAR
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

/**************************************************
 * MAIN
 ****************************************************/
int main(int argc, char* argv[]) {
  /**************************************************
   * 初期設定
   ****************************************************/
  // Ctrl+c 対応
  if (SIG_ERR == signal(SIGINT, sigcatch)) {
    std::printf("failed to set signal handler\n");
    exit(0);
  }

  // window setup
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL_Init Error: " << SDL_GetError() << std::endl;
    return 1;
  }

  if (IMG_Init(IMG_INIT_PNG) == 0) {
    std::cerr << "IMG_Init Error: " << IMG_GetError() << std::endl;
    SDL_Quit();
    return 1;
  }

  const int WINDOW_WIDTH = 1280;
  const int WINDOW_HEIGHT = 960;
  if (SDL_Init(SDL_INIT_JOYSTICK | SDL_INIT_EVENTS) < 0) {
    std::cerr << "Failure SDL initialize. " << SDL_GetError() << std::endl;
    return 1;
  }
  SDL_Window* window = SDL_CreateWindow("Keyboard Control", 
                                        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
                                        WINDOW_WIDTH, WINDOW_HEIGHT, 
                                        SDL_WINDOW_SHOWN);
  if (!window) {
    std::cerr << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
    IMG_Quit();
    SDL_Quit();
    return 1;
  }

  SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
  if (!renderer) {
    std::cerr << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 1;
  }

  // 画像を読み込む
  SDL_Surface* backgroundSurface = IMG_Load("occMap.png");
  if (!backgroundSurface) {
    std::cerr << "IMG_Load Error: " << IMG_GetError() << std::endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 1;
  }

  SDL_Texture* backgroundTexture = SDL_CreateTextureFromSurface(renderer, backgroundSurface);
  SDL_FreeSurface(backgroundSurface);
  if (!backgroundTexture) {
    std::cerr << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 1;
  }

  // Joystick setup
  std::atomic<bool> use_keyboard(false);
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

  /**************************************************
   * main loop
   ****************************************************/
  SDL_Event event;
  while (running) {
    while (SDL_PollEvent(&event)) {
      if (use_keyboard.load()) {
        if (event.type == SDL_KEYDOWN) {
          running.store(false);    // thread実行フラグを停止側にセット
        }
      }
    }
    // 描画処理
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, backgroundTexture, nullptr, nullptr);

    // 何かの描画処理

    SDL_RenderPresent(renderer);

    //SDL_Delay(10); // 60FPSくらいにする
    sleep_for(milliseconds(100));  // 時間調整待ち時間
  }

  /**************************************************
   * 終了処理
   ****************************************************/
  // スレッドの終了待機
  if (th_motor.joinable()) {
    th_motor.join();
  }
  if (th_lidar.joinable()) {
    th_lidar.join();
  }

  // 状態表示
  std::cout << *state << std::endl;
  std::cout << *odo << std::endl;
  std::cout << "[In main] Bye." << std::endl;

  // joystickライブラリの終了
  SDL_DestroyWindow(window);
  SDL_JoystickClose(joystick);
  SDL_Quit();

  return 0;
}

/**************************************************
 * Ctrl + c 対応
 ****************************************************/
void sigcatch(int sig) {
  std::cerr << "\033[31m" << "\nSignal " << sig << " received." << "\033[0m" << std::endl;
  running.store(false);
}
