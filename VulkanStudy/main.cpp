#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include "WindowManager.h"
#include "VulkanFunction.h"

#include <iostream>
#include <set>
struct ObjectA {
  ObjectA(const int value) : 
    _value(value) {
    std::cout << "construct:" << _value << std::endl;
  }

  ~ObjectA() {
    std::cout << "destruct:" << _value << std::endl;
  }

  const int _value;
};

class FactoryA {
  static auto& _getContainer() {
    static std::set<std::shared_ptr<ObjectA>> container;
    return container;
  }
public:
  static auto createObject(const int value) {
    auto object = std::make_shared<ObjectA>(value);
    _getContainer().insert(object);
    return std::move(object);
  }

  static void destroyObject(std::shared_ptr<ObjectA> object) {
    _getContainer().erase(object);
  }

  static void clear() {
    _getContainer().clear();
  }

protected:
private:
};

int main(int argc, char* argv[]) {
#if _DEBUG
  _CrtSetDbgFlag(_CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF);
#endif

  WindowManager window(L"window", 1366, 768);
  window.startWindowThread();

  initVulkan(window.gethinstance(), window.gethwnd(), 1366, 768);

  while (window.isExistWindowThread()) {
    updateVulkan();

    Sleep(1);
  }

  uninitVulkan();

  return 0;
}
