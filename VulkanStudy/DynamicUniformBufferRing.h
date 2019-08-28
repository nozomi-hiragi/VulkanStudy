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
    MemoryProperties memory_properties,
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
    auto memory_type_index = memory_properties.findProperties(_buffer.getObject(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    DeviceMemoryOrder order;
    order.params = DeviceMemoryParams(device, _buffer.getObject()->_vk_memory_requirements.size, memory_type_index);
    order.address = [this](DeviceMemoryBorrowed borrowed) {
      _memory = borrowed;
    };

    _device_memory_factory.borrowingRgequest(order);
    _buffer.getObject()->bindBufferMemory(device, _memory.getObject(), 0);

    _pointer = reinterpret_cast<char*>(_memory.getObject()->mapMemory(device, 0, _buffer.getObject()->_vk_memory_requirements.size));
  }

  ~DynamicUniformBufferRing() {
    _memory.getObject()->unmapMemory(_device);

    _memory.returnObject();
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

  DeviceMemoryBorrowed _memory;
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
