cmake_minimum_required(VERSION 3.21)
project(mypool)

set(CMAKE_CXX_STANDARD 14)

add_executable(mypool main.cpp Pool.h common.h Chunk.h Pool.cpp Test.hpp Chunk.cpp FixSizePool.cpp FixSizePool.h)
