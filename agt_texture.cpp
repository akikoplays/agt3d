#include "agt_texture.h"

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image.h"

namespace agt3d
{

Texture::Texture() {}

const std::string& Texture::getPath() noexcept { return path; }

void Texture::loadFromFile(const std::string& path) noexcept
{
  this->path = path;
  loaded = fromFile();
}

Texture::~Texture()
{
  if (textureObject) {
#ifdef VERBOSE
    std::cout << "-- deleted texture object: " << textureObject
              << " path: " << path << "\n";
#endif
    glDeleteTextures(1, &textureObject);
  }
}

bool Texture::isLoaded() const noexcept { return loaded; }

void Texture::setType(const std::string& _type) { type = _type; }

const std::string& Texture::getType() const noexcept { return type; }

GLuint Texture::getTexture() { return textureObject; }

bool Texture::fromFile()
{
  std::cout << "Loading texture from disk: " << path << "\n";
  stbi_set_flip_vertically_on_load(true);
  auto image =
    stbi_load(path.c_str(), &resolution.x, &resolution.y, &channels, 0);
  if (!image) {
    std::cerr << "WARN: texture failed to load\n";
    return false;
  }
  std::cout << "-- resolution: " << glm::to_string(resolution) << std::endl;
  std::cout << "-- result:" << std::to_string(image != nullptr) << std::endl;
  createOpenGLTextureObject(resolution.x, resolution.y, channels, true, image);
  stbi_image_free(image);
  return true;
}

glm::ivec2 Texture::getResolution() { return resolution; }

void Texture::updateFromBGRABuffer(const void* data)
{
  glBindTexture(GL_TEXTURE_2D, textureObject);
  glTexSubImage2D(GL_TEXTURE_2D,                           // target
                  0,                                       // level
                  0,                                       // xoffset
                  0,                                       // yoffset
                  resolution.x,                            // width
                  resolution.y,                            // height
                  GL_BGRA,                                 // format
                  GL_UNSIGNED_BYTE,                        // type
                  reinterpret_cast<const GLvoid*>(data));  // pixels
  if (genMipmaps) {
    glGenerateMipmap(textureTarget);
  }
}

void Texture::updateFromRGBBuffer(const void* data)
{
  glBindTexture(GL_TEXTURE_2D, textureObject);
  glTexSubImage2D(GL_TEXTURE_2D,                           // target
                  0,                                       // level
                  0,                                       // xoffset
                  0,                                       // yoffset
                  resolution.x,                            // width
                  resolution.y,                            // height
                  GL_RGB,                                  // format
                  GL_UNSIGNED_BYTE,                        // type
                  reinterpret_cast<const GLvoid*>(data));  // pixels
  if (genMipmaps) {
    glGenerateMipmap(textureTarget);
  }
}

void Texture::updateFromRedBuffer(const void* data)
{
  glBindTexture(GL_TEXTURE_2D, textureObject);
  glTexSubImage2D(GL_TEXTURE_2D,                           // target
                  0,                                       // level
                  0,                                       // xoffset
                  0,                                       // yoffset
                  resolution.x,                            // width
                  resolution.y,                            // height
                  GL_RED,                                  // format
                  GL_UNSIGNED_BYTE,                        // type
                  reinterpret_cast<const GLvoid*>(data));  // pixels
  if (genMipmaps) {
    glGenerateMipmap(textureTarget);
  }
}

void Texture::updateFromRGBABuffer(const void* data)
{
  glBindTexture(GL_TEXTURE_2D, textureObject);
  glTexSubImage2D(GL_TEXTURE_2D,                           // target
                  0,                                       // level
                  0,                                       // xoffset
                  0,                                       // yoffset
                  resolution.x,                            // width
                  resolution.y,                            // height
                  GL_RGBA,                                 // format
                  GL_UNSIGNED_BYTE,                        // type
                  reinterpret_cast<const GLvoid*>(data));  // pixels
  if (genMipmaps) {
    glGenerateMipmap(textureTarget);
  }
}

void Texture::createOpenGLTextureObject(int width, int height, int _channels,
                                        bool _genMipmaps,
                                        const void* data) noexcept
{
  // Delete texture object if already exists!
  if (textureObject != 0) {
    glDeleteTextures(1, &textureObject);
  }
  glGenTextures(1, &textureObject);
  // Store current binding
  GLuint boundTexture = 0;
  glGetIntegerv(GL_TEXTURE_BINDING_2D, (GLint*)&boundTexture);
  glBindTexture(textureTarget, textureObject);
#ifdef VERBOSE
  std::cout << "-- created texture object: " << textureObject << std::endl;
#endif
  genMipmaps = _genMipmaps;
  resolution.x = width;
  resolution.y = height;
  channels = _channels;
  auto format = GL_RGB;

  if (channels == 1) {
    format = GL_RED;
  } else if (channels == 3) {
    format = GL_RGB;
  } else if (channels == 4) {
    format = GL_RGBA;
  } else {
    std::cerr << "unsupported image format, channels #" << channels
              << std::endl;
    abort();
  }

  glTexImage2D(textureTarget, 0, format, resolution.x, resolution.y, 0, format,
               GL_UNSIGNED_BYTE, (void*)data);
  if (genMipmaps) {
    glGenerateMipmap(textureTarget);
  }
  // glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  // glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER,
                  genMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
  glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  glBindTexture(textureTarget, boundTexture);
}

void Texture::update(GLenum format, GLenum type, void* data)
{
  glBindTexture(GL_TEXTURE_2D, textureObject);
  glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, resolution.x, resolution.y, format,
                  type, data);
  glBindTexture(GL_TEXTURE_2D, 0);
}

}  // namespace agt3d
