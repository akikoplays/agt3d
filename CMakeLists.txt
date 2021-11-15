cmake_minimum_required(VERSION 3.0.0)
project(agt3d VERSION 0.1.0)
include(FetchContent)

FetchContent_Declare(
  assimp
  GIT_REPOSITORY https://github.com/assimp/assimp.git
)
set(ASSIMP_BUILD_TESTS OFF CACHE INTERNAL "Build Assimp tests")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE INTERNAL "Build Assimp tools")
set(BUILD_SHARED_LIBS OFF CACHE INTERNAL "Build shared libs")
FetchContent_MakeAvailable(assimp)

if (MSVC)
  add_compile_options(/W3 /WX)
  add_definitions(-DWINDOWS)
else()
  add_compile_options(-W -Wall -pedantic 
    -Wno-unused-variable -Wno-deprecated-volatile -Wno-reorder-ctor -Wno-unused-parameter)
endif()

file (GLOB_RECURSE GLAD_SRCS
  glad/*.h
  glad/*.c
  glad/*.cpp
)

file (GLOB_RECURSE STB_SRCS
  stb/*.h
  stb/*.c
  stb/*.cpp
)

file (GLOB SRCS 
  *.h
  *.c
  *.cpp
)

add_library(agt3d
  ${GLAD_SRCS}
  ${STB_SRCS}
  ${SRCS}
)

target_link_libraries(agt3d PRIVATE 
  assimp
)

target_include_directories(agt3d PUBLIC
  glad
  glm
  ./
  assimp
)

target_include_directories(agt3d PRIVATE
  stb
)

message(STATUS "ASSIMP include dir: ${assimp_SOURCE_DIR}")
message(STATUS "ASSIMP build dir: ${assimp_BINARY_DIR}")

target_include_directories(agt3d PUBLIC
  ${assimp_SOURCE_DIR}/include
  ${assimp_BINARY_DIR}/include
)

target_precompile_headers(agt3d PRIVATE 
  "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/agt_stdafx.h>"
)

set_target_properties(agt3d PROPERTIES CXX_STANDARD 20)
