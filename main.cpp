#include "Window.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "Shader.h"
#include "Mesh.h"
#include "Texture.h"
#include <iostream>
#include "Timer.h"
#include "ParticleSystem.h"
#include "Camera.h"
#include "Model.h"
#include <glm/gtc/matrix_transform.inl>

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
  if (quadVAO == 0)
  {
    float quadVertices[] = {
      // positions        // texture Coords
      0.75f,  1.0f, 0.0f, 0.0f, 1.0f,
      0.75f, 0.75f, 0.0f, 0.0f, 0.0f,
      1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
      1.0f, 0.75f, 0.0f, 1.0f, 0.0f,
    };
    // setup plane VAO
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
  }
  glBindVertexArray(quadVAO);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void renderScene(GLuint& vao, Shader& shader, glm::mat4& catTransform, Texture& cat, Mesh& mesh,
  glm::mat4& woodTransform, Texture& woodTexture, Mesh& wood,
  glm::mat4& modelTransform, Model& testModel);

int main()
{
  srand(static_cast<unsigned int>(time(nullptr)));

  if (!glfwInit())
    return -1;

  Window window{ 1280, 720, "Hello World" };
  window.makeContextCurrent();

  // Enable depth testing
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  // Enable alpha blending
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glBlendEquation(GL_FUNC_ADD);

  // Enable face culling
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);

  // Enable gamma correction
  //glEnable(GL_FRAMEBUFFER_SRGB);

  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  Model testModel("./nanosuit/nanosuit.obj");
  auto modelTransform = glm::translate(glm::scale(glm::mat4(), glm::vec3(0.15f, 0.15f, 0.15f)), glm::vec3(-5.0f, 0.0f, -3.0f));

  auto camera = Camera::createPerspective(45.0f, 1280.0f / 720.0f, 0.1f, 100.0f);
  camera.setPosition(glm::vec3(0, 1, 4));// .lookAt(glm::vec3(0, 8, 0));
  
  Shader shader("shader");
  shader.bind();
  shader.setUniform("model", glm::mat4());
  shader.setUniform("projection", camera.getProjection());
  shader.setUniform("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
  auto lightPos = glm::vec3(-3, 6, 4);
  shader.setUniform("lightPos", lightPos);
  shader.setUniform("texture_diffuse1", 0);
  shader.setUniform("shadowMap", 7);
  //shader.setUniform("norm_trans", glm::mat3(glm::transpose(glm::inverse(glm::mat4()))));

  Texture cat("cat.jpg");
  auto mesh = Mesh::createRect(1, 1);
  auto catTransform = glm::translate(glm::mat4(), glm::vec3(0.0f, 0.5f, 1.0f));

  auto wood = Mesh::createRect(10, 10);
  auto woodTransform = glm::rotate(glm::mat4(), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
  Texture woodTexture("wood/diffuse.tga");

  glClearColor(0, 0, 0.4f, 0);

  ParticleSystem particleSystem(1000, "particle.png", "particle");

  auto deltaTime = window.getDeltaTime();

  // --- Shadow Mapping ---
  GLuint depthMapFBO;
  glGenFramebuffers(1, &depthMapFBO);

  const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

  GLuint depthMap;
  glGenTextures(1, &depthMap);
  glBindTexture(GL_TEXTURE_2D, depthMap);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

  glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
    std::cout << "Incomplete shadow map framebuffer" << std::endl;
  }
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  Shader simpleDepthShader("shadow");

  auto near_plane = 1.0f, far_plane = 10.5f;
  auto lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
  auto lightView = glm::lookAt(lightPos,
    glm::vec3(0.0f, 0.0f, 0.0f),
    glm::vec3(0.0f, 1.0f, 0.0f));
  auto lightSpaceMatrix = lightProjection * lightView;

  Shader debugDepthQuad("debug_quad");
  // --- End Shadow Mapping ---

  while (!window.shouldClose())
  {
    camera.processInput(deltaTime);
    Input::update();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // --- Render Shadow Mapping ---
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    simpleDepthShader.bind();
    simpleDepthShader.setUniform("lightSpaceMatrix", lightSpaceMatrix);

    //glCullFace(GL_FRONT);
    renderScene(vao, simpleDepthShader, catTransform, cat, mesh, woodTransform, woodTexture, wood, modelTransform, testModel);
    //glCullFace(GL_BACK);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // --- End Shadow Mapping ---

    glViewport(0, 0, window.getWidth(), window.getHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.bind();
    shader.setUniform("view", camera.getView());
    shader.setUniform("viewPos", camera.getPosition());
    shader.setUniform("lightSpaceMatrix", lightSpaceMatrix);

    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glActiveTexture(GL_TEXTURE0);

    renderScene(vao, shader, catTransform, cat, mesh, woodTransform, woodTexture, wood, modelTransform, testModel);

    // Render particle system
    glDisable(GL_CULL_FACE);
    particleSystem.update(deltaTime, camera.getPosition());
    particleSystem.render(camera.getView(), camera.getProjection());
    glEnable(GL_CULL_FACE);

    // render Depth map to quad for visual debugging
    // ---------------------------------------------
    debugDepthQuad.bind();
    debugDepthQuad.setUniform("near_plane", near_plane);
    debugDepthQuad.setUniform("far_plane", far_plane);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    renderQuad();

    //std::cout << "DELTA: " << deltaTime << std::endl;

    auto err = glGetError();
    if(err != GL_NO_ERROR) {
      std::cout << "GL_ERROR: " << err << std::endl;
    }

    deltaTime = window.getDeltaTime();
    window.update();
    glfwPollEvents();
  }

  window.destroy();
  glfwTerminate();
  return 0;
}

void renderScene(GLuint& vao, Shader& shader, glm::mat4& catTransform, Texture& cat, Mesh& mesh,
                glm::mat4& woodTransform, Texture& woodTexture, Mesh& wood,
                glm::mat4& modelTransform, Model& testModel) {
  glBindVertexArray(vao);
  shader.bind();

  glDisable(GL_BLEND);
  // Render cat image
  shader.setUniform("model", catTransform);
  //shader.setUniform("norm_trans", glm::mat3(glm::transpose(glm::inverse(catTransform))));
  cat.bind();
  mesh.render();

  // Render floor
  shader.setUniform("model", woodTransform);
  //shader.setUniform("norm_trans", glm::mat3(glm::transpose(glm::inverse(woodTransform))));
  woodTexture.bind();
  wood.render();

  // Render crysis model
  shader.setUniform("model", modelTransform);
  //shader.setUniform("norm_trans", glm::mat3(glm::transpose(glm::inverse(modelTransform))));
  testModel.render();
  glEnable(GL_BLEND);
}