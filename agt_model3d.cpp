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

agt3d::Node createPRSFromTransformation(aiMatrix4x4& tm)
{
  glm::mat4 mat = glm::make_mat4(tm[0]);
  glm::vec3 scale, trans, skew;
  glm::quat rot;
  glm::vec4 persp;
  glm::decompose(mat, scale, rot, trans, skew, persp);

  agt3d::Node prs;
  prs.setLocalPosition(trans);
  prs.setLocalScale(scale);
  prs.setLocalRotation(rot);

  // Old way;
  // Set PRS Transform
  // aiVector3D scalev, posv;
  // aiQuaternion rotq;
  // tm.Decompose(scalev, rotq, posv);
  // agt3d::Node prs;
  // prs.setLocalPosition(glm::make_vec3(&posv[0]));
  // prs.setLocalScale(glm::make_vec3(&scalev[0]));
  // rotq = rotq.Conjugate();
  // prs.setLocalRotation(glm::quat(rotq.w, rotq.x, rotq.y, rotq.z));
  return prs;
}

/**
 * @brief Experimental - attempt to fix the fbx axis permutations.. unsuccessful
 * so far.
 * @param scene
 * @return
 */
void fixUpAxisAndScale(aiScene* scene) noexcept
{
  if (scene->mMetaData) {
    for (unsigned int i = 0; i < scene->mMetaData->mNumProperties; ++i) {
      std::cout << scene->mMetaData->mKeys[i].C_Str();
      int val = 0;
      scene->mMetaData->Get<int>(scene->mMetaData->mKeys[i], val);
      std::cout << " : " << val << "\n";
    }

    int coordAxis = 0, coordAxisSign = 1, upAxis = 1, upAxisSign = 1,
        frontAxis = 2, frontAxisSign = 1;

    int originalUpAxis = 1;
    int originalUpAxisSign = 1;
    scene->mMetaData->Get<int>("OriginalUpAxis", originalUpAxis);
    scene->mMetaData->Get<int>("OriginalUpAxisSign", originalUpAxisSign);

    scene->mMetaData->Get<int>("CoordAxis", coordAxis);
    scene->mMetaData->Get<int>("UpAxis", upAxis);
    scene->mMetaData->Get<int>("FrontAxis", frontAxis);

    scene->mMetaData->Get<int>("CoordAxisSign", coordAxisSign);
    scene->mMetaData->Get<int>("UpAxisSign", upAxisSign);
    scene->mMetaData->Get<int>("FrontAxisSign", frontAxisSign);

    // In some of the fbx scenes, originalUpAxis is 2 => it's up and front
    // swapping
    if (originalUpAxis >= 0 && originalUpAxis <= 2) {
      if (originalUpAxis != upAxis) {
        if (originalUpAxis == 0) {
          coordAxis = upAxis;
        } else if (originalUpAxis == 2) {
          frontAxis = upAxis;
        }
        upAxis = originalUpAxis;
      }
    }

    double unitScaleFactor = 1.0;
    scene->mMetaData->Get<double>("unitScaleFactor", unitScaleFactor);

    aiVector3D upVec = upAxis == 0 ? aiVector3D((float)upAxisSign, 0.0f, 0.0f)
                       : upAxis == 1
                         ? aiVector3D(0.0f, (float)upAxisSign, 0.0f)
                         : aiVector3D(0.0f, 0.0f, (float)upAxisSign);
    aiVector3D forwardVec =
      frontAxis == 0   ? aiVector3D((float)frontAxisSign, 0.0f, 0.0f)
      : frontAxis == 1 ? aiVector3D(0.0f, (float)frontAxisSign, 0.0f)
                       : aiVector3D(0.0f, 0.0f, (float)frontAxisSign);
    aiVector3D rightVec =
      coordAxis == 0   ? aiVector3D((float)coordAxisSign, 0.0f, 0.0f)
      : coordAxis == 1 ? aiVector3D(0.0f, (float)coordAxisSign, 0.0f)
                       : aiVector3D(0.0f, 0.0f, (float)coordAxisSign);

    upVec[upAxis] = upAxisSign * (float)unitScaleFactor;
    forwardVec[frontAxis] = frontAxisSign * (float)unitScaleFactor;
    rightVec[coordAxis] = coordAxisSign * (float)unitScaleFactor;

    aiMatrix4x4 mat(rightVec.x, rightVec.y, rightVec.z, 0.0f, upVec.x, upVec.y,
                    upVec.z, 0.0f, forwardVec.x, forwardVec.y, forwardVec.z,
                    0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

    // mat = {
    //   1.0f, 0.0f, 0.0f, 0.0f,
    //   0.0f, 1.0f, 0.0f, 0.0f,
    //   0.0f, 0.0f, 1.0f, 0.0f,
    //   0.0f, 0.0f, 0.0f, 1.0f,
    // };
    scene->mRootNode->mTransformation = mat;
  }
}

void Model3d::prepare(agt3d::Scene& s)
{
  MY_ASSERT(scene, "scene is NULL. You have to load it from the disk first.")

  // Experimental - don't use in production
  // fixUpAxisAndScale(getScene());

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

  auto prs = createPRSFromTransformation(scene->mRootNode->mTransformation);
  root->setLocalPRS(prs);

  s.addObjectInstance(root);
  processNode(scene->mRootNode, s, root);

  return;
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

#endif  // USE_ASSIMP
