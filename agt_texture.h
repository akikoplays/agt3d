#pragma once

#include "agt_stdafx.h"

namespace agt3d
{

class Texture
{
 public:
  Texture();
  ~Texture();
  Texture& operator=(Texture&& other) = default;
  Texture& operator=(const Texture& other) = delete;
  Texture(Texture&) = delete;
  void loadFromFile(const std::string& path) noexcept;
  uint32_t getTexture();
  void updateFromBGRABuffer(const void* data);
  void updateFromRGBBuffer(const void* data);
  void updateFromRGBABuffer(const void* data);
  void updateFromRedBuffer(const void* data);
  void createOpenGLTextureObject(int width, int height, int _channels,
                                 bool _genMipmaps, const void* data) noexcept;
  void create(int width, int height);
  void create(int width, int height, GLenum _sizedInternalFormat);
  void update(GLenum format, GLenum type, void* data);
  glm::ivec2 getResolution();
  void setType(const std::string& _type);
  const std::string& getType() const noexcept;
  bool isLoaded() const noexcept;
  const std::string& getPath() noexcept;

 private:
  bool fromFile();

 private:
  std::string path = "";
  std::string type = "";
  glm::ivec2 resolution = {0, 0};
  int channels = 0;
  GLenum textureTarget = GL_TEXTURE_2D;
  GLenum sizedInternalFormat;
  GLuint textureObject = 0;
  bool genMipmaps = false;
  bool loaded = false;
};

}  // namespace agt3d
