#include <algorithm>
#include <iostream>
#include <limits>
#include <stack>
#include <vector>

#include "agt_object_instance.h"
#include "agt_stdafx.h"
#include "agt_utils.h"

namespace agt3d
{
void checkOpenGLErrors() noexcept
{
  const GLenum glStatus = glGetError();
  if (glStatus != GL_NO_ERROR) {
    std::stringstream ss;
    ss << "OpenGL error: " << static_cast<int>(glStatus);
    std::cout << ss.str() << "\n";
  }
}

std::vector<std::string> getFilesInFolder(const std::string& path) noexcept
{
  std::vector<std::string> list;
  const std::filesystem::path dir{path};
  for (auto const& file : std::filesystem::directory_iterator{dir}) {
    list.push_back(file.path().string());
  }
  return list;
}

glm::vec3 viewToWorldSpaceCoordTransform(float mouse_x, float mouse_y,
                                         float windowWidth, float windowHeight,
                                         glm::mat4 viewProjection) noexcept
{
  float x = 2.0f * mouse_x / windowWidth - 1.0f;
  float y = 2.0f * mouse_y / windowHeight - 1.0f;

  // Homogeneous space
  glm::vec4 screenPos = glm::vec4(x, -y, -1.0f, 1.0f);

  // Projection / eye space
  glm::mat4 invViewProjection = glm::inverse(viewProjection);
  glm::vec4 worldPos = invViewProjection * screenPos;
  return glm::vec3(worldPos);
}

glm::vec3 raycast(float mx, float my, float windowWidth, float windowHeight,
                  const glm::mat4& view, const glm::mat4& proj) noexcept
{
  // these positions must be in range [-1, 1] (!!!), not [0, width] and [0,
  // height]
  float mouseX = mx / (windowWidth * 0.5f) - 1.0f;
  float mouseY = my / (windowHeight * 0.5f) - 1.0f;

  glm::mat4 invVP = glm::inverse(proj * view);
  glm::vec4 screenPos = glm::vec4(mouseX, -mouseY, 1.0f, 1.0f);
  glm::vec4 worldPos = invVP * screenPos;

  glm::vec3 dir = glm::normalize(glm::vec3(worldPos));
  return dir;
}

void dumpToFile(std::string filename, uint8_t* data, size_t len) noexcept
{
  std::ofstream fp;
  fp.open(filename, std::ios::out | std::ios::binary);
  fp.write((char*)data, len);
  fp.close();
}

void linearRegression(std::vector<glm::vec2> points, float& b,
                      float& m) noexcept
{
  // mean x and y vals
  float meanx = 0.0f;
  float meany = 0.0f;
  for (auto& p : points) {
    meanx += p.x;
    meany += p.y;
  }
  meanx = meanx / points.size();
  meany = meany / points.size();

  // slope
  auto sum1 = 0.0f;
  auto sum2 = 0.0f;
  for (auto& p : points) {
    auto x = p.x - meanx;
    auto y = p.y - meany;
    sum1 += x * y;
    sum2 += powf(p.x - meanx, 2);
  }
  m = sum1 / sum2;

  // y intercept
  b = meany - m * meanx;
}

glm::vec3 get_arcball_vector(int x, int y, int screen_width,
                             int screen_height) noexcept
{
  glm::vec3 P = glm::vec3(1.0 * x / screen_width * 2 - 1.0,
                          1.0 * y / screen_height * 2 - 1.0, 0);
  P.y = -P.y;
  float OP_squared = P.x * P.x + P.y * P.y;
  if (OP_squared <= 1 * 1)
    P.z = sqrt(1 * 1 - OP_squared);  // Pythagoras
  else
    P = glm::normalize(P);  // nearest point
  return P;
}

glm::vec3 convertToSpherical(glm::vec3& vec) noexcept
{
  float r = glm::length(vec);
  float theta = atan2(vec.y, vec.x);
  float phi = acos(vec.z / r);
  return glm::vec3(r, theta, phi);
}

glm::vec3 convertToCartesian(glm::vec3 sp) noexcept
{
  glm::vec3 vec;
  vec.x = sp.x * cos(sp.y) * cos(sp.z);
  vec.y = sp.x * sin(sp.y) * cos(sp.z);
  vec.z = sp.x * sin(sp.z);
  return vec;
}

uint64_t getSystemTimeUsec() noexcept
{
  auto now = std::chrono::system_clock::now();
  auto duration = now.time_since_epoch();
  uint64_t usec =
    std::chrono::duration_cast<std::chrono::microseconds>(duration).count();
  return usec;
}

uint64_t getSystemTimeMsec() noexcept
{
  auto now = std::chrono::system_clock::now();
  auto duration = now.time_since_epoch();
  uint64_t msec =
    std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
  return msec;
}

#pragma warning(disable : 4996)
std::string time_in_HH_MM_SS_MMM(const char* format) noexcept
{
  using namespace std::chrono;

  // get current time
  auto now = system_clock::now();

  // get number of milliseconds for the current second
  // (remainder after division into seconds)
  auto ms = duration_cast<milliseconds>(now.time_since_epoch()) % 1000;

  // convert to std::time_t in order to convert to std::tm (broken time)
  auto timer = system_clock::to_time_t(now);

  // convert to broken time
  std::tm bt = *std::localtime(&timer);

  std::ostringstream oss;

  oss << std::put_time(&bt, format);  // HH:MM:SS
  oss << '.' << std::setfill('0') << std::setw(3) << ms.count();

  return oss.str();
}

std::string getFilenameWithExtension(std::string fullpath) noexcept
{
  const size_t last_slash_idx = fullpath.find_last_of("\\/");
  if (std::string::npos != last_slash_idx) {
    fullpath.erase(0, last_slash_idx + 1);
  }
  return fullpath;
}

std::string getPath(std::string fullpath) noexcept
{
  const size_t last_slash_idx = fullpath.find_last_of("\\/");
  if (std::string::npos != last_slash_idx) {
    fullpath.erase(last_slash_idx + 1, fullpath.length() - last_slash_idx);
  }
  return fullpath;
}

agt3d::ObjectInstance* oiFactory(std::string&& name) noexcept
{
  // Create the complete tree for an OI: mesh, object, material, object
  // instance, and connect them
  auto mesh = std::shared_ptr<agt3d::Mesh>(new agt3d::Mesh());
  auto mat = std::shared_ptr<agt3d::Material>(new agt3d::Material());
  auto obj = std::shared_ptr<agt3d::Object>(new agt3d::Object(mesh, mat, name));
  auto oi = new agt3d::ObjectInstance(name);
  oi->setObject(obj);
  return oi;
}

agt3d::ObjectInstance* oiFactoryPlaneXZ(std::string&& name) noexcept
{
  auto oi = oiFactory(std::move(name));

  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> uvs;
  std::vector<GLuint> indices;

  vertices.push_back({-0.5f, 0.0f, -0.5f});
  vertices.push_back({0.5f, 0.0f, 0.5f});
  vertices.push_back({-0.5f, 0.0f, 0.5f});
  vertices.push_back({0.5f, 0.0f, -0.5f});

  uvs.push_back({0, 1});
  uvs.push_back({1, 0});
  uvs.push_back({0, 0});
  uvs.push_back({1, 1});

  normals.push_back({0, 1, 0});
  normals.push_back({0, 1, 0});
  normals.push_back({0, 1, 0});
  normals.push_back({0, 1, 0});

  indices.resize(6);
  indices.push_back(0);
  indices.push_back(1);
  indices.push_back(2);
  indices.push_back(0);
  indices.push_back(3);
  indices.push_back(1);

  oi->getObject()->getMesh()->setDataBuffer(
    agt3d::DataStream::VERTEX, (float*)vertices.data(),
    vertices.size() * sizeof(glm::vec3), GL_FLOAT, 3);
  oi->getObject()->getMesh()->setDataBuffer(
    agt3d::DataStream::TEXCOORD0, (float*)uvs.data(),
    uvs.size() * sizeof(glm::vec2), GL_FLOAT, 2);
  oi->getObject()->getMesh()->setDataBuffer(
    agt3d::DataStream::NORMAL, (float*)normals.data(),
    normals.size() * sizeof(glm::vec3), GL_FLOAT, 3);
  oi->getObject()->getMesh()->setIndices(indices);

  oi->getObject()->getMesh()->updateVAO();

  return oi;
}

agt3d::ObjectInstance* oiFactoryOverlay(std::string&& name) noexcept
{
  auto oi = oiFactory(std::move(name));

  std::vector<glm::vec3> vertices;
  std::vector<glm::vec2> uvs;
  std::vector<GLuint> indices;

  vertices.push_back({0, 0, 0});
  vertices.push_back({1, 0, 0});
  vertices.push_back({0, 1, 0});
  vertices.push_back({1, 1, 0});

  uvs.push_back({0, 0});
  uvs.push_back({1, 0});
  uvs.push_back({0, 1});
  uvs.push_back({1, 1});

  indices = {0, 1, 2, 1, 2, 3};

  oi->getObject()->getMesh()->setDataBuffer(
    agt3d::DataStream::VERTEX, (float*)vertices.data(),
    vertices.size() * sizeof(glm::vec3), GL_FLOAT, 3);
  oi->getObject()->getMesh()->setDataBuffer(
    agt3d::DataStream::TEXCOORD0, (float*)uvs.data(),
    uvs.size() * sizeof(glm::vec2), GL_FLOAT, 2);
  oi->getObject()->getMesh()->setIndices(indices);

  oi->getObject()->getMesh()->updateVAO();

  return oi;
}
}  // namespace agt3d
