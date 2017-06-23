#include "Window.h"
#include <sstream>
#include <iomanip>
#include <numeric>

auto Window::createWindow(const GLuint& width, const GLuint& height, const std::string& title) -> GLFWwindow* {
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

  // MSAA 4x multisampling
  glfwWindowHint(GLFW_SAMPLES, 4);
  glEnable(GL_MULTISAMPLE);

  // Enable v-sync
  glfwSwapInterval(1);

  auto result = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

  if(!result) {
    glfwTerminate();
    throw std::runtime_error("Failed to create window");
  }

  return result;
}

void Window::makeContextCurrent() const {
  glfwMakeContextCurrent(handle);

  static auto isGlewInit = false;
  if(!isGlewInit) {
    if(GLEW_OK != glewInit()) {
      glfwTerminate();
      throw std::runtime_error("Failed to initialize glew");
    }
    isGlewInit = true;
  }
}

void Window::swapBuffers() const {
  glfwSwapBuffers(handle);
}

void Window::update() {
  // Calculate current fps
  auto newfps = 1.0 / getDeltaTime();
  while(fpsQueue.size() >= SAMPLE_SIZE) {
    fpsQueue.pop_back();
  }
  fpsQueue.push_front(newfps);

  // Calculate average fps
  auto fps = std::accumulate(begin(fpsQueue), end(fpsQueue), 0.0) / static_cast<double>(fpsQueue.size());

  // Update fps in title
  std::ostringstream ss;
  ss << title << std::setw(5) << " " << "FPS: " << std::fixed << std::setprecision(0) << fps;
  glfwSetWindowTitle(handle, ss.str().c_str());

  // Input to close window
  if(Input::isKeyPressed(GLFW_KEY_ESCAPE)) {
    isShouldClose = true;
  }

  timer.reset();
  swapBuffers();
}

bool Window::shouldClose() const {
  return isShouldClose || glfwWindowShouldClose(handle);
}

void Window::destroy() const {
  glfwDestroyWindow(handle);
}
