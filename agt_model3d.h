#pragma once

#include "assimp/cimport.h"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "assimp/Importer.hpp"

#include "agt_scene.h"
#include "agt_mesh.h"

namespace agt3d
{

class Model3d
{
 public:
  enum class ErrorCode {
    OK = 0,
    FAILED_TO_LOAD,
  };

  Model3d(const std::string& fullpath);
  ~Model3d();
  /**
   * @brief Get pointer to the Assimp scene object.
   * @return
   */
  aiScene* getScene();
  std::shared_ptr<agt3d::Material>& getMaterialById(int id);
  /**
   * @brief Import the assimp scene into agt3d::Scene object.
   * @param s agt3d::Scene object that will contain geometries imported from the
   * assimp scene.
   */
  void prepare(agt3d::Scene& s);
  Model3d::ErrorCode getError();

 private:
  std::vector<std::shared_ptr<agt3d::Texture>> loadMaterialTextures(
    aiMaterial* mat, aiTextureType type, const std::string& typeName);
  void processNode(aiNode* node, agt3d::Scene& s,
                   std::shared_ptr<agt3d::ObjectInstance>& parent);
  std::shared_ptr<agt3d::Mesh> processMesh(aiMesh* aiMesh);
  bool loadFromDisk();

 private:
  Assimp::Importer imp;
  aiScene* scene = nullptr;
  std::vector<std::shared_ptr<agt3d::Material>> materials;
  std::vector<std::shared_ptr<agt3d::Texture>> texturesLoaded;
  Model3d::ErrorCode error = Model3d::ErrorCode::OK;
  const std::string fullPath;
  const std::string pathWithoutFilename;
  const std::string filenameWithExtension;
};
}  // namespace agt3d
