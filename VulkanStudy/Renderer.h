#pragma once

#include "InstanceFactory.h"
#include "SurfaceFactory.h"

class Renderer {
public:
  Renderer() {
  }

  ~Renderer() {
  }

  void init(const char* app_name, const uint32_t app_version, const HINSTANCE hinstance, const HWND hwnd) {
    _instance_object = _instance_factory.createObject(nullptr, app_name, app_version);
    _physical_device_object = _instance_object->_physical_devices[0];
    _surface_object = _surface_factory.createObject(_instance_object, hinstance, hwnd);
    _device_object = _device_factory.createObject(nullptr, _physical_device_object, _surface_object);
    _queue_object = _device_object->_queue_object;
    _command_pool_object = _command_pool_factory.createObject(_device_object, _queue_object);
    _command_buffer_object = _command_pool_object->createObject(_device_object);
  }

  void uninit() {
    _command_pool_object->destroyObject(_command_buffer_object);
    _command_pool_factory.destroyObject(_command_pool_object);
    _device_factory.destroyObject(_device_object);
    _surface_factory.destroyObject(_surface_object);
    _instance_factory.destroyObject(_instance_object);
  }

  std::shared_ptr<InstanceObject> _instance_object;
  std::shared_ptr<PhysicalDeviceObject> _physical_device_object;
  std::shared_ptr<SurfaceObject> _surface_object;
  std::shared_ptr<DeviceObject> _device_object;
  std::shared_ptr<QueueObject> _queue_object;
  std::shared_ptr<CommandPoolObject> _command_pool_object;
  std::shared_ptr<CommandBufferObject> _command_buffer_object;
protected:
private:
  InstanceFactory _instance_factory;
  SurfaceFactory _surface_factory;
  DeviceFactory _device_factory;
  CommandPoolFactory _command_pool_factory;
};
