#pragma once

#include <glm/glm/glm.hpp>

namespace agt3d
{

struct AABB {
  AABB();
  void calculateFromPoints(const glm::vec3* verts, const uint32_t numVerts);
  glm::vec3 min;
  glm::vec3 max;
};

struct BoundingSphere {
  glm::vec3 center;
  float radius;

  BoundingSphere& operator+(const BoundingSphere& a) noexcept;

  BoundingSphere& operator+=(const BoundingSphere& rhs) noexcept;
};

}  // namespace agt3d
