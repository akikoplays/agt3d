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

void setGlslVersion(const std::string& version) noexcept;
std::string getGlslVersion() noexcept;

std::optional<Shader> compileShader(const std::string& path) noexcept;
void cacheShader(const std::string& id, const Shader& shader) noexcept;
std::optional<const Shader*> getShader(const std::string& id) noexcept;
void useShader(const Shader& shader) noexcept;

inline bool hasUniform(const Shader& shader, const std::string& name) noexcept
{
  auto it = shader.uniforms.find(name);
  return it != shader.uniforms.end();
}

inline GLuint getUniformLoc(const Shader& shader,
                            const std::string& name) noexcept
{
  if (!hasUniform(shader, name)) {
    return 0;
  }
  return shader.uniforms.at(name);
}

inline void setUniform(const Shader& shader, const std::string& name,
                       const agt3d::shader_param_t param) noexcept
{
  auto loc = getUniformLoc(shader, name);

  if (std::get_if<float>(&param)) {
    glUniform1f(loc, std::get<float>(param));
  } else if (std::get_if<int>(&param)) {
    glUniform1i(loc, std::get<int>(param));
  } else if (std::get_if<glm::vec2>(&param)) {
    glUniform2f(loc, std::get<glm::vec2>(param).x,
                std::get<glm::vec2>(param).y);
  } else if (std::get_if<glm::vec3>(&param)) {
    glUniform3f(loc, std::get<glm::vec3>(param).x, std::get<glm::vec3>(param).y,
                std::get<glm::vec3>(param).z);
  } else if (std::get_if<glm::vec4>(&param)) {
    glUniform4f(loc, std::get<glm::vec4>(param).x, std::get<glm::vec4>(param).y,
                std::get<glm::vec4>(param).z, std::get<glm::vec4>(param).w);
  } else if (std::get_if<glm::mat3>(&param)) {
    glUniformMatrix3fv(
      loc, 1, GL_FALSE,
      (const GLfloat*)glm::value_ptr(std::get<glm::mat3>(param)));
  } else if (std::get_if<glm::mat4>(&param)) {
    glUniformMatrix4fv(
      loc, 1, GL_FALSE,
      (const GLfloat*)glm::value_ptr(std::get<glm::mat4>(param)));
  } else {
    MY_ABORT("param type not implemented");
  }
}

};  // namespace agt3d
