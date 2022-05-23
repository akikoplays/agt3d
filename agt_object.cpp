#include "agt_stdafx.h"
#include "agt_object.h"

namespace agt3d {

  Object::Object(std::shared_ptr<agt3d::Mesh>& _mesh, std::shared_ptr<agt3d::Material> _material, const std::string& _name) :
    mesh(_mesh),
    material(_material),
    name(_name)
  {
#ifdef VERBOSE
		std::cout << "Object ctor " << name << std::endl;
#endif
	}

	void Object::setMesh(std::shared_ptr<Mesh>& _mesh)
	{
		mesh = _mesh;
	}

	Mesh* Object::getMesh() noexcept
	{
		return mesh.get();
	}

	const std::string& Object::getName() const noexcept
	{
		return name;
	}

  Material* Object::getMaterial()
  {
    return material.get();
  }

}
