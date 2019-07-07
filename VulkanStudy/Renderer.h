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
    _instance_obj = _instance_factory.createInstance(app_name, app_version);
  }

  void destroyInstance() {
    _instance_factory.destroyInstance(_instance_obj);
  }

  void initPhysicalDevice() {
    _physical_device = PhysicalDevice(_instance_obj->_devices[PRIMALY_PHYSICAL_DEVICE_INDEX]);
    _memory_properties = _physical_device.getMemoryProperties();
  }

  void uninitPhysicalDevice() {
  }

  void createSurface(HINSTANCE hinstance, HWND hwnd) {
    _surface = Surface::createSurface(_instance_obj->_vk_instance, hinstance, hwnd);
    _surface.fixSurfaceProperties(&_physical_device);
  }

  void destroySurface() {
    Surface::destroySurface(_instance_obj->_vk_instance, _surface);
  }

  void createDevice() {
    _device = _physical_device.createDevice(_surface);
  }

  void destroyDevice() {
    _device.destroy();
  }

  PhysicalDevice _physical_device;
  VkPhysicalDeviceMemoryProperties _memory_properties;
  Surface _surface;
  Device _device;
protected:
private:
  InstanceFactory _instance_factory;
  std::shared_ptr<InstanceObject> _instance_obj;
};
