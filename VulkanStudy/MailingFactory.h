#pragma once

#include <functional>

template<class Object>
class Borrowed {
public:
  Borrowed() {}
  Borrowed(std::shared_ptr<Object> object, std::function<void(Object*)>return_func)
    : _object(object)
    , _return_func(return_func) {}

  auto getObject() {
    return _object;
  }

  void returnObject() {
    _return_func(_object.get());
  }

private:
  std::shared_ptr<Object> _object;
  std::function<void(Object*)> _return_func;
};

template<class Object, class Params>
struct Order {
  Params params;
  std::function<void(Borrowed<Object>)> address;
};

template<class Object, class Params>
class MailingFactory {
public:
  void borrowinRgequest(Order<Object, Params>& order) {
    auto object = _createObject(order.params);
    Borrowed<Object> borrowed(object, [this](Object* ptr) {
      _returnObject(ptr);
    });
    order.address(borrowed);
  }

protected:
  virtual std::shared_ptr<Object> _createObject(Params& params) = 0;
  virtual void _returnObject(Object* object) = 0;
private:
};
