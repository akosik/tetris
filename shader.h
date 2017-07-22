#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/glew.h>
#include <glm/glm.hpp>

class Shader
{
public:
  Shader(const std::string &filename);

  void Bind() const;
  
  virtual ~Shader();

  GLuint GetId() const { return id_; }
  
  void SetBool(const std::string& name, const bool value) const;
  void SetInt(const std::string& name, const int value) const;
  void SetFloat(const std::string& name, const float value) const;
  void SetTransform(const std::string& name, const glm::mat4& value) const;
  void Set3f(const std::string& name, const glm::vec3 value) const;
  void Set4f(const std::string& name, const glm::vec4 value) const;
  
private:
  Shader(const Shader &) {};
  void operator=(const Shader &) {};

  static const unsigned int NUM_SHADERS = 2;
  GLuint id_;
  GLuint shaders_[NUM_SHADERS];
};


#endif // SHADER_H
