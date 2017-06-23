#include "Camera.h"
#include <glm/gtc/matrix_transform.inl>
#include "Input.h"
#include <iostream>

Camera Camera::createPerspective(const float& fovy, const float& aspect, const float& zNear, const float& zFar) {
  auto proj = glm::perspective(glm::radians(fovy), aspect, zNear, zFar);

  return Camera(proj);
}

Camera& Camera::lookAt(const glm::vec3& center) {
  forward = glm::normalize(center - position);
  update();
  return *this;
}

void Camera::update() {
  view = glm::lookAt(position, position + forward, up);
}

void Camera::processInput(const double& deltaTime) {
  yaw = glm::mod(yaw + Input::getDx(), 360.0);
  pitch -= Input::getDy();

  if (pitch > 89.0)
    pitch = 89.0;
  if (pitch < -89.0)
    pitch = -89.0;

  forward.x = static_cast<float>(cos(glm::radians(yaw)) * cos(glm::radians(pitch)));
  forward.y = static_cast<float>(sin(glm::radians(pitch)));
  forward.z = static_cast<float>(sin(glm::radians(yaw)) * cos(glm::radians(pitch)));

  forward = glm::normalize(forward);

  auto speed = 2.0f * static_cast<float>(deltaTime);

  if(Input::isKeyPressed(GLFW_KEY_W)) {
    position += speed * forward;
  } else if(Input::isKeyPressed(GLFW_KEY_S)) {
    position -= speed * forward;
  }

  if(Input::isKeyPressed(GLFW_KEY_A)) {
    position -= speed * glm::normalize(glm::cross(forward, up));
  } else if(Input::isKeyPressed(GLFW_KEY_D)) {
    position += speed * glm::normalize(glm::cross(forward, up));
  }

  if(Input::isKeyPressed(GLFW_KEY_SPACE)) {
    position += speed * up;
  } else if(Input::isKeyPressed(GLFW_KEY_LEFT_SHIFT)) {
    position -= speed * up;
  }

  update();
}
