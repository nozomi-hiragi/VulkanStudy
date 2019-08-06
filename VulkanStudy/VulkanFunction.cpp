
#include "VulkanFunction.h"
#include <vector>
#include <iostream>

#include <glm/glm.hpp>

#include "Renderer.h"

const char* const APP_NAME = "VulkanStudy";
const uint32_t APP_VERSION = 0;

Renderer _renderer;

MeshStatus nl;
std::shared_ptr<MeshStatus> aaa;

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

  aaa = _renderer.createMesh(pos, nor, col, tex, idx);
}
int a = 0;
void updateVulkan() {
  _renderer.update();
  if (a++ > 200) {
    aaa->clear();
  }
}

void uninitVulkan() {
  _renderer.uninit();
}
