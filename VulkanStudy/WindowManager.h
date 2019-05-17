#pragma once
/**
 * @file WindowManager.h
 * @brief
 * @author nozomi hiragi
 * @date 2019/05/02
 * Copyright (C) 2019 nozomi hiragi. All rights reserved.
 */
#pragma once
#ifndef WINDOW_MANAGER_H_
#define WINDOW_MANAGER_H_

#include <Windows.h>
#include <string>
#include <thread>
#include <mutex>

class WindowManager {
private:
  HWND _hwnd;
  HINSTANCE _hinstance;

  std::wstring _app_name;
  long _window_width;
  long _window_height;

  std::thread _window_thread;
  bool _window_thread_end_flag;

  std::mutex _condition_mutex;
  std::condition_variable _start_condition;

  /**
   * Processing message
   */
  LRESULT _processingMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

  /**
   * Window procedure
   */
  static LRESULT CALLBACK _windowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    WindowManager* instance = reinterpret_cast<WindowManager*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

    if (instance != nullptr) {
      return instance->_processingMessage(uMsg, wParam, lParam);
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }

protected:
public:

  /**
   * Constructor
   */
  WindowManager(const wchar_t* app_name, long width, long height) :
    _app_name(app_name),
    _window_width(width),
    _window_height(height),
    _window_thread(),
    _window_thread_end_flag(true),
    _hwnd(nullptr) {
  }

  HWND gethwnd() { return _hwnd; }
  HINSTANCE gethinstance() { return _hinstance; }

  /**
   * Destructor
   */
  ~WindowManager() {
    endWindowThread();
  }

  /**
   * Create window
   */
  void createWindow();

  /**
   * Destroy window
   */
  void destroyWindow();

  /**
   * Process system message
   * return quit: true
   */
  bool processSystemMessage();

  /**
   * Start window thread
   */
  void startWindowThread();

  /**
   * End window thread
   */
  void endWindowThread();

  bool isExistWindowThread() {
    return !_window_thread_end_flag;
  }
};

#endif
