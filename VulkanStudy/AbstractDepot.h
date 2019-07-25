#pragma once

#include <functional>
#include <map>
#include <string>

template<class T>
class AbstractDepot {
public:
  void add(const std::string& name, const T& value) {
    _container.insert_or_assign(name, value);
  }

  void remove(const std::string& name) {
    _container.erase(name);
  }

  const T& get(const std::string name) {
    return _container.find(name)->second;
  }

  void edit(const std::string name, const std::function<void(T&)> edit_function) {
    edit_function(_container.find(name)->second);
  }

protected:
private:
  std::map<const std::string, T> _container;
};
