#pragma once

#include <memory>
#include <vulkan/vulkan.h>

#include "PhysicalDeviceObject.h"
#include "DeviceObject.h"
#include "DeviceMemoryFactory.h"
#include "BufferFactory.h"

class DynamicUniformBufferRing {
public:
  DynamicUniformBufferRing(std::shared_ptr<DeviceObject> device,
    std::shared_ptr<PhysicalDeviceObject> physical_device,
    BufferFactory& buffer_factory,
    DeviceMemoryFactory& device_memory_factory,
    const uint32_t size) : _total_size(size), _offset(0), _device(device), _buffer_factory(buffer_factory), _device_memory_factory(device_memory_factory)
  {
    BufferOrder buffer_order;
    buffer_order.params = BufferParams(device, size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
    buffer_order.address = [this](BufferBorrowed borrowed) {
      _buffer = borrowed;
    };
    _buffer_factory.borrowingRgequest(buffer_order);
    auto memory_type_index = physical_device->findProperties(_buffer.getObject(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    _memory = _device_memory_factory.createObject(device, _buffer.getObject()->_vk_memory_requirements.size, memory_type_index);
    _buffer.getObject()->bindBufferMemory(device, _memory, 0);

    _pointer = reinterpret_cast<char*>(_memory->mapMemory(device, 0, _buffer.getObject()->_vk_memory_requirements.size));
  }

  ~DynamicUniformBufferRing() {
    _memory->unmapMemory(_device);

    _device_memory_factory.destroyObject(_memory);
    _buffer.returnObject();
  }

  void* getPointer(const uint32_t size, uint32_t * offset) {
    const auto alignmentied_size = (size + 255) & ~255;
    auto end = _offset + alignmentied_size;
    if (end >= _total_size) {
      _offset = alignmentied_size;
      *offset = 0;
      return _pointer;
    }
    else {
      *offset = _offset;
      _offset += alignmentied_size;
      return _pointer + *offset;
    }
  }

  std::shared_ptr<DeviceMemoryObject> _memory;
  BufferBorrowed _buffer;
protected:
private:
  const uint32_t _total_size;
  uint32_t _offset;
  char* _pointer;

  std::shared_ptr<DeviceObject> _device; // TODO?

  BufferFactory& _buffer_factory;
  DeviceMemoryFactory& _device_memory_factory;
};
