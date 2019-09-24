
#include "VulkanFunction.h"
#include <vector>

#include <glm/glm.hpp>

#include "Renderer.h"
#include "PosRotCamera.h"

const char* const APP_NAME = "VulkanStudy";
const uint32_t APP_VERSION = 0;

Renderer _renderer;

std::shared_ptr<MeshStatus> mesh1;
std::shared_ptr<MeshStatus> mesh2;

std::shared_ptr<ShaderModuleObject> vs;
std::shared_ptr<ShaderModuleObject> ps;

RenderPassBorrowed render_pass;

PosRotCamera camera;

void initVulkan(GLFWwindow* window, uint32_t width, uint32_t height) {
  camera._width = static_cast<float>(width);
  camera._height = static_cast<float>(height);
  _renderer.init(APP_NAME, APP_VERSION, width, height, window);
  _renderer.createUniformBuffer();
  _renderer.updateDescriptorSet();
  render_pass = _renderer.createRenderPass();

  std::string vs_code =
    "#version 450\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#extension GL_ARB_shading_language_420pack : enable\n"
    "layout (binding = 0) uniform bufferVals {\n"
    "    mat4 mvp;\n"
    "} myBufferVals;\n"
    "layout (location = 0) in vec3 pos;\n"
    "layout (location = 1) in vec3 nor;\n"
    "layout (location = 2) in vec4 inColor;\n"
    "layout (location = 3) in vec2 tex;\n"
    "\n"
    "layout (location = 0) out vec4 outColor;\n"
    "layout (location = 1) out vec2 outUv;\n"
    "void main() {\n"
    "   outColor = vec4(inColor.x,inColor.y,inColor.z, inColor.w);//inColor;\n"
    "   outUv = tex;\n"
    "   gl_Position = myBufferVals.mvp * vec4(pos.xyz, 1);\n"
    "}\n"
    ;

  std::string ps_code =
    "#version 450\n"
    "#extension GL_ARB_separate_shader_objects : enable\n"
    "#extension GL_ARB_shading_language_420pack : enable\n"
    "layout(location = 0) in vec4 color;\n"
    "layout(location = 1) in vec2 uv;\n"
    "layout(binding = 1) uniform sampler2D samColor;\n"
    "\n"
    "layout(location = 0) out vec4 outColor;\n"
    "void main() {\n"
    "    outColor = texture(samColor, uv, 0) * color;\n"
    "}\n"
    ;

  vs = _renderer.createShaderModule(vs_code, "vs_code", VK_SHADER_STAGE_VERTEX_BIT);
  ps = _renderer.createShaderModule(ps_code, "ps_code", VK_SHADER_STAGE_FRAGMENT_BIT);

  _renderer.createPipeline(vs, ps, render_pass->getObject());
  _renderer.createSwapchainFrameBuffer(render_pass->getObject());

  camera._position = glm::vec3(0, 0, -10);

  glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      camera._position.x -= 0.1f;
    }
    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      camera._position.x += 0.1f;
    }
    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      camera._position.z -= 0.1f;
    }
    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      camera._position.z += 0.1f;
    }
    if (key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      camera._position.y -= 0.1f;
    }
    if (key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      camera._position.y += 0.1f;
    }

    if (key == GLFW_KEY_LEFT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      camera._rotation.y -= 0.1f;
    }
    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      camera._rotation.y += 0.1f;
    }
    if (key == GLFW_KEY_DOWN && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      camera._rotation.x += 0.1f;
    }
    if (key == GLFW_KEY_UP && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
      camera._rotation.x -= 0.1f;
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

  camera.update();
  _renderer.beginCommand();
  _renderer.beginRenderPass(render_pass->getObject());
  _renderer.bindPipeline();
  _renderer.update(camera.getViewProjection());
  _renderer.endRenderPass();
  _renderer.endCommand();
}

void uninitVulkan() {
  _renderer.destroyShaderModule(vs);
  _renderer.destroyShaderModule(ps);
  render_pass.reset();
  _renderer.uninit();
}
