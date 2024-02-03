cmake_minimum_required(VERSION 3.20)
project(mypool)

set(CMAKE_CXX_STANDARD 14)

add_executable(mypool main.cpp common.h Chunk.h Chunk.cpp Pool.h Pool.cpp Test.hpp FixSizePool.hpp GC.cpp GC.h)
