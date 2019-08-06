#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "BufferFactory.h"
#include "DeviceObject.h"
#include "PhysicalDeviceObject.h"
#include "DeviceMemoryFactory.h"
#include "CommandBufferObject.h"

class Mesh {
public:
  Mesh(
    const std::vector<glm::vec3>& position,
    const std::vector<glm::vec3>& normal,
    const std::vector<glm::vec4>& color,
    const std::vector<glm::vec2>& texcoord,
    const std::vector<glm::uint16> index,
    BufferFactory& buffer_factory,
    DeviceMemoryFactory& memory_factory,
    std::shared_ptr<PhysicalDeviceObject> physical_device,
    std::shared_ptr<DeviceObject> device) :
    _buffer_factory(buffer_factory),
    _memory_factory(memory_factory) {

    constexpr auto size_vec2 = sizeof(glm::vec2);
    constexpr auto size_vec3 = sizeof(glm::vec3);
    constexpr auto size_vec4 = sizeof(glm::vec4);
    constexpr auto size_uint16 = sizeof(uint16_t);

    auto vertex_count = static_cast<uint32_t>(position.size());
    _index_count = static_cast<uint32_t>(index.size());

    const auto size_position = vertex_count * size_vec3;
    const auto size_normal   = vertex_count * size_vec3;
    const auto size_color    = vertex_count * size_vec4;
    const auto size_texcoord = vertex_count * size_vec2;
    const auto size_index    = _index_count * size_uint16;

    _buffer_position = _buffer_factory.createObject(device, size_position, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
    _buffer_normal   = _buffer_factory.createObject(device, size_normal,   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
    _buffer_color    = _buffer_factory.createObject(device, size_color,    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
    _buffer_texcoord = _buffer_factory.createObject(device, size_texcoord, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_SHARING_MODE_EXCLUSIVE);
    _buffer_index    = _buffer_factory.createObject(device, size_index,    VK_BUFFER_USAGE_INDEX_BUFFER_BIT,  VK_SHARING_MODE_EXCLUSIVE);

    auto memory_property_bits = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    auto memory_type_index = physical_device->findProperties(_buffer_position, memory_property_bits);

    auto memory_size =
      _buffer_position->_vk_memory_requirements.size +
      _buffer_normal->_vk_memory_requirements.size +
      _buffer_color->_vk_memory_requirements.size +
      _buffer_texcoord->_vk_memory_requirements.size +
      _buffer_index->_vk_memory_requirements.size;

    _vertex_memory = _memory_factory.createObject(device, memory_size, memory_type_index);

    char* vertex_map = static_cast<char*>(_vertex_memory->mapMemory(device, 0, memory_size));
    memcpy(vertex_map,
      position.data(), size_position);
    memcpy(vertex_map + _buffer_position->_vk_memory_requirements.size,
      normal.data(),   size_normal);
    memcpy(vertex_map + _buffer_position->_vk_memory_requirements.size + _buffer_normal->_vk_memory_requirements.size,
      color.data(),    size_color);
    memcpy(vertex_map + _buffer_position->_vk_memory_requirements.size + _buffer_normal->_vk_memory_requirements.size + _buffer_color->_vk_memory_requirements.size,
      texcoord.data(), size_texcoord);
    memcpy(vertex_map + _buffer_position->_vk_memory_requirements.size + _buffer_normal->_vk_memory_requirements.size + _buffer_color->_vk_memory_requirements.size + _buffer_texcoord->_vk_memory_requirements.size,
      index.data(), size_index);
    _vertex_memory->unmapMemory(device);

    _buffer_position->bindBufferMemory(device, _vertex_memory, 0);
    _buffer_normal  ->bindBufferMemory(device, _vertex_memory, _buffer_position->_vk_memory_requirements.size);
    _buffer_color   ->bindBufferMemory(device, _vertex_memory, _buffer_position->_vk_memory_requirements.size + _buffer_normal->_vk_memory_requirements.size);
    _buffer_texcoord->bindBufferMemory(device, _vertex_memory, _buffer_position->_vk_memory_requirements.size + _buffer_normal->_vk_memory_requirements.size + _buffer_color->_vk_memory_requirements.size);
    _buffer_index   ->bindBufferMemory(device, _vertex_memory, _buffer_position->_vk_memory_requirements.size + _buffer_normal->_vk_memory_requirements.size + _buffer_color->_vk_memory_requirements.size + _buffer_texcoord->_vk_memory_requirements.size);

    _vk_buffers[0] = _buffer_position->_vk_buffer;
    _vk_buffers[1] = _buffer_normal->_vk_buffer;
    _vk_buffers[2] = _buffer_color->_vk_buffer;
    _vk_buffers[3] = _buffer_texcoord->_vk_buffer;

    _buffer_offsets[0] = 0;
    _buffer_offsets[1] = 0;
    _buffer_offsets[2] = 0;
    _buffer_offsets[3] = 0;
  }

  ~Mesh() {
    _memory_factory.destroyObject(_vertex_memory);

    _buffer_factory.destroyObject(_buffer_index);
    _buffer_factory.destroyObject(_buffer_texcoord);
    _buffer_factory.destroyObject(_buffer_color);
    _buffer_factory.destroyObject(_buffer_normal);
    _buffer_factory.destroyObject(_buffer_position);
  }

  void draw(std::shared_ptr<CommandBufferObject> command_buffer) {

    command_buffer->bindVertexBuffers(0, 4, _vk_buffers, _buffer_offsets);
    command_buffer->bindIndexBuffer(_buffer_index->_vk_buffer, 0);

    constexpr uint32_t instance_count = 1;
    constexpr uint32_t first_index = 0;
    constexpr uint32_t vertex_offset = 0;
    constexpr uint32_t first_instance = 0;
    command_buffer->drawIndexed(_index_count, instance_count, first_index, vertex_offset, first_instance);
  }

protected:
private:
  BufferFactory& _buffer_factory;
  DeviceMemoryFactory& _memory_factory;

  std::shared_ptr<BufferObject> _buffer_position;
  std::shared_ptr<BufferObject> _buffer_normal;
  std::shared_ptr<BufferObject> _buffer_color;
  std::shared_ptr<BufferObject> _buffer_texcoord;
  std::shared_ptr<BufferObject> _buffer_index;

  std::shared_ptr<DeviceMemoryObject> _vertex_memory;

  VkBuffer _vk_buffers[4];
  uint64_t _buffer_offsets[4];
  uint32_t _index_count;
};
