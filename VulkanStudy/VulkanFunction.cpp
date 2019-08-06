
#include "VulkanFunction.h"
#include <vector>
#include <iostream>

#include "Renderer.h"

const char* const APP_NAME = "VulkanStudy";
const uint32_t APP_VERSION = 0;

Renderer _renderer;

void initVulkan(HINSTANCE hinstance, HWND hwnd, uint32_t width, uint32_t height) {
  _renderer.init(APP_NAME, APP_VERSION, width, height, hinstance, hwnd);
}

void updateVulkan() {
  _renderer.update();
}

void uninitVulkan() {
  _renderer.uninit();
}
