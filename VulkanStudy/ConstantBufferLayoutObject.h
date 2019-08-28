#pragma once

#include <memory>

#include "DescriptorSetLayoutObject.h"
#include "DescriptorSetObject.h"
#include "PipelineLayoutObject.h"

class ConstantBufferLayoutObject {
public:
  ConstantBufferLayoutObject(std::shared_ptr<DescriptorSetLayoutObject> descriptor_set_layout, std::shared_ptr<DescriptorSetObject> descriptor_set, std::shared_ptr<PipelineLayoutObject> pipeline_layout)
    : _descriptor_set_layout(descriptor_set_layout)
    , _descriptor_set(descriptor_set)
    , _pipeline_layout(pipeline_layout) {
  }

  std::shared_ptr<DescriptorSetLayoutObject> _descriptor_set_layout;
  std::shared_ptr<DescriptorSetObject> _descriptor_set;
  std::shared_ptr<PipelineLayoutObject> _pipeline_layout;
protected:
private:
};
