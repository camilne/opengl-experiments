#include "Texture.h"
#include <soil/SOIL.h>
#include <iostream>

std::string Texture::REL_PATH = "./";

Texture::Texture(const std::string& fileName){
  glGenTextures(1, &id);
  glBindTexture(GL_TEXTURE_2D, id);

  std::cout << "Loading texture: " << (REL_PATH + fileName).c_str() << std::endl;

  // Load image using SOIL, forcing RGBA
  int channels;
  auto image = SOIL_load_image((REL_PATH + fileName).c_str(), &width, &height, &channels, SOIL_LOAD_RGBA);

  // Upload image to opengl
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
  SOIL_free_image_data(image);

  // Generate mipmaps
  glGenerateMipmap(GL_TEXTURE_2D);

  // Set texture wrapping
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

  // Set texture scaling
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void Texture::bind() const {
  glBindTexture(GL_TEXTURE_2D, id);
}
