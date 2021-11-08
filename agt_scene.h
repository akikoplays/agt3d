#pragma once

#include "agt_AABB.h"

namespace agt3d
{

class Texture;
class Material;
class ObjectInstance;

class Scene
{
 public:
  Scene();
  Scene(const std::string& _name);
  ~Scene();
  Scene& operator=(const Scene& other) = delete;
  Scene(Scene&) = delete;
  const std::string& getName() const;
  ObjectInstance* getRoot();
  void addObjectInstance(std::shared_ptr<agt3d::ObjectInstance>& oi);
  void addMaterial(std::shared_ptr<agt3d::Material>& mat);
  const std::vector<std::shared_ptr<agt3d::Material>>& getMaterials();
  void addTexture(std::shared_ptr<agt3d::Texture>& tex);
  const std::vector<std::shared_ptr<agt3d::Texture>>& getTextures();
  agt3d::ObjectInstance* findOiByName(const char* name);
  void removeObjectInstance(const agt3d::ObjectInstance* oi);
  /**
   * @brief Locate ObjectInstance by a pointer. Fast.
   */
  agt3d::ObjectInstance* findOiByPointer(const agt3d::ObjectInstance* oi);
  /**
   * @brief Return bounding sphere of the whole scene. This is a bit expensive,
   * and applies translate and scale to individual object bounding spheres.
   * @return bounding sphere object.
   */
  agt3d::BoundingSphere calculateBoundingSphere() const;

 public:
  std::vector<std::shared_ptr<agt3d::ObjectInstance>> ois;
  std::vector<std::shared_ptr<agt3d::Material>> materials;
  std::vector<std::shared_ptr<agt3d::Texture>> textures;

 private:
  std::string name;
};
}  // namespace agt3d
