#include "camera.h"

#include "openglmatrix.h"

#include <iostream>

using namespace math;

namespace openGL {
namespace scene {

Camera::Camera(Vec3f pos) : m_pos(pos) {
}

Camera::Camera(Vec3f pos, Vec3f forward, Vec3f up)
    : m_pos(pos), m_forward(forward), m_up(up) {
  Vec3f right = m_forward ^ m_up;
  m_up = normalized(right ^ m_forward);
}

Vec3f Camera::localPos() const { return m_pos; }

Vec3f Camera::localUp() const { return m_up; }

Vec3f Camera::localRight() const { return normalized(m_forward ^ m_up); }

Vec3f Camera::localForward() const { return m_forward; }

Vec3f Camera::localFocalPoint() const {
  return m_pos + m_forward * m_focalDistance;
}

float Camera::focusDistance() const { return m_focalDistance; }

void Camera::translate(Vec3f const &offset) { m_pos += offset; }

void Camera::moveUp(float distance) { translate(distance * localUp()); }

void Camera::moveDown(float distance) { translate((-distance) * localUp()); }

void Camera::moveLeft(float distance) { translate((-distance) * localRight()); }

void Camera::moveRight(float distance) { translate(distance * localRight()); }

void Camera::moveForward(float distance) {
  translate(distance * localForward());
}

void Camera::moveBackward(float distance) {
  translate((-distance) * localForward());
}

void Camera::rotateUp(float angleDegrees) {
  Vec3f right = localRight();
  m_forward = rotateAroundNormalizedAxis(m_forward, right, angleDegrees);

  m_forward.normalize();
  m_up = right ^ m_forward;
  m_up.normalize();
}

void Camera::rotateDown(float angleDegrees) { rotateUp(-angleDegrees); }

void Camera::rotateLeft(float angleDegrees) {
  m_forward = rotateAroundNormalizedAxis(m_forward, m_up, angleDegrees);
  m_forward.normalize();
}

void Camera::rotateRight(float angleDegrees) { rotateLeft(-angleDegrees); }

void Camera::rotateLeftAroundPoint(Vec3f const &point, float angleDegrees) {
  Vec3f offset = m_pos - point;
  offset = rotateAroundNormalizedAxis(offset, m_up, angleDegrees);
  m_forward = -normalized(offset);
  m_pos = point + offset;
}

void Camera::rotateRightAroundPoint(Vec3f const &point, float angleDegrees) {
  rotateLeftAroundPoint(point, -angleDegrees);
}

void Camera::rotateUpAroundPoint(Vec3f const &point, float angleDegrees) {
  Vec3f offset = m_pos - point;
  Vec3f right = localRight();

  offset = rotateAroundNormalizedAxis(offset, right, angleDegrees);
  m_up = rotateAroundNormalizedAxis(m_up, right, angleDegrees);

  m_forward = -normalized(offset);
  m_pos = point + offset;
}

void Camera::rotateDownAroundPoint(Vec3f const &point, float angleDegrees) {
  rotateUpAroundPoint(point, -angleDegrees);
}

void Camera::rotateLeftAroundFocus(float angleDegrees) {
  rotateLeftAroundPoint(localFocalPoint(), angleDegrees);
}

void Camera::rotateRightAroundFocus(float angleDegrees) {
  rotateLeftAroundPoint(localFocalPoint(), -angleDegrees);
}

void Camera::rotateUpAroundFocus(float angleDegrees) {
  rotateUpAroundPoint(localFocalPoint(), angleDegrees);
}

void Camera::rotateDownAroundFocus(float angleDegrees) {
  rotateUpAroundPoint(localFocalPoint(), -angleDegrees);
}

void Camera::rollRight(float angleDegrees) {
  m_up = rotateAroundNormalizedAxis(m_up, m_forward, angleDegrees);
  m_up.normalize();
}

void Camera::rollLeft(float angleDegrees) { rollRight(-angleDegrees); }

void Camera::setFocusPoint(const Vec3f &point) {
  m_pos = point - m_focalDistance * m_forward;
}

void Camera::moveTowardFocus(float distance) {
  if (m_focalDistance - distance < 0.1f)
    distance = 0;
  m_focalDistance -= distance;
  moveForward(distance);
}

void Camera::focusDistance(float distance) { m_focalDistance = distance; }

// Free function

Mat4f makeViewMatrix(Camera const &c) {
  return LookAtMatrix(c.localPos(), c.localFocalPoint(), c.localUp());
}

Mat4f makeInverseViewMatrix(Camera const &c) {
  return inverseLookAtMatrix(c.localPos(), c.localFocalPoint(), c.localUp());
}

} // namespace scene
} // namespace openGL
