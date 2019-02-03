/** For some derivations see:
 * http://www.glprogramming.com/red/appendixf.html
 * http://www.songho.ca/opengl/gl_projectionmatrix.html
 */

#include "openglmatrix.h"

#include <cmath>

using namespace math;

namespace openGL {

Mat4f UniformScaleMatrix(float scale) {
  Mat4f uniform = {
      scale, 0.f,   0.f,   0.f, //
      0.f,   scale, 0.f,   0.f, //
      0.f,   0.f,   scale, 0.f, //
      0.f,   0.f,   0.f,   1.f  //
  };

  return uniform;
}

Mat4f ScaleMatrix(float x, float y, float z) {
  Mat4f scale = {
      x,   0.f, 0.f, 0.f, //
      0.f, y,   0.f, 0.f, //
      0.f, 0.f, z,   0.f, //
      0.f, 0.f, 0.f, 1.f  //
  };

  return scale;
}

Mat4f ScaleMatrix(Vec3f const &s) {
  Mat4f scale = {
      s.m_x, 0.f,   0.f,   0.f, //
      0.f,   s.m_y, 0.f,   0.f, //
      0.f,   0.f,   s.m_z, 0.f, //
      0.f,   0.f,   0.f,   1.f  //
  };

  return scale;
}

Mat4f TranslateMatrix(float x, float y, float z) {
  Mat4f trans = {
      1.f, 0.f, 0.f, x,  //
      0.f, 1.f, 0.f, y,  //
      0.f, 0.f, 1.f, z,  //
      0.f, 0.f, 0.f, 1.f //
  };

  return trans;
}

Mat4f TranslateMatrix(Vec3f const &pos) {
  Mat4f trans = {
      1.f, 0.f, 0.f, pos.m_x, // 1 0 0 x
      0.f, 1.f, 0.f, pos.m_y, // 0 1 0 y
      0.f, 0.f, 1.f, pos.m_z, // 0 0 1 z
      0.f, 0.f, 0.f, 1.f      // 0 0 0 1
  };

  return trans;
}

Mat4f RotateAboutXMatrix(float angleDeg) {
  float angleRad = angleDeg * (M_PI / 180.f);

  float c = std::cos(angleRad);
  float s = std::sin(angleRad);

  Mat4f rot = {
      1.f, 0.f, 0.f, 0.f, //
      0.f, c,   -s,  0.f, //
      0.f, s,   c,   0.f, //
      0.f, 0.f, 0.f, 1.f  //
  };
  return rot;
}

Mat4f RotateAboutYMatrix(float angleDeg) {
  float angleRad = angleDeg * (M_PI / 180.f);

  float c = std::cos(angleRad);
  float s = std::sin(angleRad);

  Mat4f rot = {
      c,   0.f, s,   0.f, //
      0.f, 1.f, 0.f, 0.f, //
      -s,  0.f, c,   0.f, //
      0.f, 0.f, 0.f, 1.f  //
  };

  return rot;
}

Mat4f RotateAboutZMatrix(float angleDeg) {
  float angleRad = angleDeg * (M_PI / 180.f);

  float c = std::cos(angleRad);
  float s = std::sin(angleRad);

  Mat4f rot = {
      c,   -s,  0.f, 0.f, //
      s,   c,   0.f, 0.f, //
      0.f, 0.f, 1.f, 0.f, //
      0.f, 0.f, 0.f, 1.f  //
  };

  return rot;
}

Mat4f OrthographicProjection(float l, float r, float t, float b, float n,
                             float f) {
  float a00 = 2.f / (r - l);
  float a03 = -(r + l) / (r - l);
  float a11 = 2.f / (t - b);
  float a13 = -(t + b) / (t - b);
  float a22 = -2.f / (f - n);
  float a23 = -(f + n) / (f - n);

  Mat4f ortho = {
      a00, 0.f, 0.f, a03, //
      0.f, a11, 0.f, a13, //
      0.f, 0.f, a22, a23, //
      0.f, 0.f, 0.f, 1.f  //
  };
  return ortho;
}

Mat4f SymmetricOrthographicProjection(float r, float t, float n, float f) {
  float a00 = 1.f / r;
  float a11 = 1.f / t;
  float a22 = -2.f / (f - n);
  float a23 = -(f + n) / (f - n);

  Mat4f ortho = {
      a00, 0.f, 0.f, 0.f, //
      0.f, a11, 0.f, 0.f, //
      0.f, 0.f, a22, a23, //
      0.f, 0.f, 0.f, 1.f  //
  };
  return ortho;
}

Mat4f FrustumProjection(float l, float r, float t, float b, float n, float f) {
  float a00 = (2.f * n) / (r - l);
  float a02 = (r + l) / (r - l);
  float a11 = (2.f * n) / (t - b);
  float a12 = (t + b) / (t - b);
  float a22 = -(f + n) / (f - n);
  float a23 = -2.f * f * n / (f - n);

  Mat4f frustum = {
      a00, 0.f, a02,  0.f, // 0 1 2 3
      0.f, a11, a12,  0.f, // 4 5 6 7
      0.f, 0.f, a22,  a23, // 8 9 10 11
      0.f, 0.f, -1.f, 0.f  // 12 13 14 15
  };
  return frustum;
}

Mat4f inverseFrustumProjection(float l, float r, float t, float b, float n,
                               float f) {
  float a00 = (r - l) * (2.f * n);
  float a03 = (r + l) / (2.f * n);
  float a11 = (t - b) / (2.f * n);
  float a13 = (t + b) / (2.f * n);
  float a32 = -(f - n) / (2.f * f * n);
  float a33 = (f + n) / (2.f * f * n);

  Mat4f frustum = {
      a00, 0.f, 0.f, a03,  // 0 1 2 3
      0.f, a11, 0.f, a13,  // 4 5 6 7
      0.f, 0.f, 0.f, -1.f, // 8 9 10 11
      0.f, 0.f, a32, a33   // 12 13 14 15
  };
  return frustum;
}

Mat4f SymmetricFrustumProjection(float r, float t, float n, float f) {
  float a00 = n / r;
  float a11 = n / t;
  float a22 = -(f + n) / (f - n);
  float a23 = -2.f * f * n / (f - n);

  Mat4f symmetricFrustum = {
      a00, 0.f, 0.f,  0.f, // 0 1 2 3
      0.f, a11, 0.f,  0.f, // 4 5 6 7
      0.f, 0.f, a22,  a23, // 8 9 10 11
      0.f, 0.f, -1.f, 0.f  // 12 13 14 15
  };
  return symmetricFrustum;
}

Mat4f inverseSymmetricFrustumProjection(float r, float t, float n, float f) {
  float a00 = r / n;
  float a11 = t / n;
  float a32 = -(f - n) / (2.f * f * n);
  float a33 = (f + n) / (2.f * f * n);

  Mat4f invSymmetricFrustum = {
      a00, 0.f, 0.f, 0.f,  // 0 1 2 3
      0.f, a11, 0.f, 0.f,  // 4 5 6 7
      0.f, 0.f, a00, -1.f, // 8 9 10 11
      0.f, 0.f, a32, a33   // 12 13 14 15
  };
  return invSymmetricFrustum;
}

Mat4f PerspectiveProjection(float fovDegrees, float aspectRatio, float zNear,
                            float zFar) {
  float top = std::tan(fovDegrees * (M_PI / 180.f) * (1.f / 2.f)) * zNear;
  float right = top * aspectRatio;

  return SymmetricFrustumProjection(right, top, zNear, zFar);
}

Mat4f inversePerspectiveProjection(float fovDegrees, float aspectRatio,
                                   float zNear, float zFar) {
  float top = std::tan(fovDegrees * (M_PI / 180.f) * (1.f / 2.f)) * zNear;
  float right = top * aspectRatio;

  return inverseSymmetricFrustumProjection(right, top, zNear, zFar);
}

Mat4f LookAtMatrix(const Vec3f &pos, const Vec3f &target, const Vec3f &up) {
  Vec3f f = pos - target; // inverted for R-handed CS
  f.normalize();
  Vec3f u = normalized(up);
  Vec3f r = normalized(u ^ f);
  u = normalized(f ^ r);

  Mat4f view = {
      r.m_x, r.m_y, r.m_z, -r * pos, //
      u.m_x, u.m_y, u.m_z, -u * pos, //
      f.m_x, f.m_y, f.m_z, -f * pos, //
      0.f,   0.f,   0.f,   1.f       //
  };
  return view;
}

Mat4f inverseLookAtMatrix(const Vec3f &pos, const Vec3f &target,
                          const Vec3f &up) {
  Vec3f f = pos - target; // inverted for R-handed CS
  f.normalize();
  Vec3f u = normalized(up);
  Vec3f r = normalized(u ^ f);
  u = normalized(f ^ r);

  Mat4f invView = {
      r.m_x, u.m_x, f.m_x, r * pos, //
      r.m_y, u.m_y, f.m_y, u * pos, //
      r.m_z, u.m_z, f.m_z, f * pos, //
      0.f,   0.f,   0.f,   1.f      //
  };
  return invView;
}

} // namespace openGL
