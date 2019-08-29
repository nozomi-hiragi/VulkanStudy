#pragma once

#include <functional>
#include <iostream>
#include <vector>
#include <memory>

template<class Object>
class Borrowed {
public:
  Borrowed() {}
  Borrowed(std::shared_ptr<Object> object, std::function<void(Object*)>return_func)
    : _object(object)
    , _return_func(return_func)
  {}

  ~Borrowed() {
    _return_func(_object.get());
  }

  auto getObject() {
    return _object;
  }

private:
  std::shared_ptr<Object> _object;
  std::function<void(Object*)> _return_func;
};

template<class Object, class Params>
struct Order {
  Params params;
  std::function<void(std::unique_ptr<Borrowed<Object>>)> address;
};

template<class Object, class Params>
class MailingFactory {
public:
  MailingFactory() : _borrowing_count(0) {}
  ~MailingFactory() {
    if (_borrowing_count == 0) {
      return;
    }
    std::cerr << _borrowing_count << " objects have not been returned." << std::endl;;
  }

  void borrowingRgequest(const Order<Object, Params>& order) {
    auto object = _createObject(order.params);
    auto borrowed = std::make_unique<Borrowed<Object>>(object, [this](Object* ptr) {
      _returnObject(ptr);
      _borrowing_count--;
    });
    order.address(std::move(borrowed));
    _borrowing_count++;
  }

  template<class Array>
  void borrowingRgequests(const Array& orders) {
    for (auto it : orders) {
      borrowingRgequest(it);
    }
  }

protected:
  virtual std::shared_ptr<Object> _createObject(const Params& params) = 0;
  virtual void _returnObject(Object*) = 0;
private:
  uint32_t _borrowing_count;
};
