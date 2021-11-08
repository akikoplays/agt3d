#pragma once
#include "agt_AABB.h"

namespace agt3d {

  enum class DataStream {
    VERTEX = 0,
    NORMAL = 1,
    TEXCOORD0 = 2,
    COLOR = 3,
    TEXCOORD1 = 4,
    PIXCOORD = 5,
    LAST
  };

  class Mesh
  {
  public:
    Mesh();
    ~Mesh();
    void setDataBuffer(DataStream id, const float* data, size_t sizeInBytes, int type, int components);
    void setDataBufferFromInterleaved(DataStream id, const uint8_t* data, int strideInBytes, int type, int components, size_t elementsCount, size_t elementSizeInBytes);
    bool hasDataBuffer(DataStream id);
    std::vector<uint8_t>& getDataBuffer(DataStream id);
    std::pair<int, int> getDataBufferDesc(DataStream id);
    void updateData();
    void updateVAO();
    void setIndices(const std::vector<unsigned int>& _indices);
    void use();
    void unuse();
    const BoundingSphere& getBoundingSphere();
    const AABB& getAABB();

  private:
    void calculateAABB();
    void initOpenGLObjects();

  public:
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint vib = 0;
    std::vector<uint8_t> data;
    std::vector<uint8_t> rawBuffers[static_cast<int>(DataStream::LAST)];
    std::vector<std::pair<int, int>> rawBufferDesc;
    std::vector<unsigned int> indices;
    uint32_t streams = 0UL;
  private:
    AABB aabb;
    BoundingSphere boundingSphere = { {0,0,0}, 0.0f };
    bool aabbNeedsUpdate = true;
    bool needsUpdate = false;

  };
  
}