#pragma once

#include "InstanceFactory.h"
#include "SurfaceFactory.h"

class Renderer {
public:
  Renderer() {
  }

  ~Renderer() {
  }

  void init(const char* app_name, const uint32_t app_version, const uint32_t width, const uint32_t height, const HINSTANCE hinstance, const HWND hwnd) {
    _width = width;
    _height = height;
    _instance_object = _instance_factory.createObject(nullptr, app_name, app_version);
    _physical_device_object = _instance_object->_physical_devices[0];
    _surface_object = _surface_factory.createObject(_instance_object, hinstance, hwnd);
    _device_object = _device_factory.createObject(nullptr, _physical_device_object, _surface_object);
    _queue_object = _device_object->_queue_object;
    _command_pool_object = _command_pool_factory.createObject(_device_object, _queue_object);
    _command_buffer_object = _command_pool_object->createObject(_device_object);
    _swapchain_object = _swapchain_factory.createObject(_device_object, _surface_object, _physical_device_object, _width, _height);
    _depth_image_object = _image_factory.createObject(_device_object, VK_FORMAT_D16_UNORM, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, width, height, VK_IMAGE_ASPECT_DEPTH_BIT);
    {
      auto memory_type_index = _physical_device_object->findProperties(_depth_image_object, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
      _depth_memory_object = _device_memory_factory.createObject(_device_object, _depth_image_object->_vk_memory_requirements.size, memory_type_index);
    }
    _depth_image_object->bindImageMemory(_device_object, _depth_memory_object, 0);
    _swapchain_image_view_objects.reserve(_swapchain_object->_swapchain_image_count);
    for (auto image : _swapchain_object->_swapchain_images) {
      _swapchain_image_view_objects.push_back(_image_view_factory.createObject(_device_object, image));
    }
    _depth_image_view_object = _image_view_factory.createObject(_device_object, _depth_image_object);
    _semaphore = _semaphore_factory.createObject(_device_object);
    _fence = _fence_factory.createObject(_device_object);
    _descriptor_set_layout_factory.getDescriptorSetLayoutBindingDepot().add("Uniform",
      0,
      VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
      1,
      VK_SHADER_STAGE_VERTEX_BIT,
      nullptr
    );
    std::vector<std::string> descriptor_set_layout_binding_names = { "Uniform" };
    _descriptor_set_layout = _descriptor_set_layout_factory.createObject(_device_object, descriptor_set_layout_binding_names);
    _descriptor_pool = _descriptor_pool_factory.createObject(_device_object);
    _descriptor_set = _descriptor_pool->createObject(_device_object, { _descriptor_set_layout->_vk_descriptor_set_layout });

  }

  void uninit() {
    _descriptor_pool->destroyObject(_descriptor_set);
    _descriptor_pool_factory.destroyObject(_descriptor_pool);
    _descriptor_set_layout_factory.destroyObject(_descriptor_set_layout);
    _fence_factory.destroyObject(_fence);
    _semaphore_factory.destroyObject(_semaphore);
    _image_view_factory.destroyObject(_depth_image_view_object);
    for (auto& image_view : _swapchain_image_view_objects) {
      _image_view_factory.destroyObject(image_view);
    }
    _swapchain_image_view_objects.clear();
    _device_memory_factory.destroyObject(_depth_memory_object);
    _image_factory.destroyObject(_depth_image_object);
    _swapchain_factory.destroyObject(_swapchain_object);
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
  std::shared_ptr<SwapchainObject> _swapchain_object;
  std::shared_ptr<ImageObject> _depth_image_object;
  std::shared_ptr<DeviceMemoryObject> _depth_memory_object;
  std::vector<std::shared_ptr<ImageViewObject>> _swapchain_image_view_objects;
  std::shared_ptr<ImageViewObject> _depth_image_view_object;
  std::shared_ptr<SemaphoreObject> _semaphore;
  std::shared_ptr<FenceObject> _fence;
  std::shared_ptr<DescriptorSetLayoutObject> _descriptor_set_layout;
  std::shared_ptr<DescriptorPoolObject> _descriptor_pool;
  std::shared_ptr<DescriptorSetObject> _descriptor_set;
protected:
private:
  uint32_t _width;
  uint32_t _height;
  InstanceFactory _instance_factory;
  SurfaceFactory _surface_factory;
  DeviceFactory _device_factory;
  CommandPoolFactory _command_pool_factory;
  SwapchainFactory _swapchain_factory;
  ImageFactory _image_factory;
public:
  DeviceMemoryFactory _device_memory_factory;
private:
  ImageViewFactory _image_view_factory;
  SemaphoreFactory _semaphore_factory;
  FenceFactory _fence_factory;
  DescriptorSetLayoutFactory _descriptor_set_layout_factory;
  DescriptorPoolFactory _descriptor_pool_factory;
};
