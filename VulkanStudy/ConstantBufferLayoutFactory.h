#pragma once

#include <vulkan/vulkan.h>
#include <memory>

#include "AbstractFactory.h"
#include "ConstantBufferLayoutObject.h"
#include "DescriptorSetLayoutFactory.h"
#include "DescriptorPoolFactory.h"
#include "PipelineLayoutFactory.h"

class ConstantBufferLayoutFactory : public AbstractFactory<ConstantBufferLayout, DeviceObject, std::vector<VkDescriptorSetLayoutBinding>&> {
public:

  std::shared_ptr<ConstantBufferLayout> _createCore(std::vector<VkDescriptorSetLayoutBinding>& descriptor_set_layout_bindings) {
    if (!_descriptor_pool) {
      _descriptor_pool = _descriptor_pool_factory.createObject(_parent);
    }

    auto descriptor_set_layout = _descriptor_set_layout_factory.createObject(_parent, descriptor_set_layout_bindings);
    auto descriptor_set = _descriptor_pool->createObject(_parent, descriptor_set_layout);
    auto pipeline = _pipeline_layout_factory.createObject(_parent, { descriptor_set_layout->_vk_descriptor_set_layout });

    return std::make_shared<ConstantBufferLayout>(descriptor_set_layout, descriptor_set, pipeline);
  }

  void _destroyCore(std::shared_ptr<ConstantBufferLayout> object) {
    _pipeline_layout_factory.destroyObject(object->_pipeline_layout);
    _descriptor_pool->destroyObject(object->_descriptor_set);
    _descriptor_set_layout_factory.destroyObject(object->_descriptor_set_layout);
  }

  void destroyAll() override {
    AbstractFactory::destroyAll();
    _descriptor_pool_factory.destroyAll();
  }

protected:
private:
  DescriptorSetLayoutFactory _descriptor_set_layout_factory;
  DescriptorPoolFactory _descriptor_pool_factory;
  PipelineLayoutFactory _pipeline_layout_factory;

  std::shared_ptr<DescriptorPoolObject> _descriptor_pool;
};
