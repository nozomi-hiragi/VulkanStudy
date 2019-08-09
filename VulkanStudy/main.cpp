#include <Windows.h>
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>

#pragma comment(lib,"winmm.lib")

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

  LARGE_INTEGER time_frequency;
  LARGE_INTEGER time_last;
  LARGE_INTEGER time_current;
  QueryPerformanceFrequency(&time_frequency);
  QueryPerformanceCounter(&time_last);

  double delta;
  constexpr double frame_time = 1. / 60;
  timeBeginPeriod(1);
  while (!glfwWindowShouldClose(window)) {
    QueryPerformanceCounter(&time_current);
    delta = static_cast<double>(time_current.QuadPart - time_last.QuadPart) / static_cast<double>(time_frequency.QuadPart);

    if (delta < frame_time) {
      Sleep(static_cast<DWORD>(frame_time - delta) * 1000);
      continue;
    }
    time_last = time_current;

    updateVulkan();

    glfwPollEvents();
  }
  timeEndPeriod(1);

  uninitVulkan();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
