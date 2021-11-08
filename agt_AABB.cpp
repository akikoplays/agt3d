#include "agt_stdafx.h"
#include "agt_AABB.h"
#include "agt_utils.h"

using namespace agt3d;

AABB::AABB() : min({ 0,0,0 }), max({ 0,0,0 })
{
  return;
}

void AABB::calculateFromPoints(const glm::vec3* verts, const uint32_t numVerts)
{
  unsigned int i;
  MY_ASSERT(numVerts > 0, "numVerts is zero");

  min = verts[0];
  max = verts[0];

  for (i = 1; i < numVerts; i++) {
    if (verts[i].x < min.x)
      min.x = verts[i].x;
    if (verts[i].y < min.y)
      min.y = verts[i].y;
    if (verts[i].z < min.z)
      min.z = verts[i].z;
    if (verts[i].x > max.x)
      max.x = verts[i].x;
    if (verts[i].y > max.y)
      max.y = verts[i].y;
    if (verts[i].z > max.z)
      max.z = verts[i].z;
  }

  return;
}