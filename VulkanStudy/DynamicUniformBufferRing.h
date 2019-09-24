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
    const uint32_t size,
    std::function<void(BufferOrder buffer_order)> buffer_order_func,
    std::function<void(DeviceMemoryOrder memory_order)> memory_order_func)
    : _total_size(size)
    , _offset(0)
  {
    BufferOrder buffer_order;
    buffer_order.params = BufferParams(device, _total_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
    buffer_order.address = [this](BufferBorrowed borrowed) {
      _buffer = std::move(borrowed);
    };
    buffer_order_func(buffer_order);

    auto memory_type_index = memory_properties.findProperties(_buffer->getObject(), VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    DeviceMemoryOrder memory_order;
    memory_order.params = DeviceMemoryParams(device, _buffer->getObject()->_vk_memory_requirements.size, memory_type_index);
    memory_order.address = [this, device](DeviceMemoryBorrowed borrowed) {
      _memory = std::move(borrowed);

      _buffer->getObject()->bindBufferMemory(device, _memory->getObject(), 0);
      _pointer = reinterpret_cast<char*>(_memory->getObject()->mapMemory(device, 0, _buffer->getObject()->_vk_memory_requirements.size));
    };
    memory_order_func(memory_order);
  }

  ~DynamicUniformBufferRing() {
    _memory.reset();
    _buffer.reset();
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
};
