#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>

class PosRotCamera {
  static constexpr auto UP_VEC = glm::vec4(0, -1, 0, 1);
  static constexpr auto FRONT_VEC = glm::vec4(0, 0, 1, 1);
public:
  PosRotCamera() 
    : _fov(glm::radians(45.f))
    , _near(0.1f)
    , _far(1000.f) {
  }

  void updateProjection() {
    _projection = glm::perspectiveFov(_fov, _width, _height, _near, _far);
  }

  void updateView() {
    auto rotate_mtx = glm::yawPitchRoll(_rotation.y, _rotation.x, _rotation.z);
    auto rotete_up = glm::vec3(rotate_mtx * UP_VEC);
    auto at = glm::vec3(rotate_mtx * FRONT_VEC) + _position;
    _view = glm::lookAt(_position, at, rotete_up);
  }

  void update() {
    updateProjection();
    updateView();
    _view_projection = _projection * _view;
  }

  const auto& getViewProjection() const {
    return _view_projection;
  }

  float _fov;
  float _width;
  float _height;
  float _near;
  float _far;

  glm::vec3 _position;
  glm::vec3 _rotation;
protected:
private:
  glm::mat4 _view;
  glm::mat4 _projection;
  glm::mat4 _view_projection;
};
