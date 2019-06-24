#pragma once

#include <vulkan/vulkan.hpp>

class Queue {
public:
  Queue(): _queue(nullptr) {
  }

  Queue(vk::Queue queue): _queue(queue) {
  }

  ~Queue() {
  }

  void submit(const vk::SubmitInfo& info, const vk::Fence fence) {
    _queue.submit(info, fence);
  }

  void present(const vk::PresentInfoKHR& info) {
    _queue.presentKHR(info);
  }

protected:
private:
  vk::Queue _queue;
};
