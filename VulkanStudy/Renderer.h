#pragma once

#include <Windows.h>
#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.hpp>

#include "InstanceFactory.h"
#include "PhysicalDeviceFactory.h"
#include "SurfaceFactory.h"
#include "Device.h"

constexpr auto PRIMALY_PHYSICAL_DEVICE_INDEX = 0;

class Renderer {
public:
  Renderer() {
  }

  ~Renderer() {
  }

  void createInstance(const char* const app_name, const uint32_t app_version) {
    _instance_object = _instance_factory.createInstance(app_name, app_version);
  }

  void destroyInstance() {
    _instance_factory.destroyInstance(_instance_object);
  }

  void createPhysicalDevice() {
    _physical_device_object = _physical_device_factory.createPhysicalDevice(_instance_object, PRIMALY_PHYSICAL_DEVICE_INDEX);
  }

  void destroyPhysicalDevice() {
    _physical_device_factory.destroyPhysicalDevice(_physical_device_object);
  }

  void createSurface(HINSTANCE hinstance, HWND hwnd) {
    _surface_object = _surface_factory.createSurface(_instance_object, hinstance, hwnd);
  }

  void destroySurface() {
    _surface_factory.destroySurface(_instance_object, _surface_object);
  }

  void createDevice() {
    _device = Device::createDevice(_physical_device_object, _surface_object->_vk_surface);
  }

  void destroyDevice() {
    _device.destroy();
  }

  auto getMemoryProperties() {
    return _physical_device_object->_memory_properties;
  }

  auto getPhysicalDeviceObject() {
    return _physical_device_object;
  }

  SurfaceFactory _surface_factory;
  std::shared_ptr<SurfaceObject> _surface_object;

  Device _device;
protected:
private:
  InstanceFactory _instance_factory;
  std::shared_ptr<InstanceObject> _instance_object;

  PhysicalDeviceFactory _physical_device_factory;
  std::shared_ptr<PhysicalDeviceObject> _physical_device_object;
};
