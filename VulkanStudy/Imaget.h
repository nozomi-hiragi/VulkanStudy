#pragma once

#include <vulkan/vulkan.h>
#include <memory>
#include <set>

class ImageObject {
public:
  ImageObject(const VkImage image, const VkFormat format):
    _vk_image(image),
    _vk_fotmat(format) {
  }

  ~ImageObject() {
  }

  const VkImage _vk_image;
  const VkFormat _vk_fotmat;
protected:
private:
};

class ImageFactory {
public:
  static auto _createVkImage(VkDevice device, VkFormat format, VkImageUsageFlags usage, uint32_t width, uint32_t height) {
    VkImageCreateInfo image_info = {};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.pNext = nullptr;
    image_info.flags = 0;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.format = format;
    image_info.extent = { width, height, 1};
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.samples = VK_SAMPLE_COUNT_1_BIT;
    image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
    image_info.usage = usage;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    image_info.queueFamilyIndexCount = 0;
    image_info.pQueueFamilyIndices = nullptr;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VkImage image;
    auto result = vkCreateImage(device, &image_info, nullptr, &image);
    return image;
  }

  static void _destroyVkImage(VkDevice device, VkImage image) {
    vkDestroyImage(device, image, nullptr);
  }

  ImageFactory() {
  }

  ~ImageFactory() {
  }

  auto createImage(VkDevice device, VkFormat format, VkImageUsageFlags usage, uint32_t width, uint32_t height) {
    auto vk_image = _createVkImage(device, format, usage, width, height);
    auto object = std::make_shared<ImageObject>(vk_image, format);
    _container.insert(object);
    return object;
  }

  void destroyImage(VkDevice device, std::shared_ptr<ImageObject>& object) {
    if (!object) { return; }
    auto before = _container.size();
    _container.erase(object);
    auto after = _container.size();

    if (before != after) {
      _destroyVkImage(device, object->_vk_image);
      object.reset();
    }
  }

protected:
private:
  std::set<std::shared_ptr<ImageObject>> _container;
};
