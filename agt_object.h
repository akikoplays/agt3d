#pragma once

#include "agt_material.h"
#include "agt_mesh.h"

namespace agt3d
{

class Object
{
 public:
  Object(std::shared_ptr<agt3d::Mesh>& _mesh,
         std::shared_ptr<agt3d::Material> _material, const std::string& _name);
  ~Object();
  void setMesh(std::shared_ptr<Mesh>& _mesh);
  Mesh* getMesh() noexcept;
  const std::string& getName() const noexcept;
  Material* getMaterial();

 private:
  std::string name;
  std::shared_ptr<Mesh> mesh = nullptr;
  std::shared_ptr<Material> material = nullptr;
};

}  // namespace agt3d
