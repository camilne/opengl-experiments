#include "Input.h"

const double Input::SENSITIVITY = 0.05;

GLFWwindow* Input::handle = nullptr;

double Input::dx = 0.0;
double Input::dy = 0.0;

void Input::mouseCallback(GLFWwindow* handle, double xpos, double ypos) {
  static auto lastX = xpos;
  static auto lastY = ypos;

  dx = xpos - lastX;
  dy = ypos - lastY;

  lastX = xpos;
  lastY = ypos;
}

void Input::update() {
  dx = 0.0;
  dy = 0.0;
}
