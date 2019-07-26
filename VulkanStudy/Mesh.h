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
    std::vector<glm::vec3>& position,
    std::vector<glm::vec3>& normal,
    std::vector<glm::vec4>& color,
    std::vector<glm::vec2>& texcoord,
    BufferFactory& buffer_factory,
    DeviceMemoryFactory& memory_factory,
    std::shared_ptr<PhysicalDeviceObject> physical_device,
    std::shared_ptr<DeviceObject> device) :
    _buffer_factory(buffer_factory),
    _memory_factory(memory_factory) {

    constexpr auto size_vec2 = sizeof(glm::vec2);
    constexpr auto size_vec3 = sizeof(glm::vec3);
    constexpr auto size_vec4 = sizeof(glm::vec4);

    _vertex_count = static_cast<uint32_t>(position.size());

    const auto size_position = _vertex_count * size_vec3;
    const auto size_normal   = _vertex_count * size_vec3;
    const auto size_color    = _vertex_count * size_vec4;
    const auto size_texcoord = _vertex_count * size_vec2;

    _buffer_position = _buffer_factory.createObject(device, size_position, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    _buffer_normal   = _buffer_factory.createObject(device, size_normal,   VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    _buffer_color    = _buffer_factory.createObject(device, size_color,    VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
    _buffer_texcoord = _buffer_factory.createObject(device, size_texcoord, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);

    auto memory_property_bits = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    auto memory_type_index = physical_device->findProperties(_buffer_position, memory_property_bits);

    auto memory_size =
      _buffer_position->_vk_memory_requirements.size +
      _buffer_normal->_vk_memory_requirements.size +
      _buffer_color->_vk_memory_requirements.size +
      _buffer_texcoord->_vk_memory_requirements.size;

    _vertex_memory = _memory_factory.createObject(device, memory_size, memory_type_index);

    char* vertex_map = static_cast<char*>(DeviceMemoryObject::vkMapMemory_(device->_vk_device, _vertex_memory->_vk_device_memory, 0, memory_size));
    memcpy(vertex_map + 0,                                              position.data(), size_position);
    memcpy(vertex_map + _buffer_position->_vk_memory_requirements.size, normal.data(),   size_normal);
    memcpy(vertex_map + _buffer_position->_vk_memory_requirements.size + _buffer_normal->_vk_memory_requirements.size,   color.data(),    size_color);
    memcpy(vertex_map + _buffer_position->_vk_memory_requirements.size + _buffer_normal->_vk_memory_requirements.size + _buffer_color->_vk_memory_requirements.size,    texcoord.data(), size_texcoord);
    DeviceMemoryObject::vkUnmapMemory_(device->_vk_device, _vertex_memory->_vk_device_memory);

    BufferObject::vkBindBufferMemory_(device->_vk_device, _buffer_position->_vk_buffer, _vertex_memory->_vk_device_memory, 0);
    BufferObject::vkBindBufferMemory_(device->_vk_device, _buffer_normal->_vk_buffer,   _vertex_memory->_vk_device_memory, _buffer_position->_vk_memory_requirements.size);
    BufferObject::vkBindBufferMemory_(device->_vk_device, _buffer_color->_vk_buffer,    _vertex_memory->_vk_device_memory, _buffer_position->_vk_memory_requirements.size + _buffer_normal->_vk_memory_requirements.size);
    BufferObject::vkBindBufferMemory_(device->_vk_device, _buffer_texcoord->_vk_buffer, _vertex_memory->_vk_device_memory, _buffer_position->_vk_memory_requirements.size + _buffer_normal->_vk_memory_requirements.size + _buffer_color->_vk_memory_requirements.size);

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

    _buffer_factory.destroyObject(_buffer_texcoord);
    _buffer_factory.destroyObject(_buffer_color);
    _buffer_factory.destroyObject(_buffer_normal);
    _buffer_factory.destroyObject(_buffer_position);
  }

  void draw(std::shared_ptr<CommandBufferObject> command_buffer) {

    command_buffer->bindVertexBuffers(0, 4, _vk_buffers, _buffer_offsets);

    constexpr uint32_t instance_count = 1;
    constexpr uint32_t first_vertex = 0;
    constexpr uint32_t first_instance = 0;
    command_buffer->draw(_vertex_count, instance_count, first_vertex, first_instance);
  }

protected:
private:
  BufferFactory& _buffer_factory;
  DeviceMemoryFactory& _memory_factory;

  std::shared_ptr<BufferObject> _buffer_position;
  std::shared_ptr<BufferObject> _buffer_normal;
  std::shared_ptr<BufferObject> _buffer_color;
  std::shared_ptr<BufferObject> _buffer_texcoord;

  std::shared_ptr<DeviceMemoryObject> _vertex_memory;

  VkBuffer _vk_buffers[4];
  uint64_t _buffer_offsets[4];
  uint32_t _vertex_count;
};
