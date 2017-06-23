#pragma once
#include <vector>
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include "Texture.h"

class Model {
  std::vector<Mesh> meshes;
  std::string directory;

  void loadModel(const std::string& path);
  void processNode(aiNode* node, const aiScene* scene);
  Mesh processMesh(aiMesh* mesh, const aiScene* scene) const;
  std::vector<Texture> loadMaterialTextures(const aiMaterial* mat, aiTextureType type, const std::string& typeName) const;
public:
  explicit Model(const std::string& path) {
    loadModel(path);
  }

  void render() const {
    for(auto& mesh : meshes) {
      mesh.render();
    }
  }
};
