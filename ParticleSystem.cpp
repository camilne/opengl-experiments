#include "ParticleSystem.h"
#include <glm/gtx/norm.hpp>
#include <iostream>

ParticleSystem::ParticleSystem(const unsigned int& particlesPerSecond, const std::string& textureName, const std::string& shaderName)
  : particlesPerSecond(particlesPerSecond), texture(textureName), shader(shaderName), lastUsedParticle(0), particleCount(0) {
  particles = std::make_unique<std::array<Particle, MAX_PARTICLES>>();

  glGenBuffers(1, &particlesPositionSizeBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, particlesPositionSizeBuffer);
  glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), nullptr, GL_DYNAMIC_DRAW);

  glGenBuffers(1, &particlesColorBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, particlesColorBuffer);
  glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), nullptr, GL_DYNAMIC_DRAW);

  glGenVertexArrays(1, &quadVAO);
  glBindVertexArray(quadVAO);

  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, particlesPositionSizeBuffer);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, nullptr);

  glEnableVertexAttribArray(1);
  glBindBuffer(GL_ARRAY_BUFFER, particlesColorBuffer);
  glVertexAttribPointer(1, 4, GL_UNSIGNED_BYTE, GL_TRUE, 0, nullptr);

  glBindVertexArray(0);
}

void ParticleSystem::addNewParticles(const double& deltaTime) {
  auto newParticles = static_cast<int>(deltaTime * particlesPerSecond);
  if (newParticles > static_cast<int>(0.016f * particlesPerSecond)) {
    newParticles = static_cast<int>(0.016f * particlesPerSecond);
  }

  for(auto i = 0; i < newParticles; i++) {
    Particle p;
    p.position = glm::vec3(0, 0, -0.1);
    p.velocity = glm::vec3(static_cast<double>(rand()) / RAND_MAX - 0.5, 5 + static_cast<double>(rand()) / RAND_MAX, static_cast<double>(rand()) / RAND_MAX - 0.5);
    p.r = static_cast<GLubyte>((static_cast<float>(rand()) / RAND_MAX) * 255);
    p.g = static_cast<GLubyte>((static_cast<float>(rand()) / RAND_MAX) * 255);
    p.b = static_cast<GLubyte>((static_cast<float>(rand()) / RAND_MAX) * 255);
    p.a = static_cast<GLubyte>(255 * 0.5f);
    p.size = 0.1f;
    p.life = 3; // 3000 ms
    p.cameraDistance = 0.0f;

    (*particles)[findUnusedParticle()] = p;
  }
}

unsigned int ParticleSystem::findUnusedParticle() {
  for(auto i = lastUsedParticle; i < MAX_PARTICLES; i++) {
    if((*particles)[i].life <= 0.0f) {
      lastUsedParticle = i;
      return i;
    }
  }

  for(unsigned int i = 0; i < lastUsedParticle; i++) {
    if((*particles)[i].life <= 0.0f) {
      lastUsedParticle = i;
      return i;
    }
  }

  return 0;
}

void ParticleSystem::update(const double& deltaTime, const glm::vec3& cameraPosition) {
  addNewParticles(deltaTime);
  //std::sort(begin(*particles), end(*particles));

  auto particlePositionSizeData = std::make_unique<std::array<GLfloat, MAX_PARTICLES * 4>>();
  auto particleColorData = std::make_unique<std::array<GLubyte, MAX_PARTICLES * 4>>();

  particleCount = 0;
  for(unsigned int i = 0; i < MAX_PARTICLES; i++) {
    auto& p = (*particles)[i];

    if (p.life > 0.0f) {

      p.life -= static_cast<float>(deltaTime);
      if (p.life > 0.0f) {
        p.velocity += glm::vec3(0, -9.81f, 0) * static_cast<float>(deltaTime) * 0.5f;
        p.position += p.velocity * static_cast<float>(deltaTime);
        p.cameraDistance = glm::length2(p.position - cameraPosition);

        (*particlePositionSizeData)[4 * particleCount + 0] = p.position.x;
        (*particlePositionSizeData)[4 * particleCount + 1] = p.position.y;
        (*particlePositionSizeData)[4 * particleCount + 2] = p.position.z;

        (*particlePositionSizeData)[4 * particleCount + 3] = p.size;

        (*particleColorData)[4 * particleCount + 0] = p.r;
        (*particleColorData)[4 * particleCount + 1] = p.g;
        (*particleColorData)[4 * particleCount + 2] = p.b;
        (*particleColorData)[4 * particleCount + 3] = p.a;
      }
      else {
        p.cameraDistance = -1.0f;
      }

      particleCount++;
    }
  }

  glBindVertexArray(quadVAO);
  glBindBuffer(GL_ARRAY_BUFFER, particlesPositionSizeBuffer);
  //glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), nullptr, GL_STREAM_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * 4 * sizeof(GLfloat), &particlePositionSizeData->front());

  glBindBuffer(GL_ARRAY_BUFFER, particlesColorBuffer);
  //glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLubyte), nullptr, GL_STREAM_DRAW);
  glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * 4 * sizeof(GLubyte), &particleColorData->front());
  glBindVertexArray(0);
}

void ParticleSystem::render(const glm::mat4& modelView, const glm::mat4& projection) {
  shader.bind();
  shader.setUniform("MV", modelView);
  shader.setUniform("proj", projection);
  texture.bind();

  glDepthMask(GL_FALSE);
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_POINTS, 0, particleCount);
  //std::cout << "Count " << particleCount << std::endl;
  //std::cout << "Particle position " << (*particles)[0].position.y << " velocity " << (*particles)[0].velocity.y << std::endl;
  glBindVertexArray(0);
  glDepthMask(GL_TRUE);
}
