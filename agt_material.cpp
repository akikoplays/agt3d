#include "agt_material.h"
#include "agt_stdafx.h"

namespace agt3d
{

Material::Material()
{
#ifdef VERBOSE
  std::cout << "Material ctor" << std::endl;
#endif
}

Material::~Material()
{
#ifdef VERBOSE
  std::cout << "Material dtor" << std::endl;
#endif
}

void Material::setShader(const std::shared_ptr<Shader>& _shader)
{
  shader = _shader;
}

Shader* Material::getShader() { return shader.get(); }

void Material::setShaderParam(const std::string&& key, shader_param_t param)
{
  std::string type = "";

  if (std::get_if<float>(&param)) {
    type = "float";
  } else if (std::get_if<int>(&param)) {
    type = "int";
  } else if (std::get_if<glm::vec2>(&param)) {
    type = "vec2";
  } else if (std::get_if<glm::vec3>(&param)) {
    type = "vec3";
  } else if (std::get_if<glm::vec4>(&param)) {
    type = "vec4";
  } else if (std::get_if<glm::mat3>(&param)) {
    type = "mat3";
  } else if (std::get_if<glm::mat4>(&param)) {
    type = "mat4";
  }

  shaderParams.insert_or_assign(key, std::make_pair(type, param));
}

std::map<std::string, std::pair<std::string, agt3d::shader_param_t>>&
Material::getShaderParams()
{
  return shaderParams;
}
}  // namespace agt3d
