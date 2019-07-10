#pragma once

#include <memory>
#include <set>

template<class Object, class Params>
class AbstractFactory {
public:
  auto createObject(const Params& params) {
    auto object = _createCore(params);
    _container.insert(object);
    return object;
  }

  void destroyObject(std::shared_ptr<Object>& object) {
    if (!object) { return; }
    auto before_size = _container.size();
    _container.erase(object);
    auto after_size = _container.size();

    if (before_size != after_size) {
      _destroyCore(object);
      object.reset();
    }
  }

protected:
  virtual std::shared_ptr<Object> _createCore(const Params&) = 0;
  virtual void _destroyCore(std::shared_ptr<Object>) = 0;
private:
  std::set<std::shared_ptr<Object>> _container;
};
