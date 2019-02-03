#include "program.h"

#include <iostream>
#include <vector>

namespace opengl {

Program::Program(GLuint programID) : m_id(programID) {}

Program::~Program() { release(); }

Program::Program(Program &&other) : m_id(other.m_id) { other.m_id = 0; }

Program &Program::operator=(Program &&other) {
  if (this != &other) {
    release();
    std::swap(m_id, other.m_id);
  }
  return *this;
}

bool Program::isValid() const { return m_id > 0; }

void Program::use() const { glUseProgram(m_id); }

void Program::setUniformVec3f(std::string const &name, float x, float y,
                              float z) {
  glUniform3f(glGetUniformLocation(m_id, name.c_str()), x, y, z);
}

void Program::setUniformVec3f(std::string const &name, math::Vec3f const &vec) {
  glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, vec.data());
}

void Program::setUniformVec3f(std::string const &name, GLuint count,
                              float const *vecPtr) {
  glUniform3fv(glGetUniformLocation(m_id, name.c_str()), count, vecPtr);
}

void Program::setUniformVec3f(GLuint uniformLocation, float x, float y,
                              float z) {
  glUniform3f(uniformLocation, x, y, z);
}

void Program::setUniformVec3f(GLuint uniformLocation, math::Vec3f const &vec) {
  glUniform3fv(uniformLocation, 1, vec.data());
}

void Program::setUniformVec3f(GLuint uniformLocation, GLuint count,
                              float const *vecPtr) {
  glUniform3fv(uniformLocation, count, vecPtr);
}

void Program::setUniformMat4f(const std::string &name, math::Mat4f const &value,
                              GLboolean applyTranspose) {
  glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1,
                     applyTranspose, value.data());
}

void Program::setUniformMat4f(GLuint uniformLocation, math::Mat4f const &value,
                              GLboolean applyTranspose) {
  glUniformMatrix4fv(uniformLocation, 1, applyTranspose, value.data());
}

void Program::setUniform1f(std::string const &name, float value) {
  glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void Program::setUniform1f(GLuint uniformLocation, float value) {
  glUniform1f(uniformLocation, value);
}

void Program::release() {
  glDeleteProgram(m_id);
  m_id = 0;
}

Program makeProgram(std::string const &vertexShaderSource,
                    std::string const &fragmentShaderSource) {
  auto vs = makeShader(vertexShaderSource, Shader::VERTEX);
  if (!vs.isValid()) {
    std::cerr << "Failed to compile vertex shader\n";
    return Program(0);
  }
  auto fs = makeShader(fragmentShaderSource, Shader::FRAGMENT);
  if (!fs.isValid()) {
    std::cerr << "Failed to compile fragment shader\n";
    return Program(0);
  }
  return makeProgram(vs, fs);
}

Program makeProgram(Shader const &vertexShader, Shader const &fragmentShader) {
  if (!vertexShader.isValid() || !fragmentShader.isValid()) {
    return Program(0);
  }

  if ((vertexShader.type() != Shader::VERTEX) ||
      (fragmentShader.type() != Shader::FRAGMENT)) {
    return Program(0);
  }

  GLuint programID = glCreateProgram();
  if (programID == 0) {
    return Program(0);
  }

  glAttachShader(programID, vertexShader.id());
  glAttachShader(programID, fragmentShader.id());

  glLinkProgram(programID);

  glDetachShader(programID, vertexShader.id());
  glDetachShader(programID, fragmentShader.id());

  if (!checkLinkStatus(programID)) {
    glDeleteProgram(programID);
    return Program(0);
  }

  return Program(programID);
}

Program makeProgram(Shader const &vertexShader, Shader const &geometryShader,
                    Shader const &fragmentShader) {
  if (!vertexShader.isValid() || !fragmentShader.isValid() ||
      !geometryShader.isValid()) {
    return Program(0);
  }

  if ((vertexShader.type() != Shader::VERTEX) ||
      (geometryShader.type() != Shader::GEOMETRY) ||
      (fragmentShader.type() != Shader::FRAGMENT)) {
    return Program(0);
  }

  GLuint programID = glCreateProgram();
  if (programID == 0) {
    return Program(0);
  }

  glAttachShader(programID, vertexShader.id());
  glAttachShader(programID, geometryShader.id());
  glAttachShader(programID, fragmentShader.id());

  glLinkProgram(programID);

  glDetachShader(programID, vertexShader.id());
  glDetachShader(programID, geometryShader.id());
  glDetachShader(programID, fragmentShader.id());

  if (!checkLinkStatus(programID)) {
    glDeleteProgram(programID);
    return Program(0);
  }

  return Program(programID);
}

bool checkLinkStatus(GLuint programID) {
  GLint result;
  int infoLogLength;
  glGetProgramiv(programID, GL_LINK_STATUS, &result);
  if (!result) {
    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
    std::vector<char> errorMsg(infoLogLength);
    glGetProgramInfoLog(programID, infoLogLength, nullptr, errorMsg.data());
    std::cerr << errorMsg.data() << std::endl;
    return false;
  }

  return true;
}

} // namespace opengl
