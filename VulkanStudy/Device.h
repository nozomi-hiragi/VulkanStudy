#pragma once

#include <vulkan/vulkan.hpp>
#include <memory>

#include "PhysicalDeviceObject.h"
#include "QueueObject.h"

class Device {
public:
  static Device createDevice(std::shared_ptr<PhysicalDeviceObject> physical_device, VkSurfaceKHR surface);

  Device(): _device(nullptr) {
  }

  Device(const vk::Device device) :
    _device(device) {
  }

  ~Device() {
  }

  void destroy() {
    if (!_device) { return; }
    _device.waitIdle();
    _device.destroy();
    _device = nullptr;
  }

  // for queue
  auto getQueue() {
    return _queue;
  }

  // for fence
  void resetFence(const vk::Fence fence) {
    _device.resetFences(fence);
  }

  // fom memory
  void* mapMemory(const vk::DeviceMemory memory, const uint64_t offset, const uint64_t size) {
    return _device.mapMemory(memory, offset, size);
  }

  void unmapMemory(const vk::DeviceMemory memory) {
    _device.unmapMemory(memory);
  }

  // for buffer
  void bindBufferMemory(const vk::Buffer buffer, const vk::DeviceMemory memory, const uint64_t offset) {
    _device.bindBufferMemory(buffer, memory, offset);
  }

  // for image
  void bindImageMemory(const vk::Image image, const vk::DeviceMemory memory, const uint64_t offset) {
    _device.bindImageMemory(image, memory, offset);
  }

  // for descriptor
  void updateDescriptorSets(const uint32_t write_count, const vk::WriteDescriptorSet* writes, const uint32_t copy_count, const vk::CopyDescriptorSet* copies) {
    _device.updateDescriptorSets(write_count, writes, copy_count, copies);
  }

  // for swapchain
  void acquireNextImage(VkSwapchainKHR swapchain, const uint64_t timeout, const vk::Semaphore semaphore, const vk::Fence fence, uint32_t* image_index) {
    _device.acquireNextImageKHR(swapchain, timeout, semaphore, fence, image_index);
  }

  auto getVkDevice() {
    return _device;
  }

protected:
private:
  vk::Device _device;
  std::shared_ptr<QueueObject> _queue;
};
