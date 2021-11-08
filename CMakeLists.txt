cmake_minimum_required(VERSION 3.0.0)
project(agt3d VERSION 0.1.0)

if (MSVC)
  add_compile_options(/W3 /WX)
  add_definitions(-DLINK_ASSIMP -DWINDOWS)
else()
  add_definitions(-DLINK_ASSIMP)
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

if (MSVC)
  target_link_directories(agt3d PUBLIC 
    lib
  )
else()
  target_link_directories(agt3d PUBLIC 
    lib
  )
endif()

target_link_libraries(agt3d PRIVATE 
  assimp
)

target_include_directories(agt3d PUBLIC
  glad
  glm
  ./
)

target_include_directories(agt3d PRIVATE
  stb
)


target_precompile_headers(agt3d PRIVATE 
  "$<$<COMPILE_LANGUAGE:CXX>:${CMAKE_CURRENT_SOURCE_DIR}/agt_stdafx.h>"
)

set_target_properties(agt3d PROPERTIES CXX_STANDARD 20)
