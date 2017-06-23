#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>

using Index = GLuint;

struct Vertex {
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 textureCoords;
  glm::vec3 color;

  static GLuint getSize(const unsigned int& location);

  // TODO: redo using offsetof
  static GLvoid* getOffset(const unsigned int& location);
};
