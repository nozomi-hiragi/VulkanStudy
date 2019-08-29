#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "BufferFactory.h"
#include "DeviceObject.h"
#include "PhysicalDeviceObject.h"
#include "DeviceMemoryFactory.h"
#include "CommandBufferObject.h"
#include "VertexLayoutBuilder.h"

class Mesh {
public:
  Mesh(
    const std::vector<glm::vec3>& position,
    const std::vector<glm::vec3>& normal,
    const std::vector<glm::vec4>& color,
    const std::vector<glm::vec2>& texcoord,
    const std::vector<glm::uint16>& index,
    MemoryProperties memory_properties)
    : _position(position)
    , _normal(normal)
    , _color(color)
    , _texcoord(texcoord)
    , _index(index)
    , _vertex_count(static_cast<uint32_t>(position.size()))
    , _index_count(static_cast<uint32_t>(index.size()))
    , _size_position(_vertex_count * sizeof(glm::vec3))
    , _size_normal(_vertex_count * sizeof(glm::vec3))
    , _size_color(_vertex_count * sizeof(glm::vec4))
    , _size_texcoord(_vertex_count * sizeof(glm::vec2))
    , _size_index(_index_count * sizeof(uint16_t))
    , _memory_properties(memory_properties)
  {
  }

  ~Mesh() {
    _vertex_memory.reset();

    _buffer_index   .reset();
    _buffer_texcoord.reset();
    _buffer_color   .reset();
    _buffer_normal  .reset();
    _buffer_position.reset();
  }

