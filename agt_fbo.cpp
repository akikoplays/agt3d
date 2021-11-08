#include "agt_fbo.h"
#include "agt_utils.h"

using namespace std;

namespace agt3d
{

FBO::FBO(int _width, int _height, bool _multiSampled, int _samples,
         GLuint _internalFormat)
    : width(_width),
      height(_height),
      multiSampled(_multiSampled),
      samples(_samples),
      internalFormat(_internalFormat)
{
  glGenFramebuffers(1, &id);
  return;
}

FBO::~FBO()
{
  for (auto it = textures.begin(); it != textures.end(); ++it) {
    GLuint tex = (*it).texture;
    glDeleteTextures(1, &tex);
  }
  glDeleteFramebuffers(1, &id);
  glDeleteRenderbuffers(1, &rboDepth);
  return;
}

bool FBO::checkReadiness()
{
  bind();
  auto code = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (code != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "Framebuffer not complete! Code: " << code << std::endl;
    unbind();
    return false;
  }
  unbind();
  return true;
}

void agt3d::FBO::resize(int newWidth, int newHeight)
{
  // NOP if size already matching
  if (width == newWidth && height == newHeight) {
    return;
  }
  width = newWidth;
  height = newHeight;

  // Resize renderbuffer
  if (rboDepth) {
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    if (multiSampled) {
      glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples,
                                       GL_DEPTH24_STENCIL8, width, height);
    } else {
      glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    }
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
  }
  agt3d::checkOpenGLErrors();
  checkReadiness();
  // Resize all attachments
  for (const auto& desc : textures) {
    if (multiSampled) {
      glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, desc.texture);
      glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples,
                              internalFormat, width, height, GL_TRUE);
      glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    } else {
      glBindTexture(GL_TEXTURE_2D, desc.texture);
      glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0,
                   desc.format, desc.type, NULL);
      glBindTexture(GL_TEXTURE_2D, 0);
    }
  }
  checkOpenGLErrors();
  checkReadiness();
}

void agt3d::FBO::blitQuick(agt3d::FBO& dst, GLuint mask, GLuint filter)
{
  glBindFramebuffer(GL_READ_FRAMEBUFFER, id);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst.id);
  glBlitFramebuffer(0, 0, width, height, 0, 0, dst.width, dst.height, mask,
                    filter);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

GLuint FBO::attachRenderBuffer()
{
  glGenRenderbuffers(1, &rboDepth);
  glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
  if (multiSampled) {
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples,
                                     GL_DEPTH24_STENCIL8, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, rboDepth);
  } else {
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, id);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                              GL_RENDERBUFFER, rboDepth);
  }
  checkOpenGLErrors();
  return rboDepth;
}

GLuint FBO::attachTexture(GLuint attachment, GLuint type, GLuint format,
                          GLuint minmagFilter)
{
  glBindFramebuffer(GL_FRAMEBUFFER, id);

  GLuint texture;
  glGenTextures(1, &texture);
  if (multiSampled) {
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, texture);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, internalFormat,
                            width, height, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment,
                           GL_TEXTURE_2D_MULTISAMPLE, texture, 0);
  } else {
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format,
                 type, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minmagFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, minmagFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture,
                           0);
  }

  textures.push_back({format, type, texture});
  checkOpenGLErrors();
  return texture;
}

void FBO::bind() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, id);
  return;
}

void FBO::unbind() const
{
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  return;
}

std::unique_ptr<agt3d::FBO> fboFactory(int width, int height, bool multisample,
                                       int samples)
{
  auto fbo =
    std::make_unique<agt3d::FBO>(width, height, multisample, samples, GL_RGBA);
  fbo->attachRenderBuffer();
  fbo->attachTexture(GL_COLOR_ATTACHMENT0);
  if (!fbo->checkReadiness()) {
    return nullptr;
  }
  fbo->unbind();
  return fbo;
}
}  // namespace agt3d
