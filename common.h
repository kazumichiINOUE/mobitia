#ifndef __COMMON_H__
#define __COMMON_H__

#include <chrono>
#include <thread>
#include <mutex>

using std::chrono::milliseconds;
using std::chrono::seconds;
using std::this_thread::sleep_for;
using LockGuard = std::lock_guard<std::mutex>;

extern std::mutex mtx; 

#endif
