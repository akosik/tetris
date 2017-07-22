#include <glm/gtx/transform.hpp>
#include <glm/ext.hpp>

#include "texture_renderer.h"

TextureRenderer::TextureRenderer(const std::string& texture_shaders_prefix,
				 const std::string& color_shaders_prefix,
				 const glm::mat4& projection)
  : projection_(projection),
    texture_shader_(texture_shaders_prefix),
    color_shader_(color_shaders_prefix)
{
  // Don't save these since we have the vao
  // (And we're not altering the shape)
  GLfloat unit_cube_vertices[] =
    {0, 0, 0, 1,
     0, 1, 0, 0,
     1, 0, 1, 1,
     1, 1, 1, 0};
    
  GLuint vbo;
  glGenVertexArrays(1, &vao_);
  glBindVertexArray(vao_);
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(unit_cube_vertices), unit_cube_vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*) (2 * sizeof(GLfloat)) );
  glEnableVertexAttribArray(1);
  glBindVertexArray(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void TextureRenderer::Render(const Texture& texture, GLfloat x, GLfloat y, GLfloat w, GLfloat h) const
{
  glm::mat4 transform(1.0);
  transform = glm::scale(transform, glm::vec3(w, h, 1));
  transform = glm::translate(transform, glm::vec3(x/w, y/h, 0));
  transform = projection_ * transform;
  texture_shader_.Bind();
  texture_shader_.SetTransform("transform", transform);
  texture_shader_.SetFloat("mix_ratio", 0);
  texture.Bind(0);
  glBindVertexArray(vao_);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void TextureRenderer::TransparentRender(const Texture& texture, GLfloat x, GLfloat y, GLfloat w, GLfloat h, GLfloat mix_ratio, glm::vec3 mix_color) const
{
  glm::mat4 transform(1.0);
  transform = glm::scale(transform, glm::vec3(w, h, 1));
  transform = glm::translate(transform, glm::vec3(x/w, y/h, 0));
  transform = projection_ * transform;
  texture_shader_.Bind();
  texture_shader_.SetTransform("transform", transform);
  texture_shader_.SetFloat("mix_ratio", mix_ratio);
  texture_shader_.Set3f("mix_color", mix_color);
  texture.Bind(0);
  glBindVertexArray(vao_);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

void TextureRenderer::ColorRender(const glm::vec3& color, GLfloat x, GLfloat y, GLfloat w, GLfloat h) const
{
  glm::mat4 transform(1.0);
  transform = glm::scale(transform, glm::vec3(w, h, 1));
  transform = glm::translate(transform, glm::vec3(x/w, y/h, 0));
  transform = projection_ * transform;
  color_shader_.Bind();
  color_shader_.SetTransform("transform", transform);
  color_shader_.Set4f("in_color", glm::vec4(color,1));
  glBindVertexArray(vao_);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
  glBindVertexArray(0);
}

TextureRenderer::~TextureRenderer()
{
  glDeleteVertexArrays(1,&vao_);
}
