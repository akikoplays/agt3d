#pragma once

namespace agt3d {

  class AABB
  {
  public:
    AABB();
    void calculateFromPoints(const glm::vec3* verts, const uint32_t numVerts);
    glm::vec3 min;
    glm::vec3 max;
  };

  struct BoundingSphere
  {
    glm::vec3 center;
    float radius;

    BoundingSphere& operator + (const BoundingSphere& a)
    {
      center = a.center;
      radius = a.radius;
      return *this;
    }

    BoundingSphere& operator+=(const BoundingSphere& rhs)
    {
      center += rhs.center;
      radius += rhs.radius;
      return *this;
    }
  };

}
