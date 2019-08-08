#pragma once

#include <Windows.h>
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void initVulkan(GLFWwindow* window, uint32_t width, uint32_t height);
void updateVulkan();
void uninitVulkan();
