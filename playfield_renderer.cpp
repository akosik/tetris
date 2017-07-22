#include "playfield_renderer.h"

#define _USE_MATH_DEFINES
#include <math.h>

glm::vec3 PlayFieldRenderer::kPlayfieldFill_(0,0,0);
glm::vec3 PlayFieldRenderer::kPlayfieldLines_(30/255.f,30/255.f,30/255.f);
const GLint PlayFieldRenderer::kHiddenRows_ = 2;

PlayFieldRenderer::PlayFieldRenderer(const std::string& shaders_prefix, glm::mat4& projection, GLfloat x, GLfloat y, GLint nrows, GLint ncols, GLfloat tile_size, const TextureRenderer& texture_renderer, const std::vector<Texture>& ttiles) : x_(x), y_(y), nrows_(nrows), ncols_(ncols), shader_(shaders_prefix), tile_size_(tile_size), texture_renderer_(texture_renderer), tile_textures_(ttiles)
{
  GLfloat width = ncols * tile_size;
  GLfloat height = (nrows - kHiddenRows_) * tile_size;

  GLfloat a[8] = {x, y,
	  x, y + height,
	  x + width, y,
	  x + width, y + height};
  playfield_outline_vertices_.insert(playfield_outline_vertices_.end(), a, a+8);
  
  GLfloat xx = x;
  for (auto i = 0; i <= ncols; ++i)
    {
      playfield_outline_vertices_.push_back(xx);
      playfield_outline_vertices_.push_back(y);
      playfield_outline_vertices_.push_back(xx);
      playfield_outline_vertices_.push_back(y + height);
      xx += tile_size;
    }

  GLfloat yy = y;
  for (auto i = 0; i <= nrows - kHiddenRows_; ++i)
    {
      playfield_outline_vertices_.push_back(x);
      playfield_outline_vertices_.push_back(yy);
      playfield_outline_vertices_.push_back(x + width);
      playfield_outline_vertices_.push_back(yy);
      yy += tile_size;
    }

  GLuint vbo;
  glGenBuffers(1, &vbo);
  glGenVertexArrays(1,&vao_);
  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, playfield_outline_vertices_.size() * sizeof(playfield_outline_vertices_[0]), playfield_outline_vertices_.data(), GL_STATIC_DRAW);
  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(playfield_outline_vertices_[0]), 0);
  glEnableVertexAttribArray(0);
  glBindBuffer(GL_ARRAY_BUFFER,0);
  glBindVertexArray(0);

  shader_.Bind();
  shader_.SetTransform("projection", projection);
}

void PlayFieldRenderer::Render(const PlayField& pf) const
{
  RenderGrid();
  RenderStack(pf);
}

void PlayFieldRenderer::RenderGrid() const
{
  shader_.Bind();
  glBindVertexArray(vao_);
  shader_.Set3f("in_color", kPlayfieldFill_);
  glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

  shader_.Set3f("in_color", kPlayfieldLines_);
  glDrawArrays(GL_LINES, 4, 2 * (ncols_ + nrows_ - kHiddenRows_ + 2));
}

void PlayFieldRenderer::RenderStack(const PlayField& pf) const
{
  for (GLint row = 0; row < nrows_ - kHiddenRows_; ++row)
    {
      for (GLint col = 0; col < ncols_; ++col)
	{
	  TileColor color = pf.GetTileColor(row, col);
	  if (color != kEmpty)
	    {
	      texture_renderer_.Render(tile_textures_[color], x_ + col * tile_size_, y_ + row * tile_size_, tile_size_, tile_size_);
	    }
	}
    }
}

void PlayFieldRenderer::LineClearAnimation(const PlayField& playfield, GLfloat progress_percent) const {
    GLfloat t = 0.3;
    
    glm::vec3 mix_color;
    GLfloat mix_ratio;
    
    if (progress_percent < t)
      {
	mix_ratio = sin(M_PI * progress_percent / t);;
        mix_color = glm::vec3(1,1,1);
      }
    else
      {
	mix_ratio = (progress_percent - t) / (1 - t);
        mix_color = kPlayfieldFill_;
      }

    for (int row : playfield.LinesToClear())
      {
        for (int col = 0; col < ncols_; ++col)
	  {
            texture_renderer_.TransparentRender(tile_textures_.at(playfield.GetTileColor(row, col)),
						x_ + col * tile_size_,
						y_ + row * tile_size_,
						tile_size_,
						tile_size_,
						mix_ratio,
						mix_color);
	  }
      }
}

PlayFieldRenderer::~PlayFieldRenderer()
{
  glDeleteVertexArrays(1,&vao_);
}
