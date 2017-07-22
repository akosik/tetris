#ifndef HUD_RENDERER_H
#define HUD_RENDERER_H

#include "tetromino.h"
#include "text_renderer.h"
#include "texture_renderer.h"
#include "tetromino_renderer.h"
#include "playfield.h"
#include "game.h"

#include <vector>

class HudRenderer
{
public:
  HudRenderer(GLfloat x,
	      GLfloat y,
	      GLfloat width,
	      GLfloat height,
	      GLint hidden_rows,
	      GLint nrows,
	      GLint ncols,
	      GLfloat tile_size,
	      const TetrominoRenderer& tetromino_renderer,
	      const TextureRenderer& tetro_texture_renderer,
	      const TextRenderer& text_renderer);
  
  void RenderBackground(const PlayField& playfield, const Game& tetris);
  void RenderHud(TetroType next, TetroType hold, GLint score, GLint lines, GLint level);
  
  virtual ~HudRenderer();
private:
  glm::vec3 RgbToHsv(glm::vec3 rgb);
  glm::vec3 HsvToRgb(glm::vec3 hsv);
  glm::vec3 MixHsv(glm::vec3 hsv1, glm::vec3 hsv2, GLfloat mix_coefficient);
  glm::vec3 MixRgb(glm::vec3 rgb1, glm::vec3 rgb2, GLfloat mix_coefficient);

  const GLint nrows_, ncols_;
  const GLint hidden_rows_;
  const GLfloat hudx_, hudy_;
  const GLfloat hudw_, hudh_;
  const GLfloat tile_size_;
  const TetrominoRenderer& tetromino_renderer_;
  const TextureRenderer& tetro_texture_renderer_;
  const TextRenderer& text_renderer_;

  static const glm::vec3 tetro_colors_[];
};


#endif // HUD_RENDERER_H
