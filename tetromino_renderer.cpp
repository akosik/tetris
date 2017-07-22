#include "tetromino_renderer.h"

const GLint TetrominoRenderer::kHiddenLines_ = 2;

TetrominoRenderer::TetrominoRenderer(GLfloat tile_size, const TextureRenderer& texture_renderer, const std::vector<Texture>& tile_textures, GLfloat playfield_x_origin, GLfloat playfield_y_origin, GLint nrows, GLint ncols) : tile_size_(tile_size), texture_renderer_(texture_renderer), tile_textures_(tile_textures), playfield_x_origin_(playfield_x_origin), playfield_y_origin_(playfield_y_origin), nrows_(nrows), ncols_(ncols)
{ }

void TetrominoRenderer::Render(GLfloat x, GLfloat y, TetroType type) const
{
  Tetromino tetro(type);
  TileColor color = tetro.Color();
  const std::vector<TileColor>& shape = tetro.Shape();
  GLuint template_side_length = tetro.TemplateSideLength();
  
  for (GLuint template_row = 0; template_row < template_side_length; ++template_row)
    {
      for (GLuint template_col = 0; template_col < template_side_length; ++template_col)
	{
	  if (shape[template_row * template_side_length + template_col] != kEmpty)
	    {
	      texture_renderer_.Render(tile_textures_[color], x + template_col * tile_size_, y - template_row * tile_size_, tile_size_, tile_size_);
	    }
	}
    }
}

void TetrominoRenderer::RenderCentered(GLfloat x, GLfloat y, GLfloat w, GLfloat h, TetroType type) const
{
  Tetromino tetro(type);
  GLfloat tetro_width = tetro.TemplateSideLength() * tile_size_;
  Render(x + ((w - tetro_width) / 2), y, type);
}

void TetrominoRenderer::RenderOnPlayfield(GLint row, GLint col, const Tetromino& tetro) const
{
  TileColor color = tetro.Color();
  const std::vector<TileColor>& shape = tetro.Shape();
  GLuint template_side_length = tetro.TemplateSideLength();
  GLfloat x = col * tile_size_ + playfield_x_origin_;
  GLfloat y = row * tile_size_ + playfield_y_origin_;
  
  for (GLuint template_row = 0; template_row < template_side_length; ++template_row)
    {
      for (GLuint template_col = 0; template_col < template_side_length; ++template_col)
	{
	  if (shape[template_row * template_side_length + template_col] != kEmpty && row - template_row < nrows_ - kHiddenLines_)
	    {
	      texture_renderer_.Render(tile_textures_[color], x + template_col * tile_size_, y - template_row * tile_size_, tile_size_, tile_size_);
	    }
	}
    }
}

TetrominoRenderer::~TetrominoRenderer()
{
  
}
