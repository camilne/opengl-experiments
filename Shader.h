#pragma once
#include <string>
#include <unordered_map>
#include <GL/glew.h>
#include <glm/mat4x4.hpp>
#include <iostream>

class Shader {
  std::string name;
  GLuint id;
  std::unordered_map<std::string, GLuint> uniforms;

  static bool loadAndCompileSource(const GLuint& sid, const std::string& filePath);
  GLuint loadShaders(const std::string& vFilePath, const std::string& fFilePath, const std::string& gFilePath) const;
  void createUniformIfNeeded(const std::string& name);
public:
  static std::string REL_PATH;
  static std::string V_EXT;
  static std::string F_EXT;
  static std::string G_EXT;

  explicit Shader(const std::string& name)
    : name(name) {
    id = loadShaders(REL_PATH + name + V_EXT, REL_PATH + name + F_EXT, REL_PATH + name + G_EXT);
  }

  void bind() const {
    glUseProgram(id);
  }

  void setUniform(const std::string& name, const glm::mat3& val);
  void setUniform(const std::string& name, const glm::mat4& val);
  void setUniform(const std::string& name, const glm::vec3& val);
  void setUniform(const std::string& name, const GLfloat val);
  void setUniform(const std::string& name, const GLint val);
};
