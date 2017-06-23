#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <vector>
#include "Vertex.h"
#include "Texture.h"

class Mesh {
private:
  struct MeshData {
    std::vector<Vertex> vertices;
    std::vector<Index> indices;
    std::vector<Texture> textures;
  };

  GLuint vbo;
  GLuint ibo;
  glm::mat4 transform;
  size_t size;
  MeshData data;

  explicit Mesh(const MeshData& data);
public:
  static Mesh createMesh(const std::vector<Vertex>& vertices, const std::vector<Index>& indices, const std::vector<Texture>& texures);
  static Mesh createMesh(const std::vector<glm::vec3>& positions, const std::vector<Index>& indices);
  static Mesh createRect(const float& width, const float& height);

  void render() const;

  const glm::mat4& getTransform() const {
    return transform;
  }
};
