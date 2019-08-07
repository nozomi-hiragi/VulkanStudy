
#include "VulkanFunction.h"
#include <vector>

#include <glm/glm.hpp>

#include "Renderer.h"

const char* const APP_NAME = "VulkanStudy";
const uint32_t APP_VERSION = 0;

Renderer _renderer;

std::shared_ptr<MeshStatus> mesh;

void initVulkan(HINSTANCE hinstance, HWND hwnd, uint32_t width, uint32_t height) {
  _renderer.init(APP_NAME, APP_VERSION, width, height, hinstance, hwnd);

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

  mesh = _renderer.createMesh(pos, nor, col, tex, idx);
  mesh->_position.x = 1;
  mesh->_rotation.y = 1;
  mesh->_scale.x = 2;
}
int frame = 0;
void updateVulkan() {
  frame++;
  //if (frame > 200) {
  //  mesh->clear();
  //}
  mesh->_position.x = cos(frame * 0.01f);
  mesh->_rotation.y = sin(frame * 0.05f);
  _renderer.update();
}

void uninitVulkan() {
  _renderer.uninit();
}
