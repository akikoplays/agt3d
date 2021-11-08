#pragma once
#include "agt_stdafx.h"

namespace agt3d
{

struct TexDesc {
  GLuint format;
  GLuint type;
  GLuint texture;
};

class FBO
{
 public:
  FBO(int _width, int _height, bool _multiSampled, int _samples,
      GLuint _internalFormat);
  ~FBO();
  FBO operator=(const FBO& other) = delete;

  GLuint attachTexture(GLuint attachment, GLuint type = GL_UNSIGNED_BYTE,
                       GLuint format = GL_RGB,
                       GLuint minmagFilter = GL_NEAREST);
  GLuint attachRenderBuffer();
  void bind() const;
  void unbind() const;
  bool checkReadiness();
  void resize(int newWidth, int newHeight);
  /**
   * @brief Performs simple 1:1 blit to destination. Doesn't check errors. Call
   * checkOpenGLErrors() afterwards if you need to debug. Automatic rescale to
   * fit destination fbo is performed, set filter to GL_LINEAR if you need
   * interpolation.
   * @param dst destination FBO.
   * @param mask mask to use for blitting.
   * @param filter filter to apply.
   */
  void blitQuick(agt3d::FBO& dst, GLuint mask = GL_COLOR_BUFFER_BIT,
                 GLuint filter = GL_NEAREST);

 public:
  std::vector<TexDesc> textures;
  int width = 0;
  int height = 0;
  GLuint id = 0;
  GLuint rboDepth = 0;
  bool multiSampled = false;
  int samples = 0;
  GLuint internalFormat = GL_RGBA8;
};

std::unique_ptr<agt3d::FBO> fboFactory(int width, int height, bool multisample,
                                       int samples);
}  // namespace agt3d
