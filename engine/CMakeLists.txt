cmake_minimum_required(VERSION 3.12)
project(
  engine
  LANGUAGES CXX
  DESCRIPTION
    "engine - tiny wrapper around SDL to manage video, audio, events, etc")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(DG_ENGINE_SANITIZER "enable -fsanitize option" ON)
option(DG_ENGINE_PEDANTIC "enable strict compiler warnings" ON)
option(DG_ENGINE_TEST "enable building tests for engine" OFF)

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
target_sources(
  engine
  PRIVATE "include/engine/util.hpp" "src/util.cpp" "include/engine/context.hpp"
          "src/context.cpp" "include/engine/window.hpp" "src/window.cpp")
target_compile_features(engine PRIVATE cxx_std_20)
target_include_directories(engine PUBLIC "include/")
target_link_libraries(
  engine
  PRIVATE #[[ SDL2::SDL2 ]] OpenGL::GLES3 glad::glad
  PUBLIC glm::glm SDL2::SDL2)

if(ANDROID)
  target_link_libraries(engine PRIVATE android log)
endif()

if(DG_ENGINE_SANITIZER)
  include("../cmake/sanitizer.cmake")
  target_link_libraries(engine PRIVATE sanitizer::undefined sanitizer::address)
endif()

if(DG_ENGINE_PEDANTIC)
  include("../cmake/pedantic.cmake")
  target_link_libraries(engine PRIVATE pedantic)
endif()

add_library(engine::engine ALIAS engine)

if(DG_ENGINE_TEST)
  add_subdirectory("test/")
endif()
