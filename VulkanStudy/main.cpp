#include <Windows.h>
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include "WindowManager.h"
#include "VulkanFunction.h"

int main(int argc, char* argv[]) {
#if _DEBUG
  _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
#endif

  GLFWwindow* window;

  if (!glfwInit()) {
    return -1;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window = glfwCreateWindow(1366, 768, "window", nullptr, nullptr);
  if (!window) {
    glfwTerminate();
    return -1;
  }

  if (!glfwVulkanSupported()) {
    return -1;
  }

  initVulkan(window, 1366, 768);

  while (!glfwWindowShouldClose(window)) {
    updateVulkan();

    glfwPollEvents();
  }

  uninitVulkan();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
