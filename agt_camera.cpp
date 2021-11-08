#include "agt_camera.h"
#include "agt_stdafx.h"
#include "agt_utils.h"

namespace agt3d
{

BaseCamera::BaseCamera() : mat(glm::mat4(1)), projection(glm::mat4(1)) {}

void BaseCamera::setOrtho(bool ena) noexcept
{
  isOrtho = ena;
  nearZ = -1.0f;
  farZ = 1.0f;
  ;
}

glm::vec3 BaseCamera::getUp() const noexcept
{
  auto mat = getView();
  return {mat[0][1], mat[1][1], mat[2][1]};
}

glm::vec3 BaseCamera::getRight() const noexcept
{
  if (isDirty) {
    update();
  }
  return {mat[0][0], mat[1][0], mat[2][0]};
}

glm::vec3 BaseCamera::getForward() const noexcept
{
  auto mat = getView();
  return {mat[0][2], mat[1][2], mat[2][2]};
}

void BaseCamera::setViewport(glm::ivec4& v) noexcept
{
  isDirty = true;
  viewport = v;
  return;
}

void BaseCamera::setViewport(glm::ivec4&& v) noexcept
{
  isDirty = true;
  viewport = v;
  return;
}

void BaseCamera::setFov(float deg) noexcept
{
  isDirty = true;
  fovInDeg = deg;
  return;
}

/**
 * @brief Return fov in degrees.
 * @return fov in degrees.
 */
float BaseCamera::getFov() const noexcept { return fovInDeg; }

void BaseCamera::setNear(float n) noexcept
{
  isDirty = true;
  nearZ = n;
}

void BaseCamera::setFar(float f) noexcept
{
  isDirty = true;
  farZ = f;
}

void BaseCamera::update() const noexcept
{
  if (!isOrtho) {
    projection =
      glm::perspective(glm::radians(fovInDeg),
                       (float)viewport.z / (float)viewport.w, nearZ, farZ);
  } else {
    projection =
      glm::ortho(0.0f, (float)viewport.z, (float)viewport.w, 0.0f, nearZ, farZ);
  }
}

const glm::mat4& BaseCamera::getProjection() const noexcept
{
  if (isDirty) {
    update();
  }
  return projection;
}

const glm::ivec4& BaseCamera::getViewport() const noexcept { return viewport; }

int BaseCamera::getViewport(int component) const noexcept
{
  return viewport[component];
}

std::pair<float, float> BaseCamera::getNearFar() const noexcept
{
  return std::make_pair(nearZ, farZ);
}

/**
 * @brief UnProject 2d point and return ray in world space. Ray's origin is
 * set to camera eye position.
 * @param point 2d point on the screen.
 * @return ray starting at camera eye position, going into the screen through
 * the 2d point.
 */
agt3d::ray BaseCamera::raycast2dPoint(glm::vec2 point) const noexcept
{
  auto viewportWidth = viewport[2];
  auto viewportHeight = viewport[3];
  auto& view = getView();
  auto& proj = getProjection();
  glm::vec4 viewport = {0, 0, viewportWidth, viewportHeight};
  glm::vec3 pos = glm::unProject({point.x, viewportHeight - point.y, 1.0f},
                                 view, proj, viewport);
  agt3d::ray r;
  r.origin = getEye();
  r.direction = glm::normalize(pos - getEye());
  return r;
}

TargetCamera::TargetCamera() {}

TargetCamera::TargetCamera(const glm::vec3& _eye, const glm::vec3& _center,
                           const glm::vec3& _up)
    : BaseCamera(), eye(_eye), center(_center), up(_up)
{
  isDirty = true;
}

const glm::mat4& TargetCamera::getView() const noexcept
{
  update();
  return mat;
}

glm::vec3 TargetCamera::getEye() const noexcept { return eye; }

void TargetCamera::setEye(const glm::vec3& _eye) noexcept
{
  eye = _eye;
  isDirty = true;
}

void TargetCamera::setCenter(const glm::vec3& _center) noexcept
{
  center = _center;
  isDirty = true;
}

glm::vec3 TargetCamera::getCenter() const noexcept { return center; }

void TargetCamera::setUp(const glm::vec3& _up) noexcept
{
  up = _up;
  isDirty = true;
}

/**
 * @brief Extend the viewing range, by adding the distance to the eye to
 * center vector, center remains unchanged, eye moves by the value given.
 * @param val How much to add to the eye2center segment.
 */
void TargetCamera::addEyeDistance(float val) noexcept
{
  auto v = eye - center;
  auto vn = glm::normalize(v);
  auto len = glm::length(v);
  eye = center + vn * (len + val);
  isDirty = true;
}

void TargetCamera::rotateEyeAroundYAxis(float rad) noexcept 
{
  auto rotAxis = glm::vec3(0, -1, 0);
  auto vec = eye - center;
  auto vecRotated = glm::rotateY(vec, rad);
  auto vecRotatedN = glm::normalize(vecRotated);
  eye = center + vecRotatedN * glm::length(vec);
  isDirty = true;
}

void TargetCamera::rotateEyeAroundXAxis(float rad) noexcept
{
  auto rotAxis = glm::vec3(1, 0, 0);
  auto vec = eye - center;
  auto vecRotated = glm::rotateX(vec, rad);
  auto vecRotatedN = glm::normalize(vecRotated);
  eye = center + vecRotatedN * glm::length(vec);
  isDirty = true;
}

/**
 * @brief Returns current distance of the eye from the center.
 * @return distance of the eye to the center.
 */
float TargetCamera::getEyeDistance() const noexcept
{
  return glm::length(eye - center);
}

void TargetCamera::update() const noexcept
{
  BaseCamera::update();

  mat = glm::lookAt(eye, center, up);
  isDirty = false;
}

OrbitCamera::OrbitCamera()
    : BaseCamera(),
      eye({0, 0, 0}),
      radius(2.0f),
      angles({0, 0, 0}),
      center({0, 0, 0}),
      up({0, 1, 0})
{
}

/**
 * @brief Rotate around x and y axis.
 * @param x Angle in radians.
 * @param y Angle in radians.
 */
constexpr double my_pi = 3.141592653589793238462643383279502884L;
void OrbitCamera::rotate(float x, float y) noexcept
{
  angles.y += y;
  angles.x += x;
  angles.x = std::clamp(angles.x, (float)-my_pi / 2.0f * 0.99f,
                        (float)my_pi / 2.0f * 0.99f);
  isDirty = true;
}

void OrbitCamera::setAngles(const glm::vec3& _angles) noexcept
{
  angles = _angles;
  isDirty = true;
}

void OrbitCamera::setUp(const glm::vec3& _up) noexcept
{
  up = _up;
  isDirty = true;
}

void OrbitCamera::setRadius(float r) noexcept
{
  radius = r;
  isDirty = true;
}

void OrbitCamera::setCenter(const glm::vec3& c) noexcept
{
  center = c;
  isDirty = true;
}

glm::vec3 OrbitCamera::getCenter() const noexcept { return center; }

const glm::mat4& OrbitCamera::getView() const noexcept
{
  if (isDirty) {
    update();
  }
  return mat;
}

glm::vec3 OrbitCamera::getEye() const noexcept { return eye; }

void OrbitCamera::update() const noexcept
{
  if (!isDirty) {
    return;
  }
  BaseCamera::update();

  constexpr glm::vec3 fwd = {0, 0,
                             1};  // initial look at dir = look into the screen
  constexpr glm::vec3 right = {1, 0, 0};  // initial right
  glm::quat rot = glm::angleAxis(angles.x, right);
  rot = glm::rotate(rot, angles.y, up);
  auto p = fwd * static_cast<float>(radius);
  p = center + p * rot;
  auto dir = p - center;
  dir = glm::normalize(dir);
  mat = glm::lookAt(center + dir * radius, center, up);
  // mat = glm::lookAt(p, center, up);
  eye = center + dir * radius;
  isDirty = false;
}

}  // namespace agt3d
