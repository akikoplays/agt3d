#include "agt_scene.h"
#include "agt_object_instance.h"
#include "agt_stdafx.h"

agt3d::Scene::Scene() : name("unnamed")
{
#ifdef VERBOSE
  std::cout << "Scene ctor" << std::endl;
#endif
  return;
}

agt3d::Scene::Scene(const std::string& _name) : name(_name)
{
#ifdef VERBOSE
  std::cout << "Scene ctor: " << name << std::endl;
#endif
  return;
}

agt3d::Scene::~Scene()
{
#ifdef VERBOSE
  std::cout << "Scene dtor: " << name << std::endl;
#endif
  return;
}

const std::string& agt3d::Scene::getName() const { return name; }

agt3d::ObjectInstance* agt3d::Scene::getRoot() { return (*ois.begin()).get(); }

void agt3d::Scene::addObjectInstance(std::shared_ptr<agt3d::ObjectInstance>& oi)
{
  // TODO check if already in the scene!
  ois.push_back(oi);
  return;
}

void agt3d::Scene::addMaterial(std::shared_ptr<agt3d::Material>& mat)
{
  materials.push_back(mat);
  return;
}

const std::vector<std::shared_ptr<agt3d::Material>>&
agt3d::Scene::getMaterials()
{
  return materials;
}

void agt3d::Scene::addTexture(std::shared_ptr<agt3d::Texture>& tex)
{
  textures.push_back(tex);
  return;
}

const std::vector<std::shared_ptr<agt3d::Texture>>& agt3d::Scene::getTextures()
{
  return textures;
}

agt3d::ObjectInstance* agt3d::Scene::findOiByName(const char* name)
{
  auto it =
    std::find_if(ois.begin(), ois.end(),
                 [name](const std::shared_ptr<agt3d::ObjectInstance>& oi) {
                   return strncmp(name, oi->name.c_str(), strlen(name)) == 0;
                 });
  if (it == ois.end()) {
    return nullptr;
  }
  return (*it).get();
}

agt3d::ObjectInstance* agt3d::Scene::findOiByPointer(
  const agt3d::ObjectInstance* oi)
{
  auto it = std::find_if(ois.begin(), ois.end(),
                         [oi](const std::shared_ptr<agt3d::ObjectInstance>& a) {
                           return oi == a.get();
                         });
  if (it == ois.end()) {
    return nullptr;
  }
  return (*it).get();
}

void agt3d::Scene::removeObjectInstance(const agt3d::ObjectInstance* oi)
{
  ois.erase(std::remove_if(
    ois.begin(), ois.end(),
    [oi](std::shared_ptr<agt3d::ObjectInstance>& a) { return a.get() == oi; }));
  return;
}

agt3d::BoundingSphere agt3d::Scene::calculateBoundingSphere() const
{
  glm::vec3 center = {0, 0, 0};
  int count = 0;
  for (auto oi : ois) {
    if (!oi->isRenderable()) {
      continue;
    }
    auto bs = oi->getBoundingSphere();
    center += bs.center;
    count++;
  }
  center /= (float)count;

  float maxDistSq = 0.0f;
  for (auto oi : ois) {
    if (!oi->isRenderable()) {
      continue;
    }
    auto bs = oi->getBoundingSphere();
    auto vec = bs.center - center;
    // If object is in the 0,0,0, and the center of the scene is 0,0,0
    // then we will end up with inf,inf,inf
    if (glm::length(vec) < std::numeric_limits<float>::epsilon()) {
      vec = {1, 0, 0};
      vec = vec * bs.radius;
    } else {
      vec = vec + glm::normalize(vec) * bs.radius;
    }
    float d = glm::length2(vec);
    if (d > maxDistSq) {
      maxDistSq = d;
    }
  }
  float maxDist = sqrtf(maxDistSq);
  return {center, maxDist};
}
