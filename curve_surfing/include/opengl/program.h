#pragma once

#include <string>

#include <glad/glad.h>

#include "mat4f.h"
#include "shader.h"
#include "vec3f.h"

namespace opengl {

class Program {
public:
  ~Program();

  /* Explicitly remove copy/assignment (RAII) */
  Program(Program const &) = delete;
  Program &operator=(Program const &) = delete;

  /* Retain move semantics */
  Program(Program &&);
  Program &operator=(Program &&other);

  void use() const;

  bool isValid() const;

  /* Bind function use() must be called before uniform setting functions may be
   * called. Failure to call use() before calling these functions will result in
   * undefined behavior (i.e. other programs may be affected) */
  void setUniformVec3f(std::string const &name, float x, float y, float z);
  void setUniformVec3f(std::string const &name, math::Vec3f const &value);
  void setUniformVec3f(std::string const &name, GLuint count,
                       float const *vecPtr);

  void setUniformVec3f(GLuint uniformLocation, float x, float y, float z);
  void setUniformVec3f(GLuint uniformLocation, math::Vec3f const &value);
  void setUniformVec3f(GLuint uniformLocation, GLuint count,
                       float const *vecPtr);

  void setUniformMat4f(std::string const &name, math::Mat4f const &value,
                       GLboolean applyTranspose = GL_FALSE);
  void setUniformMat4f(GLuint uniformLocation, math::Mat4f const &value,
                       GLboolean applyTranspose = GL_FALSE);

  void setUniform1f(std::string const &name, float value);
  void setUniform1f(GLuint uniformLocation, float value);

private:
  /* Only called through makeProgram() factory function */
  Program(GLuint programID);

  void release();

  friend Program makeProgram(std::string const &vertexShaderSource,
                             std::string const &fragmentShaderSource);

  friend Program makeProgram(Shader const &vertexShader,
                             Shader const &fragmentShader);
  friend Program makeProgram(Shader const &vertexShader,
                             Shader const &geometryShader,
                             Shader const &fragmentShader);

private:
  GLuint m_id = 0;
};

Program makeProgram(std::string const &vertexShaderSource,
                    std::string const &fragmentShaderSource);

Program makeProgram(Shader const &vertexShader, Shader const &fragmentShader);
Program makeProgram(Shader const &vertexShader, Shader const &geometryShader,
                    Shader const &fragmentShader);

bool checkLinkStatus(GLuint programID);

} // namespace opengl
