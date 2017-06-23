#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Timer.h"
#include <deque>
#include "Input.h"

class Window {
  GLFWwindow* handle;
  Timer timer;
  
  GLuint width;
  GLuint height;
  std::string title;
  std::deque<double> fpsQueue;
  const unsigned int SAMPLE_SIZE = 60;
  bool isShouldClose;

  static auto createWindow(const GLuint& width, const GLuint& height, const std::string& title) -> GLFWwindow*;
public:
  explicit Window(const GLuint& width, const GLuint& height, const std::string& title)
    : width{ width }, height{ height }, title{ title }, isShouldClose{ false } {
    handle = createWindow(width, height, title);
    Input::setWindow(handle);
    timer.start();

    // Capture mouse
    glfwSetInputMode(handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  }

  void makeContextCurrent() const;
  void swapBuffers() const;
  void update();

  double getDeltaTime() const {
    return timer.getElapsedTimeInSeconds();
  }

  const unsigned int& getWidth() const {
    return width;
  }

  const unsigned int& getHeight() const {
    return height;
  }

  bool shouldClose() const;
  void destroy() const;
};
