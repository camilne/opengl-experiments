#include "Mesh.h"
#include <algorithm>
#include <sstream>

Mesh::Mesh(const MeshData& data) {
  //std::cout << "MEM_DUMP: " << std::endl;
  //for_each(begin(data.vertices), end(data.vertices), [&](auto& elem)
  //{
  //  std::cout << "\tVERTEX: ";
  //  for(auto i = 0; i < sizeof(Vertex) / sizeof(float); i++) {
  //    std::cout << *(((float*)&elem) + i) << " ";
  //  }
  //  std::cout << std::endl;
  //});

  this->data = data;

  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, data.vertices.size() * sizeof(Vertex), data.vertices.data(), GL_STATIC_DRAW);

  glGenBuffers(1, &ibo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.indices.size() * sizeof(Index), data.indices.data(), GL_STATIC_DRAW);
  size = data.indices.size();

  transform = glm::mat4(1.0f);
}

Mesh Mesh::createMesh(const std::vector<Vertex>& vertices, const std::vector<Index>& indices, const std::vector<Texture>& textures) {
  return Mesh({ vertices, indices, textures });
}

Mesh Mesh::createMesh(const std::vector<glm::vec3>& positions, const std::vector<Index>& indices) {
  std::vector<Vertex> vertices;
  for_each(begin(positions), end(positions), [&](auto& elem)
  {
    Vertex v;
    v.position = elem;

    vertices.emplace_back(v);
  });

  return Mesh({ vertices, indices });
}

Mesh Mesh::createRect(const float& width, const float& height) {
  std::vector<Vertex> vertices = {
    {{-width / 2, -height / 2, 0}, {0, 0, 1}, {0, 0}},
    {{width / 2, -height / 2, 0}, {0, 0, 1}, {1, 0}},
    {{width / 2, height / 2, 0}, {0, 0, 1}, {1, 1}},
    {{-width / 2, height / 2, 0}, {0, 0, 1}, {0, 1}}
  };
  std::vector<Index> indices = {
    0, 1, 2, 2, 3, 0
  };

  return Mesh({ vertices, indices });
}

void Mesh::render() const {
  unsigned int diffuseNr = 1;
  unsigned int specularNr = 1;
  for (unsigned int i = 0; i < data.textures.size(); i++)
  {
    glActiveTexture(GL_TEXTURE0 + i); // activate proper texture unit before binding
                                      // retrieve texture number (the N in diffuse_textureN)
    std::stringstream ss;
    auto name = data.textures[i].type;
    if (name == "texture_diffuse")
      ss << diffuseNr++; // transfer unsigned int to stream
    else if (name == "texture_specular")
      ss << specularNr++; // transfer unsigned int to stream
    auto number = ss.str();

    //shader.setFloat(("material." + name + number).c_str(), i);
    data.textures[i].bind();
  }
  glActiveTexture(GL_TEXTURE0);

  glBindBuffer(GL_ARRAY_BUFFER, vbo);

  for(auto i = 0; i < 4; i++) {
    glEnableVertexAttribArray(i);
    glVertexAttribPointer(i, Vertex::getSize(i), GL_FLOAT, GL_FALSE, sizeof(Vertex), Vertex::getOffset(i));
    //std::cout << "RENDER V: " << i << " size " << Vertex::getSize(i) << " stride " << sizeof(Vertex) << " offset " << Vertex::getOffset(i) << std::endl;
  }

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  glDrawElements(GL_TRIANGLES, size, GL_UNSIGNED_INT, nullptr);
}
