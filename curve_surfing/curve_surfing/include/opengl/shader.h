#pragma once

#include <string>

#include <glad/glad.h>

namespace opengl {

class Shader {
public:
  enum Type : uint32_t {
    INVALID = GL_INVALID_ENUM,
    VERTEX = GL_VERTEX_SHADER,
    GEOMETRY = GL_GEOMETRY_SHADER,
    FRAGMENT = GL_FRAGMENT_SHADER
  };

  ~Shader();

  /* Explicitly remove copy/assignment (RAII) */
  Shader(Shader const &) = delete;
  Shader &operator=(Shader const &) = delete;

  /* Retain move semantics */
  Shader(Shader &&other);
  Shader &operator=(Shader &&other);

  bool isValid() const;
  GLuint id() const;
  Shader::Type type() const;

private:
  /* Only called through makeShader() factory function */
  Shader(GLuint id, Shader::Type shaderType);

  void release();

  friend Shader makeShader(std::string const &shaderCode,
                           Shader::Type shaderType);
  friend void swap(Shader &lhs, Shader &rhs);

private:
  GLuint m_id = 0;
  Shader::Type m_shaderType = INVALID;
};

Shader makeShader(std::string const &shaderCode, GLenum shaderType);
void swap(Shader &lhs, Shader &rhs);

Shader::Type enumToShaderType(GLenum shaderTypeEnum);
bool checkCompileStatus(GLuint shaderID);

// SHADER FILE I/O
std::string loadShaderStringFromFile(std::string const &filePath);

} // namespace opengl
