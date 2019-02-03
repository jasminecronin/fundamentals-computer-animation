#pragma once

#include "mat4f.h"
#include "vec3f.h"

namespace openGL {

math::Mat4f UniformScaleMatrix(float scale);
math::Mat4f ScaleMatrix(float x, float y, float z);
math::Mat4f ScaleMatrix(math::Vec3f const &scale);
math::Mat4f TranslateMatrix(float x, float y, float z);
math::Mat4f TranslateMatrix(math::Vec3f const &pos);
math::Mat4f RotateAboutXMatrix(float angleDeg);
math::Mat4f RotateAboutYMatrix(float angleDeg);
math::Mat4f RotateAboutZMatrix(float angleDeg);
math::Mat4f OrthographicProjection(float left, float right, float top,
                                   float bottom, float near, float far);
math::Mat4f SymmetricOrthographicProjection(float right, float top, float near,
                                            float far);
math::Mat4f FrustumProjection(float left, float right, float top, float bottom,
                              float near, float far);
math::Mat4f inverseFrustumProjection(float left, float right, float top,
                                     float bottom, float near, float far);
math::Mat4f SymmetricFrustumProjection(float right, float top, float near,
                                       float far);
math::Mat4f inverseSymmetricFrustumProjection(float right, float top,
                                              float near, float far);
math::Mat4f PerspectiveProjection(float fovDegrees, float aspectRatio,
                                  float zNear, float zFar);
math::Mat4f inversePerspectiveProjection(float fovDegrees, float aspectRaito,
                                         float zNear, float zFar);
math::Mat4f LookAtMatrix(const math::Vec3f &pos, const math::Vec3f &target,
                         const math::Vec3f &up);
math::Mat4f inverseLookAtMatrix(const math::Vec3f &pos,
                                const math::Vec3f &target,
                                const math::Vec3f &up);

} // namespace openGL
