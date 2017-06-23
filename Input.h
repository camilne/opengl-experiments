#pragma once
#include "GLFW/glfw3.h"

class Input {
  static const double SENSITIVITY;

  static GLFWwindow* handle;

  static double dx;
  static double dy;
  static double lastX;
  static double lastY;

  static void mouseCallback(GLFWwindow* handle, double xpos, double ypos);
public:
  static void setWindow(GLFWwindow* handle) {
    Input::handle = handle;
    glfwSetCursorPosCallback(handle, mouseCallback);
  }

  static void update();

  static bool isKeyPressed(const int& keycode) {
    if(handle)
      return glfwGetKey(handle, keycode) == GLFW_PRESS;

    return false;
  }

  static double getDx() {
    return dx * SENSITIVITY;
  }

  static double getDy() {
    return dy * SENSITIVITY;
  }
};
