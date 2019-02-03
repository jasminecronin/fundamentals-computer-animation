#pragma once

#include <bitset>

#include "mat4f.h"
#include "vec3f.h"

namespace openGL {
namespace scene {

struct CameraUpdate {
  enum Flag {
    moveUp = 0,
    moveDown,
    moveLeft,
    moveRight,
    moveForward,
    moveBackward,
    rotateUp,
    rotateDown,
    rotateLeft,
    rotateRight,
    rollLeft,
    rollRight,
    COUNT
  };

  std::bitset<COUNT> flag;

  void set(Flag flagToSet) { flag.set(flagToSet); }
  void set(Flag flagToSet, bool value) { flag.set(flagToSet, value); }
  bool isSet(Flag flagSet) { return flag.test(flagSet); }
  bool needsUpdating() const { return flag.any(); }
  void reset() { flag.reset(); }
};

class Camera {
public:
  Camera() = default;
  Camera(math::Vec3f pos);

  // forward and up do not need to be orthonormal
  Camera(math::Vec3f pos, math::Vec3f forward, math::Vec3f up);

  math::Vec3f localPos() const;
  math::Vec3f localUp() const;
  math::Vec3f localRight() const;
  math::Vec3f localForward() const;
  math::Vec3f localFocalPoint() const;
  float focusDistance() const;

  void translate(math::Vec3f const &offset);

  void moveUp(float distance);
  void moveDown(float distance);
  void moveLeft(float distance);
  void moveRight(float distance);
  void moveForward(float distance);
  void moveBackward(float distance);

  void rotateUp(float angleDegrees);
  void rotateDown(float angleDegrees);
  void rotateLeft(float angleDegrees);
  void rotateRight(float angleDegrees);

  void rollLeft(float angleDegrees);
  void rollRight(float angleDegrees);

  void rotateLeftAroundPoint(math::Vec3f const &point, float angleDegrees);
  void rotateRightAroundPoint(math::Vec3f const &point, float angleDegrees);
  void rotateUpAroundPoint(math::Vec3f const &point, float angleDegrees);
  void rotateDownAroundPoint(math::Vec3f const &point, float angleDegrees);

  void rotateLeftAroundFocus(float angleDegrees);
  void rotateRightAroundFocus(float angleDegrees);
  void rotateUpAroundFocus(float angleDegrees);
  void rotateDownAroundFocus(float angleDegrees);

  void setFocusPoint(math::Vec3f const &point);
  void moveTowardFocus(float distance);
  void focusDistance(float distance);

private:
  float m_focalDistance = 1.f;
  math::Vec3f m_pos = math::Vec3f{0.f, 0.f, 0.f};
  math::Vec3f m_forward = math::Vec3f{0.f, 0.f, -1.f};
  math::Vec3f m_up = math::Vec3f{0.f, 1.f, 0.f};
};

// Free functions
math::Mat4f makeViewMatrix(Camera const &c);
math::Mat4f makeInverseViewMatrix(Camera const &c);

} // namespace scene
} // namespace openGL
