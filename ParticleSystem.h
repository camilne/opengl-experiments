#pragma once
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <array>
#include "Texture.h"
#include "Shader.h"
#include <memory>

class ParticleSystem {
  struct Particle {
    glm::vec3 position;
    glm::vec3 velocity;
    GLubyte r, g, b, a;
    float size;
    float life;
    float cameraDistance;

    bool operator<(Particle& other) const {
      return this->cameraDistance > other.cameraDistance;
    }
  };

  static const unsigned int MAX_PARTICLES = 50000;
  unsigned int particlesPerSecond;
  GLuint particlesPositionSizeBuffer;
  GLuint particlesColorBuffer;
  GLuint quadVAO;
  Texture texture;
  Shader shader;
  unsigned int lastUsedParticle;
  unsigned int particleCount;
  std::unique_ptr<std::array<Particle, MAX_PARTICLES>> particles;

  void addNewParticles(const double& deltaTime);
  unsigned int findUnusedParticle();
public:
  explicit ParticleSystem(const unsigned int& particlesPerSecond, const std::string& textureName, const std::string& shaderName);

  void update(const double& deltaTime, const glm::vec3& cameraPosition);
  void ParticleSystem::render(const glm::mat4& modelView, const glm::mat4& projection);
};