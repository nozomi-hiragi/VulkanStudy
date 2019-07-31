#pragma once

class Renderer {
public:
  Renderer() {
  }

  ~Renderer() {
  }

  void init(const char* app_name, const uint32_t app_version, const uint32_t width, const uint32_t height, const HINSTANCE hinstance, const HWND hwnd) {
    _width = width;
    _height = height;
  }

  void uninit() {
  }

protected:
private:
  uint32_t _width;
  uint32_t _height;
};
