#include "Shader.h"
#include <fstream>
#include <iostream>
#include <algorithm>

std::string Shader::REL_PATH = "./";
std::string Shader::V_EXT = ".vs";
std::string Shader::F_EXT = ".fs";
std::string Shader::G_EXT = ".gs";

bool Shader::loadAndCompileSource(const GLuint& sid, const std::string& filePath) {
  std::ifstream fileIn(filePath);
  auto ext = filePath.substr(filePath.find_last_of('.'));
  std::transform(begin(ext), end(ext), begin(ext), tolower);
  if(ext == G_EXT && !fileIn.good()) {
    return false;
  }

  if (fileIn.fail()) {
    throw std::runtime_error("Failed to open : " + filePath);
  }

  std::string source;
  std::string line;
  while (getline(fileIn, line)) {
    source += line + "\n";
  }

  fileIn.close();

  std::cout << "Compiling Shader: " << filePath << std::endl;
  auto c_src = source.c_str();
  glShaderSource(sid, 1, &c_src, nullptr);
  glCompileShader(sid);

  auto result = GL_FALSE;
  glGetShaderiv(sid, GL_COMPILE_STATUS, &result);
  if(result == GL_FALSE) {
    int infoLogLength;
    glGetShaderiv(sid, GL_INFO_LOG_LENGTH, &infoLogLength);

    std::vector<char> errorMessage(infoLogLength + 1);
    glGetShaderInfoLog(sid, infoLogLength, nullptr, &errorMessage[0]);
    std::cout << std::string(begin(errorMessage), end(errorMessage)) << std::endl;
  }

  return true;
}

GLuint Shader::loadShaders(const std::string& vFilePath, const std::string& fFilePath, const std::string& gFilePath) const {
  auto vid = glCreateShader(GL_VERTEX_SHADER);
  auto fid = glCreateShader(GL_FRAGMENT_SHADER);
  auto gid = glCreateShader(GL_GEOMETRY_SHADER);

  loadAndCompileSource(vid, vFilePath);
  loadAndCompileSource(fid, fFilePath);
  auto gExists = loadAndCompileSource(gid, gFilePath);

#ifdef _DEBUG
  std::cout << "Linking Program: " << this->name << std::endl;
#endif

  auto pid = glCreateProgram();
  glAttachShader(pid, vid);
  glAttachShader(pid, fid);
  if (gExists) glAttachShader(pid, gid);
  glLinkProgram(pid);

  auto result = GL_FALSE;
  glGetProgramiv(pid, GL_LINK_STATUS, &result);
  if (result == GL_FALSE) {
    int infoLogLength;
    glGetProgramiv(pid, GL_INFO_LOG_LENGTH, &infoLogLength);

    std::vector<char> errorMessage(infoLogLength + 1);
    glGetProgramInfoLog(pid, infoLogLength, nullptr, &errorMessage[0]);
    std::cout << std::string(begin(errorMessage), end(errorMessage)) << std::endl;
  }

  glDetachShader(pid, vid);
  glDetachShader(pid, fid);
  if (gExists) glDetachShader(pid, gid);

  glDeleteShader(vid);
  glDeleteShader(fid);
  if (gExists) glDeleteShader(gid);

  return pid;
}

void Shader::createUniformIfNeeded(const std::string& name) {
  if (uniforms.find(name) == uniforms.end()) {
#ifdef _DEBUG
    std::cout << "Creating uniform in [" << this->name << "]: " << name << std::endl;
#endif

    auto location = glGetUniformLocation(id, name.c_str());
    if (location < 0) {
      std::cout << "Error locating uniform " << name << " in " << this->name << " shader." << std::endl;
    }

    uniforms[name] = location;
  }
}

void Shader::setUniform(const std::string& name, const glm::mat3& val) {
  createUniformIfNeeded(name);
  glUniformMatrix3fv(uniforms[name], 1, GL_FALSE, &val[0][0]);
}

void Shader::setUniform(const std::string& name, const glm::mat4& val) {
  createUniformIfNeeded(name);
  glUniformMatrix4fv(uniforms[name], 1, GL_FALSE, &val[0][0]);
}

void Shader::setUniform(const std::string& name, const glm::vec3& val) {
  createUniformIfNeeded(name);
  glUniform3f(uniforms[name], val.x, val.y, val.z);
}

void Shader::setUniform(const std::string& name, const GLfloat val) {
  createUniformIfNeeded(name);
  glUniform1f(uniforms[name], val);
}

void Shader::setUniform(const std::string& name, const GLint val) {
  createUniformIfNeeded(name);
  glUniform1i(uniforms[name], val);
}
