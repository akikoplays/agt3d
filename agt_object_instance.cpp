#include "agt_stdafx.h"
#include "agt_object_instance.h"

namespace agt3d {

  ObjectInstance::ObjectInstance(std::string _name) :
    name(_name),
    localPRS(Node({ 0,0,0 }, { 1,0,0,0 }, { 1,1,1 }))
  {
#ifdef VERBOSE
    std::cout << "OI ctor " << name << std::endl;
#endif
    return;
  }

  ObjectInstance::~ObjectInstance()
  {
#ifdef VERBOSE
    std::cout << "OI dtor " << name << std::endl;
#endif
    return;
  }

  void ObjectInstance::setObject(std::shared_ptr<Object>& _obj)
  {
    obj = _obj;
    return;
  }

  void ObjectInstance::setLocalPRS(Node& prs)
  {
    tmDirty = true;
    localPRS = prs;
    return;
  }

  agt3d::Node ObjectInstance::getLocalPRS()
  {
    return localPRS;
  }

  void ObjectInstance::setLocalPosition(const glm::vec3& position)
  {
    tmDirty = true;
    localPRS.setLocalPosition(position);
    return;
  }

  void ObjectInstance::setLocalScale(const glm::vec3& scale)
  {
    tmDirty = true;
    localPRS.setLocalScale(scale);
    return;
  }

  void ObjectInstance::setLocalRotation(const glm::quat& rotation)
  {
    tmDirty = true;
    localPRS.setLocalRotation(rotation);
    return;
  }

//  void ObjectInstance::render(Shader* shader, glm::mat4& view, glm::mat4& proj)
//  {
//    if (!isRenderable()) {
//      return;
//    }
//
//    if (shader) {
//      shader->use();
//    } else {
//      getObject()->getMaterial()->getShader()->use();
//    }
//
//    glm::vec4 color(1, 1, 1, 1);
//
//    // TODO create update() fn and resolve all changes and updates in it, not during the render call
//    auto _tm = getTm();
//
//    // Update model/view/projective matrices in shader
//    glUniformMatrix4fv(shader->getUniformLoc("view"), 1, GL_FALSE, (const GLfloat*)glm::value_ptr(view));
//    glUniformMatrix4fv(shader->getUniformLoc("projection"), 1, GL_FALSE, (const GLfloat*)glm::value_ptr(proj));
//    glUniformMatrix4fv(shader->getUniformLoc("model"), 1, GL_FALSE, (const GLfloat*)glm::value_ptr(_tm));
//    if (shader->hasUniform("color")) {
//      glUniform4fv(shader->getUniformLoc("color"), 1, (const GLfloat*)glm::value_ptr(color));
//    }
//    if (shader->hasUniform("borderRadius")) {
//      glUniform1f(shader->getUniformLoc("borderRadius"), technique.borderRadius);
//    }
//    if (shader->hasUniform("borderColor")) {
//      glUniform4fv(shader->getUniformLoc("borderColor"), 1, (const GLfloat*)glm::value_ptr(technique.borderColor));
//    }
//
//    auto mesh = obj->getMesh();
//#if _DEBUG
//    if (mesh->vao == 0) {
//      std::cerr << "VAO == 0 in object " << obj->getName() << std::endl;
//      abort();
//    }
//#endif
//    glBindVertexArray(mesh->vao); // Bind Sphere VAO
//
//    if (technique.primitiveType == RenderTechnique::DrawPrimitiveType::TRIANGLES) {
//      glDrawElements(GL_TRIANGLES, (GLsizei)mesh->indices.size(), GL_UNSIGNED_INT, NULL);
//    }
//    else if (technique.primitiveType == RenderTechnique::DrawPrimitiveType::POINTS) {
//      glEnable(GL_PROGRAM_POINT_SIZE);
//      glPointSize(technique.pointSize);
//      if (mesh->vib) {
//        glDrawElements(GL_POINTS, (GLsizei)mesh->indices.size(), GL_UNSIGNED_INT, NULL);
//      }
//      else {
//        abort();
//        // NOT Implemented
//        glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(mesh->data.size()));
//      }
//      glDisable(GL_PROGRAM_POINT_SIZE);
//    }
//    glBindVertexArray(0);
//  }

  const ObjectInstance* ObjectInstance::getParent()
  {
    return parent.get();
  }

  // TODO: optimize me, make this lazy
  glm::mat4 ObjectInstance::getTm()
  {
    auto parentTm = glm::mat4(1);
    if (parent) {
      parentTm = parent->getTm();
    } else {
      // No need to update, tm is clean
      if (tmDirty == false) {
        return tm;
      }
    }

    auto rot = glm::toMat4(localPRS.localRot);
    auto trans = glm::translate(glm::mat4(1), localPRS.localPos);
    auto scale = glm::scale(glm::mat4(1), localPRS.localScale);
    auto _tm = trans * rot * scale;

    tm = parentTm * _tm;
    tmDirty = false;

    return tm;
  }

  void ObjectInstance::setParent(std::shared_ptr<ObjectInstance>& oi)
  {
    parent = oi;
    tmDirty = true;
    return;
  }

  agt3d::BoundingSphere ObjectInstance::getBoundingSphere()
  {
    MY_ASSERT(isRenderable(), "OI not renderable");
/*
    agt3d::BoundingSphere bs;
    bs = getObject()->getMesh()->getBoundingSphere();

    glm::vec3 scale;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 skew;
    glm::vec4 perspective;
    glm::decompose(getTm(), scale, rotation, translation, skew, perspective);
    bs.center += translation;
    // Kind of a fast hack, but we are interested in the major component (absolute value) of the (non) uniform scaling vector   
    bs.radius *= std::max(std::max(fabs(localPRS.localScale.x), fabs(localPRS.localScale.y)), fabs(localPRS.localScale.z));
*/

    agt3d::BoundingSphere bs;

    const auto& aabb = getObject()->getMesh()->getAABB();
    auto tm = getTm();
    glm::vec3 min = tm * glm::vec4(aabb.min, 1.0f);
    glm::vec3 max = tm * glm::vec4(aabb.max, 1.0f);
    float d = glm::distance(min, max);
    bs.radius = d / 2.0f;
    bs.center = (min + max) / 2.0f;

    return bs;
  }

  bool ObjectInstance::isRenderable()
  {
    return obj != nullptr;
  }

  void ObjectInstance::setRenderTechnique(const RenderTechnique& tech)
  {
    technique = RenderTechnique(tech);
    return;
  }

  const RenderTechnique& ObjectInstance::getRenderTechnique()
  {
    return technique;
  }

  void ObjectInstance::setEnabled(bool ena) 
  {
    enabled = ena;
    return;
  }

  bool ObjectInstance::isEnabled() 
  {
    return enabled;
  }

  agt3d::Object* ObjectInstance::getObject()
  {
    return obj.get();
  }

}