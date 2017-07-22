#ifndef TEXTURE_RENDERER_H
#define TEXTURE_RENDERER_H

#include "texture.h"
#include "shader.h"

#include <glm/glm.hpp>
#include <GL/glew.h>

class TextureRenderer
{
public:
  TextureRenderer(const std::string& texture_shaders_prefix,
		  const std::string& color_shaders_prefix,
		  const glm::mat4& projection);

  void Render(const Texture& texture, GLfloat x, GLfloat y, GLfloat w, GLfloat h) const;
  void TransparentRender(const Texture& texture, GLfloat x, GLfloat y, GLfloat w, GLfloat h, GLfloat mix_ratio, glm::vec3 mix_color) const;
  void ColorRender(const glm::vec3& color, GLfloat x, GLfloat y, GLfloat w, GLfloat h) const;
  
  virtual ~TextureRenderer();
private:
  glm::mat4 projection_;
  Shader texture_shader_, color_shader_;
  GLuint vao_;
};


#endif //TEXTURE_RENDERER_H
