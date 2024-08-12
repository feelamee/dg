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
  SDL3
  GIT_REPOSITORY "https://github.com/libsdl-org/SDL"
  GIT_TAG "9493e6974f70a906950c9d8196c9a9982300f261")
FetchContent_MakeAvailable(SDL3)

set(GLM_ENABLE_CXX_20
    ON
    CACHE BOOL "enable c++20 support for glm")
FetchContent_Declare(
  glm
  GIT_REPOSITORY "https://github.com/g-truc/glm"
  GIT_TAG "1.0.1")
FetchContent_MakeAvailable(glm)

FetchContent_Declare(
  tinygltf
  GIT_REPOSITORY "https://github.com/syoyo/tinygltf"
  GIT_TAG "v2.9.3")
FetchContent_MakeAvailable(tinygltf)

add_subdirectory("deps/glad/")

add_library(engine SHARED)
target_sources(
  engine
  PRIVATE "include/engine/util.hpp"
          "src/util.cpp"
          "include/engine/context.hpp"
          "src/context.cpp"
          "include/engine/shader_program.hpp"
          "src/shader_program.cpp"
          "src/error.cpp"
          "include/engine/error.hpp"
          "include/engine/bindable.hpp"
          "include/engine/bind_guard.hpp"
          "src/bind_guard.cpp"
          "include/engine/mesh.hpp"
          "src/mesh.cpp"
          "include/engine/mesh_loader.hpp"
          "src/mesh_loader.cpp"
          "include/engine/vertex_array.hpp"
          "src/vertex_array.cpp")
target_compile_features(engine PRIVATE cxx_std_20)
target_include_directories(engine PUBLIC "include/")

if(ANDROID)
  target_link_libraries(engine PRIVATE GLESv3 EGL android log)
else()
  find_package(OpenGL REQUIRED)
  target_link_libraries(engine PRIVATE OpenGL::GLES3)
endif()

target_link_libraries(
  engine
  PRIVATE #[[ SDL3::SDL3 ]] #[[ glad::glad ]] tinygltf
  PUBLIC glm::glm SDL3::SDL3 glad::glad)

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
