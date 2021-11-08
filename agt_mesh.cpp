#include "agt_mesh.h"
#include "agt_utils.h"
#include "agt_stdafx.h"

namespace agt3d
{

Mesh::Mesh()
    : rawBufferDesc(static_cast<int>(DataStream::LAST), std::make_pair(-1, -1))
{
  initOpenGLObjects();
#ifdef VERBOSE
  std::cout << "Mesh ctor" << std::endl;
#endif
}

Mesh::~Mesh()
{
  glDeleteVertexArrays(1, &vao);
  glDeleteBuffers(1, &vbo);
  glDeleteBuffers(1, &vib);
  data.clear();
  for (auto& buffer : rawBuffers) {
    buffer.resize(0);
  }
  indices.resize(0);
#ifdef VERBOSE
  std::cout << "Mesh dtor" << std::endl;
#endif
}

std::vector<uint8_t>& Mesh::getDataBuffer(DataStream id)
{
  MY_ASSERT(hasDataBuffer(id), "Mesh does not contain this data buffer");
  auto _id = static_cast<int>(id);
  return rawBuffers[_id];
}

std::pair<int, int> Mesh::getDataBufferDesc(DataStream id)
{
  MY_ASSERT(hasDataBuffer(id), "Mesh does not contain this data buffer");
  auto _id = static_cast<int>(id);
  return rawBufferDesc[_id];
}

void Mesh::setDataBuffer(DataStream id, const float* data, size_t sizeInBytes,
                         int type, int components)
{
  auto _id = static_cast<int>(id);
  streams |= (1 << _id);
  rawBufferDesc[_id] = std::make_pair(type, components);
  rawBuffers[_id].resize(sizeInBytes);
  memcpy(rawBuffers[_id].data(), (uint8_t*)data, sizeInBytes);
  needsUpdate = true;

  if (id == DataStream::VERTEX) {
    aabbNeedsUpdate = true;
  }
  return;
}

void Mesh::setDataBufferFromInterleaved(DataStream id, const uint8_t* data,
                                        int strideInBytes, int type,
                                        int components, size_t elementsCount,
                                        size_t elementSizeInBytes)
{
  if (elementsCount == 0) {
    return;
  }

  auto _id = static_cast<int>(id);
  streams |= (1 << _id);
  rawBuffers[_id].resize(elementsCount * elementSizeInBytes);
  rawBufferDesc[_id] = std::make_pair(type, components);

  const uint8_t* src = data;
  auto stride = strideInBytes - elementSizeInBytes;
  uint8_t* dst = reinterpret_cast<uint8_t*>(rawBuffers[_id].data());

  for (size_t c = 0; c < elementsCount; c++) {
    for (size_t i = 0; i < elementSizeInBytes; i++) {
      *dst++ = *src++;
    }
    src += stride;
  }
  needsUpdate = true;

  if (id == DataStream::VERTEX) {
    aabbNeedsUpdate = true;
  }

  return;
}

bool Mesh::hasDataBuffer(DataStream id)
{
  return streams & (1 << static_cast<int>(id));
}

void Mesh::initOpenGLObjects()
{
  if (vao == 0) {
    glGenVertexArrays(1, &vao);
  }
  glBindVertexArray(vao);
  if (vbo == 0) {
    glGenBuffers(1, &vbo);
  }
  if (vib == 0) {
    glGenBuffers(1, &vib);
  }

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  checkOpenGLErrors();
}

void Mesh::updateData()
{
  uint32_t totalDataLen = 0;
  for (auto i = 0; i < static_cast<int>(DataStream::LAST); i++) {
    if (hasDataBuffer(static_cast<DataStream>(i))) {
      totalDataLen += static_cast<uint32_t>(rawBuffers[i].size());
    }
  }

  data.resize(0);
  data.reserve(totalDataLen);

  auto it = data.begin();
  for (auto i = 0; i < static_cast<int>(DataStream::LAST); i++) {
    if (!hasDataBuffer(static_cast<DataStream>(i))) {
      continue;
    }
    data.insert(data.end(), rawBuffers[i].begin(), rawBuffers[i].end());
  }
  needsUpdate = false;
}

void Mesh::updateVAO()
{
#ifdef _DEBUG
  assert(vao);
  assert(vbo);
  assert(vib);
#endif

  glBindVertexArray(vao);

  // New chunk combiner approach
  if (needsUpdate) {
    updateData();
  }

  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, data.size(), data.data(), GL_STATIC_DRAW);

  size_t offset = 0;
  for (auto i = 0; i < static_cast<int>(DataStream::LAST); i++) {
    if (hasDataBuffer(static_cast<DataStream>(i))) {
      glEnableVertexAttribArray(i);
      int type = rawBufferDesc[i].first;
      int components = rawBufferDesc[i].second;

      glVertexAttribPointer(i, components, type, GL_FALSE, 0,
                            (const GLvoid*)offset);
      offset += rawBuffers[i].size();
    }
  }

  if (indices.size()) {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vib);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * indices.size(),
                 indices.data(), GL_STATIC_DRAW);
  }

  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  checkOpenGLErrors();
}

void Mesh::setIndices(const std::vector<unsigned int>& _indices)
{
  indices = _indices;
  needsUpdate = true;
}

void Mesh::use()
{
#if _DEBUG
  assert(vao);
#endif
  glBindVertexArray(vao);
  checkOpenGLErrors();
}

void Mesh::unuse()
{
  glBindVertexArray(0);
}

void Mesh::calculateAABB()
{
  if (!aabbNeedsUpdate) {
    return;
  }

  auto& vertsRaw = getDataBuffer(agt3d::DataStream::VERTEX);
  glm::vec3* verts = (glm::vec3*)vertsRaw.data();
  size_t numVerts = vertsRaw.size() / sizeof(glm::vec3);
  aabb.calculateFromPoints(verts, static_cast<uint32_t>(numVerts));
  aabbNeedsUpdate = false;

  float R = glm::distance(aabb.min, aabb.max);
  boundingSphere.radius = R / 2.0f;
  boundingSphere.center = (aabb.min + aabb.max) / 2.0f;
}
/*
  void Mesh::calculateBoundingSphere()
  {
    if (!boundingSphereDirty) {
      return;
    }

    glm::vec3 center = { 0,0,0 };
    uint32_t vertsNum = 0;
    auto& vertsRaw = getDataBuffer(agt3d::DataStream::VERTEX);
    glm::vec3* p = (glm::vec3*)vertsRaw.data();
    for (auto i = 0; i < vertsRaw.size() / sizeof(glm::vec3); ++i) {
      center += *p++;
    }
    vertsNum += vertsRaw.size() / sizeof(glm::vec3);
    center = center / (float)vertsNum;

    float maxDistSq = 0.0f;
    p = (glm::vec3*)vertsRaw.data();
    for (auto i = 0; i < vertsRaw.size() / sizeof(glm::vec3); ++i) {
      auto d = glm::distance2(*p++, center);
      if (d > maxDistSq) {
        maxDistSq = d;
      }
    }

    boundingSphere.center = center;
    boundingSphere.radius = sqrtf(maxDistSq);
    boundingSphereDirty = false;
    return;
  }
*/

const AABB& Mesh::getAABB()
{
  calculateAABB();
  return aabb;
}

const BoundingSphere& Mesh::getBoundingSphere()
{
  calculateAABB();
  return boundingSphere;
}

}  // namespace agt3d