  std::vector<BufferOrder> createBufferOrders(std::shared_ptr<DeviceObject> device, std::function<void()> callback) {
    BufferOrder borrowed_position;
    BufferOrder borrowed_normal;
    BufferOrder borrowed_color;
    BufferOrder borrowed_texcoord;
    BufferOrder borrowed_index;

    borrowed_position.params = BufferParams(device, _size_position, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
    borrowed_normal.params   = BufferParams(device, _size_normal  , VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
    borrowed_color.params    = BufferParams(device, _size_color   , VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
    borrowed_texcoord.params = BufferParams(device, _size_texcoord, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
    borrowed_index.params    = BufferParams(device, _size_index   , VK_BUFFER_USAGE_INDEX_BUFFER_BIT,  VK_SHARING_MODE_EXCLUSIVE);

    borrowed_position.address = [this](BufferBorrowed borrowed) { _buffer_position = std::move(borrowed); };
    borrowed_normal.address   = [this](BufferBorrowed borrowed) { _buffer_normal = std::move(borrowed); };
    borrowed_color.address    = [this](BufferBorrowed borrowed) { _buffer_color = std::move(borrowed); };
    borrowed_texcoord.address = [this](BufferBorrowed borrowed) { _buffer_texcoord = std::move(borrowed); };
    borrowed_index.address    = [this, callback](BufferBorrowed borrowed) {
      _buffer_index = std::move(borrowed);

      _memory_size
        = _buffer_position->getObject()->_vk_memory_requirements.size
        + _buffer_normal  ->getObject()->_vk_memory_requirements.size
        + _buffer_color   ->getObject()->_vk_memory_requirements.size
        + _buffer_texcoord->getObject()->_vk_memory_requirements.size
        + _buffer_index   ->getObject()->_vk_memory_requirements.size;

      callback();
    };

    std::vector<BufferOrder> orders;
    orders.push_back(borrowed_position);
    orders.push_back(borrowed_normal);
    orders.push_back(borrowed_color);
    orders.push_back(borrowed_texcoord);
    orders.push_back(borrowed_index);
    return std::move(orders);
  }

  const DeviceMemoryOrder createMemoryOrder(std::shared_ptr<DeviceObject> device) {
    auto memory_property_bits = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    auto memory_type_index = _memory_properties.findProperties(_buffer_position->getObject(), memory_property_bits);

    DeviceMemoryOrder order;
    order.params = DeviceMemoryParams(device, _memory_size, memory_type_index);
    order.address = [this, device](DeviceMemoryBorrowed borrowed) {
      _vertex_memory = std::move(borrowed);

      uint64_t offset = 0;
      _buffer_position->getObject()->bindBufferMemory(device, _vertex_memory->getObject(), offset += 0);
      _buffer_normal  ->getObject()->bindBufferMemory(device, _vertex_memory->getObject(), offset += _buffer_position->getObject()->_vk_memory_requirements.size);
      _buffer_color   ->getObject()->bindBufferMemory(device, _vertex_memory->getObject(), offset += _buffer_normal  ->getObject()->_vk_memory_requirements.size);
      _buffer_texcoord->getObject()->bindBufferMemory(device, _vertex_memory->getObject(), offset += _buffer_color   ->getObject()->_vk_memory_requirements.size);
      _buffer_index   ->getObject()->bindBufferMemory(device, _vertex_memory->getObject(), offset += _buffer_texcoord->getObject()->_vk_memory_requirements.size);

      setBuffer(device);
    };
    return order;
  }

  void setBuffer(std::shared_ptr<DeviceObject> device) {
    char* vertex_map = static_cast<char*>(_vertex_memory->getObject()->mapMemory(device, 0, _memory_size));
    memcpy(vertex_map +=                                                          0, _position.data(), _size_position);
    memcpy(vertex_map += _buffer_position->getObject()->_vk_memory_requirements.size, _normal  .data(), _size_normal);
    memcpy(vertex_map += _buffer_normal  ->getObject()->_vk_memory_requirements.size, _color   .data(), _size_color);
    memcpy(vertex_map += _buffer_color   ->getObject()->_vk_memory_requirements.size, _texcoord.data(), _size_texcoord);
    memcpy(vertex_map += _buffer_texcoord->getObject()->_vk_memory_requirements.size, _index   .data(), _size_index);
    _vertex_memory->getObject()->unmapMemory(device);

    _vk_buffers[0] = _buffer_position->getObject()->_vk_buffer;
    _vk_buffers[1] = _buffer_normal  ->getObject()->_vk_buffer;
    _vk_buffers[2] = _buffer_color   ->getObject()->_vk_buffer;
    _vk_buffers[3] = _buffer_texcoord->getObject()->_vk_buffer;

    _buffer_offsets[0] = 0;
    _buffer_offsets[1] = 0;
    _buffer_offsets[2] = 0;
    _buffer_offsets[3] = 0;
  }

  void draw(std::shared_ptr<CommandBufferObject> command_buffer) {

    command_buffer->bindVertexBuffers(0, 4, _vk_buffers, _buffer_offsets);
    command_buffer->bindIndexBuffer(_buffer_index->getObject()->_vk_buffer, 0);

    constexpr uint32_t instance_count = 1;
    constexpr uint32_t first_index = 0;
    constexpr uint32_t vertex_offset = 0;
    constexpr uint32_t first_instance = 0;
    command_buffer->drawIndexed(_index_count, instance_count, first_index, vertex_offset, first_instance);
  }

  static auto createVertexLayout() {
    return std::move(VertexLayoutBuilder()
      .begin(0, 0).setLayout(VK_FORMAT_R32G32B32_SFLOAT)
      .begin(1, 1).setLayout(VK_FORMAT_R32G32B32_SFLOAT)
      .begin(2, 2).setLayout(VK_FORMAT_R32G32B32A32_SFLOAT)
      .begin(3, 3).setLayout(VK_FORMAT_R32G32_SFLOAT)
      .build());
  }

protected:
private:
  const std::vector<glm::vec3>& _position;
  const std::vector<glm::vec3>& _normal;
  const std::vector<glm::vec4>& _color;
  const std::vector<glm::vec2>& _texcoord;
  const std::vector<glm::uint16>& _index;

  const uint32_t _vertex_count;
  const uint32_t _index_count;
  const uint32_t _size_position;
  const uint32_t _size_normal;
  const uint32_t _size_color;
  const uint32_t _size_texcoord;
  const uint32_t _size_index;
  const MemoryProperties _memory_properties;

  VkDeviceSize _memory_size;

  BufferBorrowed _buffer_position;
  BufferBorrowed _buffer_normal;
  BufferBorrowed _buffer_color;
  BufferBorrowed _buffer_texcoord;
  BufferBorrowed _buffer_index;

  DeviceMemoryBorrowed _vertex_memory;

  VkBuffer _vk_buffers[4];
  uint64_t _buffer_offsets[4];
};
