#pragma once

#define GL_SILENCE_DEPRECATION

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <string>
#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <mutex>
#include <numeric>
#include <optional>
#include <ostream>
#include <shared_mutex>
#include <sstream>
#include <thread>
#include <vector>
#include <variant>

// #if __has_include(<source_location>)
// #include <source_location>
// using std::source_location;
// #elif __has_include(<experimental/source_location>)
// #include <experimental/source_location>
// using std::experimental::source_location;
// #else
// #error "<source_location> not found"
// #endif

#ifdef WINDOWS
#include <execution>
#include <numbers>
#include <Windows.h>
#endif

#include <glad/glad.h>

#include <glm/glm.hpp>
#include <glm/gtc/random.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/compatibility.hpp>
#include <glm/gtx/intersect.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/polar_coordinates.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/vector_angle.hpp>

#ifdef _DEBUG
#define MY_ASSERT(a, msg)                                             \
  if (!(a)) {                                                         \
    std::cout << "### ASSERT ### " << std::endl                       \
              << msg << std::endl                                     \
              << __FILE__ << ", " << __FUNCTION__ << ", " << __LINE__ \
              << std::endl;                                           \
    abort();                                                          \
  }
#define MY_ALERT(a, msg)                                              \
  if (!(a)) {                                                         \
    std::cout << "### ASSERT ### " << std::endl                       \
              << msg << std::endl                                     \
              << __FILE__ << ", " << __FUNCTION__ << ", " << __LINE__ \
              << std::endl;                                           \
  }
#define MY_ABORT(msg)                                                 \
  {                                                                   \
    std::cout << "### ABORT ### " << std::endl                        \
              << msg << std::endl                                     \
              << __FILE__ << ", " << __FUNCTION__ << ", " << __LINE__ \
              << std::endl;                                           \
    abort();                                                          \
  }
#else
#define MY_ASSERT(a, msg)                                             \
  if (!(a)) {                                                         \
    std::cout << "### ASSERT ### " << std::endl                       \
              << msg << std::endl                                     \
              << __FILE__ << ", " << __FUNCTION__ << ", " << __LINE__ \
              << std::endl;                                           \
  }
#define MY_ALERT(a, msg)                                              \
  if (!(a)) {                                                         \
    std::cout << "### ASSERT ### " << std::endl                       \
              << msg << std::endl                                     \
              << __FILE__ << ", " << __FUNCTION__ << ", " << __LINE__ \
              << std::endl;                                           \
  }
#define MY_ABORT(msg)                                                 \
  {                                                                   \
    std::cout << "### ABORT ### " << std::endl                        \
              << msg << std::endl                                     \
              << __FILE__ << ", " << __FUNCTION__ << ", " << __LINE__ \
              << std::endl;                                           \
    abort();                                                          \
  }
#endif
