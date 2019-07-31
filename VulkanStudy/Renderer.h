#pragma once

#include "InstanceFactory.h"
#include "SurfaceFactory.h"
#include "DeviceFactory.h"

class Renderer {
public:
  Renderer() {
  }

  ~Renderer() {
  }

  void initDevice(const char* app_name, const uint32_t app_version, const uint32_t width, const uint32_t height, const HINSTANCE hinstance, const HWND hwnd) {
    _width = width;
    _height = height;

    _instance_object = _instance_factory.createObject(nullptr, app_name, app_version);
    _physical_device_object = _instance_object->_physical_devices[0];
    _surface_object = _surface_factory.createObject(_instance_object, hinstance, hwnd);
    _device_object = _device_factory.createObject(nullptr, _physical_device_object, _surface_object);
  }

  void uninitDevice() {
    _device_factory.destroyObject(_device_object);
    _surface_factory.destroyObject(_surface_object);
    _instance_factory.destroyObject(_instance_object);
  }

  InstanceFactory _instance_factory;
  SurfaceFactory _surface_factory;
  DeviceFactory _device_factory;

  std::shared_ptr<InstanceObject> _instance_object;
  std::shared_ptr<PhysicalDeviceObject> _physical_device_object;
  std::shared_ptr<SurfaceObject> _surface_object;
  std::shared_ptr<DeviceObject> _device_object;
protected:
private:
  uint32_t _width;
  uint32_t _height;
};
