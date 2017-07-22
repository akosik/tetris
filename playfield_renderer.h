#ifndef PLAYFIELD_RENDERER_H
#define PLAYFIELD_RENDERER_H

#include "shader.h"
#include "texture.h"
#include "texture_renderer.h"
#include "playfield.h"

#include <glm/glm.hpp>
#include <vector>
#include <GL/glew.h>
#include <string>

class PlayFieldRenderer
{
public:
  PlayFieldRenderer(const std::string& shaders_prefix, glm::mat4& projection, GLfloat x, GLfloat y, GLint nrows, GLint ncols, GLfloat tile_size, const TextureRenderer& texture_renderer, const std::vector<Texture>& ttiles);

  void Render(const PlayField& pf) const;
  void RenderGrid() const;
  void RenderStack(const PlayField& pf) const;
  void LineClearAnimation(const PlayField& playfield, GLfloat progress_percent) const;

  virtual ~PlayFieldRenderer();

private:

  
  std::vector<GLfloat> playfield_outline_vertices_;
  GLuint vao_;
  Shader shader_;
  const GLint ncols_, nrows_;
  const GLfloat x_, y_;
  const GLfloat tile_size_;
  const std::vector<Texture>& tile_textures_;
  const TextureRenderer& texture_renderer_;
  static const GLint kHiddenRows_;
  static glm::vec3 kPlayfieldFill_;
  static glm::vec3 kPlayfieldLines_;
};


#endif //PLAYFIELD_RENDERER_H
