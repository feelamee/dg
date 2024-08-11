cmake_minimum_required(VERSION 3.12)
project(
  orbi
  LANGUAGES CXX
  DESCRIPTION
    "orbi - simple 3d app which implement moving around orbit of objects")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COLOR_DIAGNOSTICS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(DG_ORBI_SANITIZER "enable -fsanitize option" ON)
option(DG_ORBI_PEDANTIC "enable strict compiler warnings" ON)

add_subdirectory("../engine" "${CMAKE_CURRENT_BINARY_DIR}/engine")

if(ANDROID)
  add_library(orbi SHARED)
else()
  add_executable(orbi)
endif()
target_sources(orbi PRIVATE "src/main.cpp")
target_link_libraries(orbi PRIVATE engine::engine)
target_compile_features(orbi PRIVATE cxx_std_20)

if(DG_ORBI_SANITIZER)
  include("../cmake/sanitizer.cmake")
  target_link_libraries(orbi PRIVATE sanitizer::undefined sanitizer::address)
endif()

if(DG_ORBI_PEDANTIC)
  include("../cmake/pedantic.cmake")
  target_link_libraries(orbi PRIVATE pedantic)
endif()

add_custom_target(
  copy-resources ALL
  COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/res"
          "${CMAKE_CURRENT_BINARY_DIR}/res")
