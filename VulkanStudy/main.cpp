#if _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define new new(_NORMAL_BLOCK, __FILE__, __LINE__)
#endif

#include "WindowManager.h"
#include "VulkanFunction.h"

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
