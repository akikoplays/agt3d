#pragma once

#include "agt_node.h"
#include "agt_object.h"

namespace agt3d
{

struct Shader;

class RenderTechnique
{
 public:
  enum class DrawPrimitiveType { TRIANGLES, LINES, LINE_STRIP, POINTS };

  glm::vec4 borderColor = {1, 1, 1, 1};
  /// Deprecated. Set point size via shader uniforms.
  // float pointSize = 0.25f;
  float borderRadius = 0.45f;
  float lineWidth = 1.0f;
  DrawPrimitiveType primitiveType = DrawPrimitiveType::TRIANGLES;
  bool disableDepthTest = false;
  bool enableAlphaBlending = false;
  GLuint alphaSrc = GL_ONE;
  GLuint alphaDst = GL_ONE_MINUS_SRC_ALPHA;
};

class ObjectInstance
{
 public:
  ObjectInstance(std::string _name);
  ~ObjectInstance();
  /**
   * Discontinued. Renderer is not a part of OI.
   */
  // void render(agt3d::Shader* shader, glm::mat4& view, glm::mat4& proj);
  void setObject(std::shared_ptr<agt3d::Object>& _obj);
  agt3d::Object* getObject();
  void setLocalPRS(agt3d::Node& prs);
  agt3d::Node getLocalPRS();
  void setLocalPosition(const glm::vec3& position);
  void setLocalScale(const glm::vec3& scale);
  void setLocalRotation(const glm::quat& rotation);
  agt3d::BoundingSphere getBoundingSphere();
  glm::mat4 getTm();
  void setParent(std::shared_ptr<agt3d::ObjectInstance>& oi);
  const agt3d::ObjectInstance* getParent();
  bool isRenderable();
  void setRenderTechnique(const agt3d::RenderTechnique& tech);
  const agt3d::RenderTechnique& getRenderTechnique();
  void setEnabled(bool ena);
  bool isEnabled();

 private:
  bool tmDirty = true;
  glm::mat4 tm;

 public:
  std::string name;

 private:
  agt3d::Node localPRS;
  std::shared_ptr<agt3d::Object> obj = nullptr;
  std::shared_ptr<agt3d::ObjectInstance> parent = nullptr;
  agt3d::RenderTechnique technique;
  bool enabled = true;
};

}  // namespace agt3d
