cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
set (CMAKE_CXX_FLAGS "-O3 -g3 -std=c++17")
set (CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "")

project(mobitia)

set (BIN_DIR ${PROJECT_SOURCE_DIR}/bin) # 実行ファイルの保存場所

#### 実行ファイルの作成
set(EXECUTABLE_OUTPUT_PATH ${BIN_DIR})   # 実行ファイルの保存場所
add_executable (main main.cpp)

