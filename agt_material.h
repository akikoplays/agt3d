#pragma once

#include "agt_glsl_shader.h"
#include "agt_texture.h"

namespace agt3d
{

class Material
{
 public:
  Material();
  ~Material();
  void setShader(const std::shared_ptr<agt3d::Shader>& _shader);
  agt3d::Shader* getShader();
  /**
   * @brief Wraps provided uniform name and variant value into a proper <string,
   * pair<string, shader_param_t>> entity.
   * @param key string name of the uniform that is used in the shader.
   * @param param variant value or type shader_param_t.
   */
  void setShaderParam(const std::string&& key, shader_param_t param);
  std::map<std::string, std::pair<std::string, agt3d::shader_param_t>>&
  getShaderParams();

 public:
  std::vector<std::shared_ptr<agt3d::Texture>> textures;

 private:
  std::shared_ptr<agt3d::Shader> shader = nullptr;
  /**
   * @brief : name_str <type_str, value_variant>
   */
  std::map<std::string, std::pair<std::string, agt3d::shader_param_t>>
    shaderParams;
};

}  // namespace agt3d
