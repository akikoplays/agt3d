#ifdef USE_ASSIMP

#include "agt_model3d.h"
#include "agt_object.h"
#include "agt_object_instance.h"
#include "agt_scene.h"
#include "agt_stdafx.h"
#include "agt_utils.h"

namespace agt3d
{

Model3d::Model3d(const std::string& fullpath)
    : fullPath(fullpath),
      pathWithoutFilename(agt3d::getPath(fullpath)),
      filenameWithExtension(agt3d::getFilenameWithExtension(fullpath))
{
  std::cout << "Model3d constructed " << fullPath << std::endl;
  std::cout << "-- fullpath: " << fullPath << std::endl;
  std::cout << "-- path: " << pathWithoutFilename << std::endl;
  std::cout << "-- filename: " << filenameWithExtension << std::endl;
  if (!loadFromDisk()) {
    error = Model3d::ErrorCode::FAILED_TO_LOAD;
  } else {
    error = Model3d::ErrorCode::OK;
  }
  return;
}

Model3d::~Model3d()
{
  std::cout << "Model3d destroyed, asset path:  "
            << pathWithoutFilename + filenameWithExtension << std::endl;
}

Model3d::ErrorCode Model3d::getError() { return error; }

bool Model3d::loadFromDisk()
{
  scene = const_cast<aiScene*>(
    imp.ReadFile(fullPath, aiProcessPreset_TargetRealtime_Fast));
  if (!scene) {
    std::cerr << imp.GetErrorString() << std::endl;
    return false;
  }
  return true;
}

aiScene* Model3d::getScene() { return scene; }

std::shared_ptr<agt3d::Material>& Model3d::getMaterialById(int id)
{
  return materials[id];
}

void Model3d::prepare(agt3d::Scene& s)
{
  MY_ASSERT(scene, "scene is NULL. You have to load it from the disk first.")
  materials.reserve(getScene()->mNumMaterials);
  for (unsigned int m = 0; m < getScene()->mNumMaterials; m++) {
    auto aimat = getScene()->mMaterials[m];
    auto mat = std::shared_ptr<agt3d::Material>(new agt3d::Material);
    materials.push_back(mat);
    s.addMaterial(mat);
    auto textures =
      loadMaterialTextures(aimat, aiTextureType_DIFFUSE, "tex_diffuse");
    mat->textures.insert(mat->textures.end(), textures.begin(), textures.end());
    textures =
      loadMaterialTextures(aimat, aiTextureType_SPECULAR, "tex_specular");
    mat->textures.insert(mat->textures.end(), textures.begin(), textures.end());
    textures = loadMaterialTextures(aimat, aiTextureType_NORMALS, "tex_normal");
    mat->textures.insert(mat->textures.end(), textures.begin(), textures.end());
  }

  auto root = std::shared_ptr<agt3d::ObjectInstance>(
    new agt3d::ObjectInstance("AGT3DROOT"));
  s.addObjectInstance(root);
  processNode(scene->mRootNode, s, root);

  return;
}

agt3d::Node createPRSFromTransformation(aiMatrix4x4& tm)
{
  // Set PRS Transform
  aiVector3D scalev, posv;
  aiQuaternion rotq;
  tm.Decompose(scalev, rotq, posv);
  agt3d::Node prs;
  prs.setLocalPosition(glm::make_vec3(&posv[0]));
  prs.setLocalScale(glm::make_vec3(&scalev[0]));
  rotq = rotq.Conjugate();
  prs.setLocalRotation(glm::quat(rotq.w, rotq.x, rotq.y, rotq.z));
  return prs;
}

void Model3d::processNode(aiNode* aNode, agt3d::Scene& s,
                          std::shared_ptr<agt3d::ObjectInstance>& parent)
{
  std::cout << "Processing node: " << aNode->mName.C_Str()
            << " parent: " << parent->name << std::endl;

  // Process each mesh located at the current node
  for (unsigned int i = 0; i < aNode->mNumMeshes; i++) {
    // the node object only contains indices to index the actual objects in the
    // scene. the scene contains all the data, node is just to keep stuff
    // organized (like relations between nodes).
    aiMesh* aMesh = scene->mMeshes[aNode->mMeshes[i]];
    std::cout << ".... Processing mesh: " << aMesh->mName.C_Str() << std::endl;
    auto mesh = processMesh(aMesh);
    auto material = materials[aMesh->mMaterialIndex];
    auto obj = std::shared_ptr<agt3d::Object>(
      new agt3d::Object(mesh, material, aMesh->mName.C_Str()));

    // Alloc OI and set parent
    auto oi = std::shared_ptr<agt3d::ObjectInstance>(
      new agt3d::ObjectInstance(aNode->mName.C_Str()));
    oi->setParent(parent);
    // Set PRS Transform
    auto prs = createPRSFromTransformation(aNode->mTransformation);
    oi->setLocalPRS(prs);
    oi->setObject(obj);
    s.addObjectInstance(oi);
  }
  // After we've processed all of the meshes we then recursively process each of
  // the children nodes
  for (unsigned int i = 0; i < aNode->mNumChildren; i++) {
    processNode(aNode->mChildren[i], s, parent);
  }
}

std::shared_ptr<agt3d::Mesh> Model3d::processMesh(aiMesh* aiMesh)
{
  std::vector<glm::vec3> vertices;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> texcoords;
  std::vector<unsigned int> indices;

  for (unsigned int v = 0; v < aiMesh->mNumVertices; v++) {
    glm::vec3 vert;
    vert.x = aiMesh->mVertices[v].x;
    vert.y = aiMesh->mVertices[v].y;
    vert.z = aiMesh->mVertices[v].z;
    vertices.push_back(vert);

    glm::vec3 norm;
    norm.x = aiMesh->mNormals[v].x;
    norm.y = aiMesh->mNormals[v].y;
    norm.z = aiMesh->mNormals[v].z;
    normals.push_back(norm);

    glm::vec2 texcoord;
    if (aiMesh->HasTextureCoords(0)) {
      texcoord.x = aiMesh->mTextureCoords[0][v].x;
      texcoord.y = aiMesh->mTextureCoords[0][v].y;
    } else {
      texcoord = glm::vec2(0);
    }
    texcoords.push_back(texcoord);
  }
  std::cout << "-- " << vertices.size() << " verts " << std::endl;

  for (unsigned int i = 0; i < aiMesh->mNumFaces; i++) {
    const aiFace& face = aiMesh->mFaces[i];
    if (face.mNumIndices != 3) {
      std::cerr << "!!! Only tri faces are supported!" << std::endl;
      return nullptr;
    }
    indices.push_back(face.mIndices[0]);
    indices.push_back(face.mIndices[1]);
    indices.push_back(face.mIndices[2]);
  }

  auto mesh = std::shared_ptr<agt3d::Mesh>(new agt3d::Mesh());
  if (vertices.size()) {
    mesh->setDataBuffer(agt3d::DataStream::VERTEX, (float*)vertices.data(),
                        vertices.size() * sizeof(glm::vec3), GL_FLOAT, 3);
  }
  if (normals.size()) {
    mesh->setDataBuffer(agt3d::DataStream::NORMAL, (float*)normals.data(),
                        normals.size() * sizeof(glm::vec3), GL_FLOAT, 3);
  }
  if (texcoords.size()) {
    mesh->setDataBuffer(agt3d::DataStream::TEXCOORD0, (float*)texcoords.data(),
                        texcoords.size() * sizeof(glm::vec2), GL_FLOAT, 2);
  }
  mesh->setIndices(indices);
  mesh->updateData();
  mesh->updateVAO();

  return mesh;
}

std::vector<std::shared_ptr<agt3d::Texture>> Model3d::loadMaterialTextures(
  aiMaterial* mat, aiTextureType type, const std::string& typeName)
{
  std::cout << "Processing aiMat: " << mat->GetName().C_Str()
            << " for textures of type: " << typeName << std::endl;
  std::vector<std::shared_ptr<agt3d::Texture>> textures;
  for (uint32_t i = 0; i < mat->GetTextureCount(type); i++) {
    aiString str;
    mat->GetTexture(type, i, &str);
    // check if texture was loaded before and if so, continue to next iteration:
    // skip loading a new texture
    bool skip = false;
    for (unsigned int j = 0; j < texturesLoaded.size(); j++) {
      std::cout << str.C_Str() << " vs " << texturesLoaded[j]->getPath()
                << std::endl;
      if (std::strcmp(
            agt3d::getFilenameWithExtension(texturesLoaded[j]->getPath())
              .c_str(),
            str.C_Str()) == 0) {
        textures.push_back(texturesLoaded[j]);
        skip = true;  // a texture with the same filepath has already been
                      // loaded, continue to next one. (optimization)
        std::cout << "Texture already loaded: " << str.C_Str()
                  << " ... skipping ..." << std::endl;
        break;
      }
    }
    if (!skip) {  // if texture hasn't been loaded already, load it
      std::string fullpath = pathWithoutFilename + str.C_Str();
      std::cout << "Texture will be loaded: " << fullpath << std::endl;
      auto texture = std::shared_ptr<agt3d::Texture>(new agt3d::Texture());
      texture->loadFromFile(fullpath);
      assert(texture->isLoaded());
      texture->setType(typeName);
      textures.push_back(texture);
      texturesLoaded.push_back(
        texture);  // store it as texture loaded for entire model, to ensure we
                   // won't unnecesery load duplicate textures.
    }
  }
  return textures;
}
}  // namespace agt3d

#endif // USE_ASSIMP
