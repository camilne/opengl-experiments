#pragma once
#include <GL/glew.h>
#include <string>

class Texture {
  GLuint id;
  GLint width;
  GLint height;

  static std::string REL_PATH;
public:
  std::string type;

  explicit Texture(const std::string& fileName);

  void bind() const;
};
