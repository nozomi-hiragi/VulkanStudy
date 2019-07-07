#pragma once

#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

#include "Instance.h"
#include "PhysicalDevice.h"
#include "Surface.h"
#include "Device.h"

constexpr auto PRIMALY_PHYSICAL_DEVICE_INDEX = 0;

class Renderer {
public:
  Renderer() {
  }

  ~Renderer() {
  }

  void createInstance(const char* const app_name, const uint32_t app_version) {
    _instance.createInstance(app_name, app_version);
  }

  void destroyInstance() {
    _instance.destroyInstance();
  }

  void initPhysicalDevice() {
    _physical_device = _instance.getPhysicalDevice(PRIMALY_PHYSICAL_DEVICE_INDEX);
    _memory_properties = _physical_device.getMemoryProperties();
  }

  void uninitPhysicalDevice() {
  }

  void createSurface(HINSTANCE hinstance, HWND hwnd) {
    _surface = _instance.createSurface(hinstance, hwnd);
  }

  void destroySurface() {
    _instance.destroySurface(_surface);
  }

  void createDevice() {
    _device = _physical_device.createDevice(_surface);
  }

  void destroyDevice() {
    _device.destroy();
  }

  PhysicalDevice _physical_device;
  vk::PhysicalDeviceMemoryProperties _memory_properties;
  Surface _surface;
  Device _device;
protected:
private:
  Instance _instance;
};
