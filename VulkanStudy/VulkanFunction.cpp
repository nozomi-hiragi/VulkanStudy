
#include "VulkanFunction.h"
#include <vector>

#include <glm/glm.hpp>

#include "Renderer.h"

const char* const APP_NAME = "VulkanStudy";
const uint32_t APP_VERSION = 0;

Renderer _renderer;

std::shared_ptr<MeshStatus> mesh1;
std::shared_ptr<MeshStatus> mesh2;

void initVulkan(GLFWwindow* window, uint32_t width, uint32_t height) {
  _renderer.init(APP_NAME, APP_VERSION, width, height, window);
  _renderer._camera._position = glm::vec3(0, 0, -10);

  glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      _renderer._camera._position.x -= 0.1f;
    }
    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      _renderer._camera._position.x += 0.1f;
    }
    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      _renderer._camera._position.z -= 0.1f;
    }
    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      _renderer._camera._position.z += 0.1f;
    }
    if (key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      _renderer._camera._position.y -= 0.1f;
    }
    if (key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      _renderer._camera._position.y += 0.1f;
    }

    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      _renderer._camera._rotation.y -= 0.1f;
    }
    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      _renderer._camera._rotation.y += 0.1f;
    }
    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      _renderer._camera._rotation.x += 0.1f;
    }
    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      _renderer._camera._rotation.x -= 0.1f;
    }

  });

  std::vector<glm::vec3> pos = {
  glm::vec3(-1, +1, 0),
  glm::vec3(+1, +1, 0),
  glm::vec3(-1, -1, 0),
  glm::vec3(+1, -1, 0),
  };

  std::vector<glm::vec3> nor = {
    glm::vec3(0, 0, 1),
    glm::vec3(0, 0, 1),
    glm::vec3(0, 0, 1),
    glm::vec3(0, 0, 1),
  };

  std::vector<glm::vec4> col = {
    glm::vec4(1, 0, 0, 1),
    glm::vec4(0, 1, 0, 1),
    glm::vec4(0, 0, 1, 1),
    glm::vec4(1, 1, 1, 1),
  };

  std::vector<glm::vec2> tex = {
    glm::vec2(0, 0),
    glm::vec2(1, 0),
    glm::vec2(0, 1),
    glm::vec2(1, 1),
  };

  std::vector<uint16_t> idx = {
    0, 1, 2,
    3, 2, 1,
  };

  mesh1 = _renderer.createMesh(pos, nor, col, tex, idx);
  mesh1->_scale.x = 2;
  mesh1->_rotation.x = glm::pi<float>()*0.5f;

  mesh2 = _renderer.createMesh(pos, nor, col, tex, idx);
  mesh2->_scale.y = 2;
  mesh2->_rotation.y = glm::pi<float>()*0.5f;
}
int frame = 0;
void updateVulkan() {
  frame++;


  mesh2->_position.x = sin(frame * 0.01f);
  mesh2->_rotation.y = cos(frame * 0.05f);
  _renderer.update();
}

void uninitVulkan() {
  _renderer.uninit();
}
