#pragma once

#include <memory>
#include <set>

template<class Object, class Parent, class... Params>
class AbstractFactory {
public:
  virtual ~AbstractFactory() {
    for (auto it : _container) {
      destroyObject(it);
    }
  }

  auto createObject(const std::shared_ptr<Parent> parent, const Params&... params) {
    _parent = parent;
    auto object = _createCore(params...);
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

  void destroyAll() {
    for (auto& it : _container) {
      _destroyCore(it);
    }
    _container.clear();
  }

protected:
  virtual std::shared_ptr<Object> _createCore(const Params...) = 0;
  virtual void _destroyCore(std::shared_ptr<Object>) = 0;

  std::shared_ptr<Parent> _parent;
private:
  std::set<std::shared_ptr<Object>> _container;
};
