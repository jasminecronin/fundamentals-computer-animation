#include "vec3f.h"

#include <cmath>
#include <iostream>

namespace math {

// Member function interface
// TODO added to appese clang: initialized {0,0,0} bug
Vec3f::Vec3f() : m_x(0.f), m_y(0.f), m_z(0.f) {}

Vec3f::Vec3f(float x, float y, float z) : m_x(x), m_y(y), m_z(z) {}

Vec3f &Vec3f::operator+=(Vec3f const &rhs) {
  m_x += rhs.m_x;
  m_y += rhs.m_y;
  m_z += rhs.m_z;
  return *this;
}
Vec3f &Vec3f::operator-=(Vec3f const &rhs) {
  m_x -= rhs.m_x;
  m_y -= rhs.m_y;
  m_z -= rhs.m_z;
  return *this;
}

Vec3f &Vec3f::operator*=(float rhs) {
  m_x *= rhs;
  m_y *= rhs;
  m_z *= rhs;
  return *this;
}

Vec3f &Vec3f::operator/=(float rhs) {
  m_x /= rhs;
  m_y /= rhs;
  m_z /= rhs;
  return *this;
}

float &Vec3f::operator[](size_t id) { return m_coord[id]; }

float const &Vec3f::operator[](size_t id) const { return m_coord[id]; }

float *Vec3f::data() { return &m_coord[0]; }

float const *Vec3f::data() const { return &m_coord[0]; }

Vec3f &Vec3f::normalize() {
  float l = norm(*this);
  return (*this) /= l;
}

void Vec3f::zero() {
  m_x = 0.f;
  m_y = 0.f;
  m_z = 0.f;
}

// Free function
/*
 * Vector-Vector Addition/ Subtraction
 */
Vec3f operator+(Vec3f const &a, Vec3f const &b) {
  return Vec3f(a.m_x + b.m_x, a.m_y + b.m_y, a.m_z + b.m_z);
}
Vec3f operator-(Vec3f const &a, Vec3f const &b) {
  return Vec3f(a.m_x - b.m_x, a.m_y - b.m_y, a.m_z - b.m_z);
}

/*
 * Scalar-Vector Multiplication/Division
 */
Vec3f operator*(float s, Vec3f v) {
  v.m_x *= s;
  v.m_y *= s;
  v.m_z *= s;
  return v;
}
Vec3f operator*(Vec3f v, float s) { return s * v; }

Vec3f operator/(Vec3f v, float s) {
  v.m_x /= s;
  v.m_y /= s;
  v.m_z /= s;
  return v;
}

/*
 * Negation of vector
 * -v = (-1.f) * v
 */
Vec3f operator-(Vec3f v) {
  v.m_x = -v.m_x;
  v.m_y = -v.m_y;
  v.m_z = -v.m_z;
  return v;
}

/*
 * Vector-Vector (inner/dot) product
 */
float operator*(Vec3f const &a, Vec3f const &b) {
  return a.m_x * b.m_x + a.m_y * b.m_y + a.m_z * b.m_z;
}
float dot(Vec3f const &a, Vec3f const &b) { return a * b; }

/*
 * Vector-Vector cross product
 */
Vec3f operator^(Vec3f const &a, Vec3f const &b) {
  return Vec3f(a.m_y * b.m_z - a.m_z * b.m_y, a.m_z * b.m_x - a.m_x * b.m_z,
               a.m_x * b.m_y - a.m_y * b.m_x);
} Vec3f cross(Vec3f const &a, Vec3f const &b) {
  return a ^ b;
}

/*
 * Vector norm (length)
 */
float norm(Vec3f const &v) {
  return std::sqrt(v.m_x * v.m_x + v.m_y * v.m_y + v.m_z * v.m_z);
}
float normSquared(Vec3f const &v) {
  return v.m_x * v.m_x + v.m_y * v.m_y + v.m_z * v.m_z;
}

/*
 * Normalized Vector
 */
Vec3f normalized(Vec3f v) {
  float l = norm(v);
  return v /= l;
}

/*
 * Linear interpolation
 */
Vec3f lerp(Vec3f const &a, Vec3f const &b, float t) {
  return (1.f - t) * a + t * b;
}

float distance(Vec3f const &a, Vec3f const &b) { return norm(a - b); }

float distanceSquared(Vec3f const &a, Vec3f const &b) {
  return normSquared(a - b);
}

Vec3f rotateAroundAxis(Vec3f v, Vec3f axis, float angleDegrees) {
  // Rodrigues formula
  // rotates a vector around an arbitrary axis by an angle (degrees)

  constexpr float degreesToRadians = M_PI / 180.f;
  float const sinTheta = std::sin(angleDegrees * degreesToRadians);
  float const cosTheta = std::cos(angleDegrees * degreesToRadians);

  axis.normalize();

  return v * cosTheta + (axis ^ v) * sinTheta +
         axis * ((axis * v) * (1.f - cosTheta));
}

Vec3f rotateAroundNormalizedAxis(Vec3f v, Vec3f const &axis,
                                 float angleDegrees) {
  // Rodrigues formula
  // rotates a vector around an arbitrary axis by an angle (degrees)

  constexpr float degreesToRadians = M_PI / 180.f;
  float const sinTheta = std::sin(angleDegrees * degreesToRadians);
  float const cosTheta = std::cos(angleDegrees * degreesToRadians);

  return v * cosTheta + (axis ^ v) * sinTheta +
         axis * ((axis * v) * (1.f - cosTheta));
}

Vec3f componentMultiplication(const Vec3f &lhs, const Vec3f &rhs) {
  return {lhs.m_x * rhs.m_x, lhs.m_y * rhs.m_y, lhs.m_z * rhs.m_z};
}

std::ostream &operator<<(std::ostream &out, Vec3f const &v) {
  return out << v.m_x << " " << v.m_y << " " << v.m_z;
}

std::istream &operator>>(std::istream &in, Vec3f &v) {
  return in >> v.m_x >> v.m_y >> v.m_z;
}

} // namespace math
