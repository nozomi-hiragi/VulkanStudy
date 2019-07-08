
#include "PhysicalDevice.h"

std::shared_ptr<PhysicalDeviceObject> PhysicalDeviceFactory::createPhysicalDevice(std::shared_ptr<InstanceObject> instance, uint32_t index) {
  auto vk_physical_device = instance->_devices[index];

  uint32_t count = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(vk_physical_device, &count, nullptr);
  std::vector<VkQueueFamilyProperties> queue_family_properties(count);
  vkGetPhysicalDeviceQueueFamilyProperties(vk_physical_device, &count, queue_family_properties.data());

  VkPhysicalDeviceMemoryProperties memory_properties;
  vkGetPhysicalDeviceMemoryProperties(vk_physical_device, &memory_properties);

  auto object = std::make_shared<PhysicalDeviceObject>(vk_physical_device, memory_properties, std::move(queue_family_properties));
  _container.insert(object);
  return object;
}

void PhysicalDeviceFactory::destroyPhysicalDevice(std::shared_ptr<PhysicalDeviceObject>& object) {
  if (!object) { return; }
  auto before_size = _container.size();
  _container.erase(object);
  auto after_size = _container.size();

  if (before_size != after_size) {
    // non destroy
    object.reset();
  }
}
