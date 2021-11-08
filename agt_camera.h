#pragma once
#include "glm/glm.hpp"
#include "agt_utils.h"

namespace agt3d
{
class BaseCamera
{
 public:
  BaseCamera();

  virtual const glm::mat4& getView() const noexcept = 0;
  virtual glm::vec3 getEye() const noexcept = 0;

  void setOrtho(bool ena) noexcept;
  glm::vec3 getUp() const noexcept;
  glm::vec3 getRight() const noexcept;
  glm::vec3 getForward() const noexcept;
  void setViewport(glm::ivec4& v) noexcept;
  void setViewport(glm::ivec4&& v) noexcept;
  void setFov(float deg) noexcept;
  /**
   * @brief Return fov in degrees.
   * @return fov in degrees.
   */
  float getFov() const noexcept;
  void setNear(float n) noexcept;
  void setFar(float f) noexcept;
  virtual void update() const noexcept;
  const glm::mat4& getProjection() const noexcept;
  const glm::ivec4& getViewport() const noexcept;
  int getViewport(int component) const noexcept;
  std::pair<float, float> getNearFar() const noexcept;

  /**
   * @brief UnProject 2d point and return ray in world space. Ray's origin is
   * set to camera eye position.
   * @param point 2d point on the screen.
   * @return ray starting at camera eye position, going into the screen through
   * the 2d point.
   */
  agt3d::ray raycast2dPoint(glm::vec2 point) const noexcept;

 protected:
  mutable glm::mat4 mat;
  mutable glm::mat4 projection;
  glm::ivec4 viewport = {0, 0, 800, 600};
  float fovInDeg = 60.0f;
  float nearZ = 0.1f;
  float farZ = 100.0f;
  mutable bool isDirty = true;
  bool isOrtho = false;
};

class TargetCamera : public BaseCamera
{
 public:
  TargetCamera();
  TargetCamera(const glm::vec3& _eye, const glm::vec3& _center,
               const glm::vec3& _up);
  const glm::mat4& getView() const noexcept override;
  glm::vec3 getEye() const noexcept override;
  void setEye(const glm::vec3& _eye) noexcept;
  void setCenter(const glm::vec3& _center) noexcept;
  glm::vec3 getCenter() const noexcept;
  void setUp(const glm::vec3& _up) noexcept;
  /**
   * @brief Extend the viewing range, by adding the distance to the eye to
   * center vector, center remains unchanged, eye moves by the value given.
   * @param val How much to add to the eye2center segment.
   */
  void addEyeDistance(float val) noexcept;
  void rotateEyeAroundYAxis(float rad) noexcept;
  void rotateEyeAroundXAxis(float rad) noexcept;
  /**
   * @brief Returns current distance of the eye from the center.
   * @return distance of the eye to the center.
   */
  float getEyeDistance() const noexcept;

 private:
  void update() const noexcept override;

 private:
  glm::vec3 eye = {0.0f, 0.0f, 2.0f};
  glm::vec3 center = {0.0f, 0.0f, 0.0f};
  glm::vec3 up = {0.0f, 1.0f, 0.0f};
};

class OrbitCamera : public BaseCamera
{
 public:
  OrbitCamera();

  /**
   * @brief Rotate around x and y axis.
   * @param x Angle in radians.
   * @param y Angle in radians.
   */
  void rotate(float x, float y) noexcept;
  void setAngles(const glm::vec3& _angles) noexcept;
  void setUp(const glm::vec3& _up) noexcept;
  void setRadius(float r) noexcept;
  void setCenter(const glm::vec3& c) noexcept;
  glm::vec3 getCenter() const noexcept;
  const glm::mat4& getView() const noexcept override;
  glm::vec3 getEye() const noexcept override;
  void update() const noexcept override;

 public:
  glm::vec3 angles;
  mutable glm::vec3 eye;
  glm::vec3 center;
  glm::vec3 up;
  float radius;
};
}  // namespace agt3d
