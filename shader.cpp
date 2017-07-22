#include "shader.h"

#include <iostream>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

static void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string &error_message);
static std::string LoadShader(const std::string &filename);
static GLuint CreateShader(const std::string &text, GLenum shader_type);

Shader::Shader(const std::string& filename)
{
  id_ = glCreateProgram();
  shaders_[0] = CreateShader(LoadShader(filename + ".vert"), GL_VERTEX_SHADER);
  shaders_[1] = CreateShader(LoadShader(filename + ".frag"), GL_FRAGMENT_SHADER);

  for(unsigned int i = 0; i < NUM_SHADERS; ++i)
    glAttachShader(id_, shaders_[i]);

  glLinkProgram(id_);
  CheckShaderError(id_, GL_LINK_STATUS, true, "Error: Program failed to link: ");
}

void Shader::Bind() const
{
  glUseProgram(id_);
}

Shader::~Shader()
{
  for(unsigned int i = 0; i < NUM_SHADERS; ++i)
    {
      glDetachShader(id_, shaders_[i]);
      glDeleteShader(shaders_[i]);
    }
  glDeleteShader(id_);
}

void CheckShaderError(GLuint shader, GLuint flag, bool isProgram, const std::string& error_message)
{
  GLint success = 0;
  GLchar error[1024] = { 0 };

  if(isProgram)
    glGetProgramiv(shader, flag, &success);
  else
    glGetShaderiv(shader, flag, &success);

  if(success == GL_FALSE)
    {
      if(isProgram)
	glGetProgramInfoLog(shader, sizeof(error), NULL, error);
      else
	glGetShaderInfoLog(shader, sizeof(error), NULL, error);

      std::cerr << error_message << ": '" << error << "'" << std::endl;
    }
}

std::string LoadShader(const std::string& filename)
{

  std::ifstream file;
  std::string output, line;
  file.open(filename.c_str());

  if(file.is_open())
    {
      while(file.good())
	{
	  getline(file, line);
	  output.append(line + "\n");
	}
    }
  else
    {
      std::cerr << "Unable to load Shader: " << filename << std::endl;
    }

  return output;
}

GLuint CreateShader(const std::string& text, GLenum shader_type)
{
  GLuint shader = glCreateShader(shader_type);

  if(shader == 0)
    std::cerr << "Error: Shader Creation Failed" << std::endl;

  const GLchar *shader_source_strings[1];
  GLint shader_source_string_lengths[1];

  shader_source_strings[0] = text.c_str();
  shader_source_string_lengths[0] = text.length();

  glShaderSource(shader, 1, shader_source_strings, shader_source_string_lengths);
  glCompileShader(shader);

  CheckShaderError(shader, GL_COMPILE_STATUS, false, "Error: Shader failed to compile: ");
  
  return shader;
}


void Shader::SetBool(const std::string &name, const bool value) const
{         
    glUniform1i(glGetUniformLocation(id_, name.c_str()), (int)value); 
}
void Shader::SetInt(const std::string &name, const int value) const
{ 
    glUniform1i(glGetUniformLocation(id_, name.c_str()), value); 
}
void Shader::SetFloat(const std::string &name, const float value) const
{ 
    glUniform1f(glGetUniformLocation(id_, name.c_str()), value); 
} 

void Shader::SetTransform(const std::string &name, const glm::mat4& value) const
{
  glUniformMatrix4fv(glGetUniformLocation(id_, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::Set3f(const std::string &name, const glm::vec3 value) const
{
  glUniform3f(glGetUniformLocation(id_, name.c_str()), value.x, value.y, value.z);
}

void Shader::Set4f(const std::string &name, const glm::vec4 value) const
{
  glUniform4f(glGetUniformLocation(id_, name.c_str()), value.x, value.y, value.z, value.w);
}
