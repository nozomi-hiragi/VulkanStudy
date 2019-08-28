#pragma once

#include <functional>
#include <iostream>
#include <vector>

template<class Object, class... ReturnParams>
class Borrowed {
public:
  Borrowed() {}
  Borrowed(std::shared_ptr<Object> object, std::function<void(Object*, ReturnParams...)>return_func)
    : _object(object)
    , _return_func(return_func) {}

  auto getObject() {
    return _object;
  }

  void returnObject(ReturnParams... params) {
    _return_func(_object.get(), params...);
  }

private:
  std::shared_ptr<Object> _object;
  std::function<void(Object*, ReturnParams...)> _return_func;
};

template<class Object, class CreateParams, class... ReturnParams>
struct Order {
  CreateParams params;
  std::function<void(Borrowed<Object, ReturnParams...>)> address;
};

template<class Object, class CreateParams, class... ReturnParams>
class MailingFactory {
public:
  MailingFactory() : _borrowing_count(0) {}
  ~MailingFactory() {
    if (_borrowing_count == 0) {
      return;
    }
    std::cerr << _borrowing_count << " objects have not been returned." << std::endl;;
  }

  void borrowingRgequest(const Order<Object, CreateParams, ReturnParams...>& order) {
    auto object = _createObject(order.params);
    Borrowed<Object, ReturnParams...> borrowed(object, [this](Object* ptr, ReturnParams... params) {
      _returnObject(ptr, params...);
      _borrowing_count--;
    });
    order.address(borrowed);
    _borrowing_count++;
  }

  template<class Array>
  void borrowingRgequests(const Array& orders) {
    for (auto it : orders) {
      borrowingRgequest(it);
    }
  }

protected:
  virtual std::shared_ptr<Object> _createObject(const CreateParams& params) = 0;
  virtual void _returnObject(Object*, ReturnParams...) = 0;
private:
  uint32_t _borrowing_count;
};
