#pragma once
#include <type_traits>

#include "agt_stdafx.h"

namespace agt3d
{
using shader_param_t = std::variant<int, float, glm::vec2, glm::vec3, glm::vec4,
                                    glm::mat3, glm::mat4>;
struct Shader {
  std::map<std::string, GLuint> uniforms;
  std::map<std::string, GLuint> attributes;
  std::string glslShaderPath;
  std::string glslShaderSource;
  uint32_t program;
  bool compiled;
};

std::optional<Shader> compileShader(const std::string& path) noexcept;
void cacheShader(const std::string& id, const Shader& shader) noexcept;
std::optional<const Shader*> getShader(const std::string& id) noexcept;
void useShader(const Shader& shader) noexcept;

inline bool hasUniform(const Shader& shader, const std::string& name) noexcept
{
  auto it = shader.uniforms.find(name);
  return it != shader.uniforms.end();
}

inline GLuint getUniformLoc(const Shader& shader, const std::string& name)
{
  if (!hasUniform(shader, name)) {
    return 0;
  }
  return shader.uniforms.at(name);
}

inline void setUniform(const Shader& shader, const std::string& name,
                       const glm::mat4& val)
{
  auto loc = getUniformLoc(shader, name);
  glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)glm::value_ptr(val));
}

inline void setUniform(const Shader& shader, const std::string& name,
                       const glm::mat4&& val)
{
  auto loc = getUniformLoc(shader, name);
  glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)glm::value_ptr(val));
}

inline void setUniform(const Shader& shader, const std::string& name, float val)
{
  auto loc = getUniformLoc(shader, name);
  glUniform1f(loc, val);
}

inline void setUniform(const Shader& shader, const std::string&& name,
                       float val)
{
  auto loc = getUniformLoc(shader, name);
  glUniform1f(loc, val);
}

inline void setUniform(const Shader& shader, const std::string& name,
                       glm::vec2 val)
{
  auto loc = getUniformLoc(shader, name);
  glUniform2f(loc, val.x, val.y);
}

inline void setUniform(const Shader& shader, const std::string&& name,
                       glm::vec2 val)
{
  auto loc = getUniformLoc(shader, name);
  glUniform2f(loc, val.x, val.y);
}

inline void setUniform(const Shader& shader, const std::string& name,
                       glm::vec3 val)
{
  auto loc = getUniformLoc(shader, name);
  glUniform3f(loc, val.x, val.y, val.z);
}

inline void setUniform(const Shader& shader, const std::string&& name,
                       glm::vec3 val)
{
  auto loc = getUniformLoc(shader, name);
  glUniform3f(loc, val.x, val.y, val.z);
}

inline void setUniform(const Shader& shader, const std::string& name,
                       glm::vec4 val)
{
  auto loc = getUniformLoc(shader, name);
  glUniform4f(loc, val.x, val.y, val.z, val.w);
}

inline void setUniform(const Shader& shader, const std::string&& name,
                       glm::vec4 val)
{
  auto loc = getUniformLoc(shader, name);
  glUniform4f(loc, val.x, val.y, val.z, val.w);
}

};  // namespace agt3d
