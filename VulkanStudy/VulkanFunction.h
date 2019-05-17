#pragma once

#include <Windows.h>
#include <stdint.h>

void initVulkan(HINSTANCE hinstance, HWND hwnd, uint32_t width, uint32_t height);
void updateVulkan();
void uninitVulkan();
