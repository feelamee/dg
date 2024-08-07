cmake_minimum_required(VERSION 3.12)
project(test LANGUAGES CXX)

include(FetchContent)

FetchContent_Declare(
  doctest
  GIT_REPOSITORY "https://github.com/doctest/doctest"
  GIT_TAG "v2.4.11")
FetchContent_MakeAvailable(doctest)

add_executable(test main.cpp)
target_compile_features(test PRIVATE cxx_std_20)
target_link_libraries(test PRIVATE engine::engine doctest::doctest)
