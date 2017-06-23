#include "Vertex.h"

GLuint Vertex::getSize(const unsigned int& location) {
  switch (location) {
  case 0:
    return 3;
  case 1:
    return 3;
  case 2:
    return 2;
  case 3:
    return 3;
  default:
    return 0;
  }
}

GLvoid* Vertex::getOffset(const unsigned int& location) {
  GLuint offset = 0;

  for (unsigned int i = 0; i < location; i++) {
    offset += getSize(i) * sizeof(float);
  }

  return reinterpret_cast<GLvoid*>(offset);
}