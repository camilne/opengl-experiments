﻿#include "Model.h"
#include <iostream>
#include "Vertex.h"
#include <assimp/postprocess.h>

void Model::loadModel(const std::string& path) {
  std::cout << "Loading model: " << path << std::endl;

  Assimp::Importer importer;
  const auto scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

  if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
    std::cout << "Error:ASSIMP: " << importer.GetErrorString() << std::endl;
    return;
  }

  directory = path.substr(0, path.find_last_of('/'));
  processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
  for(unsigned int i = 0; i < node->mNumMeshes; i++) {
    auto mesh = scene->mMeshes[node->mMeshes[i]];
    meshes.push_back(processMesh(mesh, scene));
  }

  for(unsigned int i = 0; i < node->mNumChildren; i++) {
    processNode(node->mChildren[i], scene);
  }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) const {
  std::vector<Vertex> vertices;
  std::vector<Index> indices;
  std::vector<Texture> textures;

  for(unsigned int i = 0; i < mesh->mNumVertices; i++) {
    Vertex vertex;

    vertex.position = {
      mesh->mVertices[i].x,
      mesh->mVertices[i].y,
      mesh->mVertices[i].z
    };

    vertex.normal = {
      mesh->mNormals[i].x,
      mesh->mNormals[i].y,
      mesh->mNormals[i].z
    };

    if(mesh->mTextureCoords[0]) {
      vertex.textureCoords = {
        mesh->mTextureCoords[0][i].x,
        mesh->mTextureCoords[0][i].y
      };
    } else {
      vertex.textureCoords = glm::vec2(0.0f, 0.0f);
    }

    vertices.push_back(vertex);
  }

  for(unsigned int i = 0; i < mesh->mNumFaces; i++) {
    auto face = mesh->mFaces[i];
    for(unsigned int j = 0; j < face.mNumIndices; j++) {
      indices.push_back(face.mIndices[j]);
    }
  }

  if (mesh->mMaterialIndex >= 0)
  {
    auto material = scene->mMaterials[mesh->mMaterialIndex];
    auto diffuseMaps = loadMaterialTextures(material,
      aiTextureType_DIFFUSE, "texture_diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    auto specularMaps = loadMaterialTextures(material,
      aiTextureType_SPECULAR, "texture_specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
  }

  return Mesh::createMesh(vertices, indices, textures);
}

std::vector<Texture> Model::loadMaterialTextures(const aiMaterial* mat, aiTextureType type, const std::string& typeName) const {
  std::vector<Texture> textures;
  for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
  {
    aiString str;
    mat->GetTexture(type, i, &str);
    Texture texture(directory + "/" + str.C_Str());
    //texture.id = TextureFromFile(str.C_Str(), directory);
    texture.type = typeName;
    //texture.path = str;
    textures.push_back(texture);
  }
  return textures;
}
