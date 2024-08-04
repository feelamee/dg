cmake_minimum_required(VERSION 3.30)
project(
  engine
  LANGUAGES CXX
  DESCRIPTION
    "engine - tiny wrapper around SDL to manage video, audio, events, etc")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)

FetchContent_Declare(
  SDL2
  GIT_REPOSITORY "https://github.com/libsdl-org/SDL"
  GIT_TAG "release-2.30.6")
FetchContent_MakeAvailable(SDL2)

set(GLM_ENABLE_CXX_20
    ON
    CACHE BOOL "enable c++20 support for glm")
FetchContent_Declare(
  glm
  GIT_REPOSITORY "https://github.com/g-truc/glm"
  GIT_TAG "1.0.1")
FetchContent_MakeAvailable(glm)

add_subdirectory("deps/glad/")

find_package(OpenGL REQUIRED)

add_library(engine SHARED)
target_sources(engine PRIVATE "include/engine/engine.hpp" "src/engine.cpp")
target_compile_features(engine PRIVATE cxx_std_20)
target_include_directories(engine PUBLIC "include/")
target_link_libraries(
  engine
  PRIVATE #[[ SDL2::SDL2 ]] OpenGL::GLES3 glad::glad
  PUBLIC glm::glm SDL2::SDL2)

if(ANDROID)
  target_link_libraries(engine PRIVATE android log)
endif()

add_library(engine::engine ALIAS engine)

option(DG_ENGINE_TEST "enable building tests for engine" ON)

if(DG_ENGINE_TEST)
  add_subdirectory("test/")
endif()
