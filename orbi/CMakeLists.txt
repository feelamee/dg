cmake_minimum_required(VERSION 3.30)
project(
  orbi
  LANGUAGES CXX
  DESCRIPTION
    "orbi - simple 3d app which implement moving around orbit of objects")

add_subdirectory("../engine" "${CMAKE_CURRENT_BINARY_DIR}/engine")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(ANDROID)
  message(FATAL_ERROR "TODO")
else()
  add_executable(orbi)
  target_sources(orbi PRIVATE "src/main.cpp")
  target_link_libraries(orbi PRIVATE engine::engine)
  target_compile_features(orbi PRIVATE cxx_std_20)
endif()
