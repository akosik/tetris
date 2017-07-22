#ifndef TETROMINO_RENDERER_H
#define TETROMINO_RENDERER_H

#include "texture.h"
#include "texture_renderer.h"
#include "tetromino.h"

#include <vector>

class TetrominoRenderer
{
public:
  TetrominoRenderer(GLfloat tile_size,
		    const TextureRenderer& texture_renderer,
		    const std::vector<Texture>& tile_textures,
		    GLfloat playfield_x_origin,
		    GLfloat playfield_y_origin,
		    GLint ncols,
		    GLint nrows);
  
  void Render(GLfloat x, GLfloat y, TetroType type) const;
  void RenderCentered(GLfloat x, GLfloat y, GLfloat w, GLfloat h, TetroType type) const;
  void RenderOnPlayfield(GLint row, GLint col, const Tetromino& tetro) const;
  
  virtual ~TetrominoRenderer();
private:
  const GLfloat tile_size_;
  const TextureRenderer& texture_renderer_;
  const std::vector<Texture>& tile_textures_;
  const GLfloat playfield_x_origin_;
  const GLfloat playfield_y_origin_;
  const GLint nrows_, ncols_;
  static const GLint kHiddenLines_;
};


#endif // TETROMINO_RENDERER_H
