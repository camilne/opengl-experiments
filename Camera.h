#pragma once

#include <glm/glm.hpp>

class Camera {
  glm::mat4 view;
  glm::mat4 projection;

  glm::vec3 position;
  glm::vec3 up;
  glm::vec3 forward;

  double yaw;
  double pitch;

  explicit Camera(glm::mat4 projection)
    : view(glm::mat4()), projection(projection), position(0.0f), up(0.0f, 1.0f, 0.0f), forward(0.0f, 0.0f, -1.0f), yaw(-90.0), pitch(0.0)
  {}
public:
  static Camera Camera::createPerspective(const float& fovy, const float& aspect, const float& zNear, const float& zFar);

  Camera& lookAt(const glm::vec3& center);

  void update();
  void processInput(const double& deltaTime);

  Camera& setPosition(const glm::vec3& position) {
    this->position = position;
    return *this;
  }

  glm::vec3 getPosition() const {
    return position;
  }

  glm::mat4 getView() const {
    return view;
  }

  glm::mat4 getProjection() const {
    return projection;
  }
};
