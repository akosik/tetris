#include "hud_renderer.h"

const glm::vec3 HudRenderer::tetro_colors_[] = { glm::vec3(0,255,255), glm::vec3(0,0,255), glm::vec3(255,128,0), glm::vec3(255,255,0), glm::vec3(0,255,0), glm::vec3(255,0,213), glm::vec3(255,0,0) };


HudRenderer::HudRenderer(GLfloat x,
			 GLfloat y,
			 GLfloat width,
			 GLfloat height,
			 GLint hidden_rows,
			 GLint nrows,
			 GLint ncols,
			 GLfloat tile_size,
			 const TetrominoRenderer& tetromino_renderer,
			 const TextureRenderer& tetro_texture_renderer,
			 const TextRenderer& text_renderer)
  : hudh_(height),
    hudw_(width),
    hudy_(y),
    hudx_(x),
    hidden_rows_(hidden_rows),
    nrows_(nrows),
    ncols_(ncols),
    tile_size_(tile_size),
    tetromino_renderer_(tetromino_renderer),
    tetro_texture_renderer_(tetro_texture_renderer),
    text_renderer_(text_renderer)
{ }

void HudRenderer::RenderBackground(const PlayField& playfield, const Game& tetris)
{
  // int cc[] = { 0,0,0,0,0,0,0 };
  // for (int row = 0; row < nrows_ - hidden_rows_; ++row)
  //   {
  //     for (int col = 0; col < ncols_; ++col)
  // 	{
  // 	  ++cc[playfield.GetTileColor(row, col)];
  // 	}
  //   }

  // int index = 0;
  // for (int i = 0; i < 7; ++i)
  //   {
  //     if (cc[i] > cc[index])
  // 	index = i;
  //   }

  // float o = 1/255.f;
  // for (int i = 0; i < 7; ++i)
  //   {
  //     glm::vec3 hsv = RgbToHsv(o*c[i]);
  //     hsv.y = .5;
  //     hsv.z = 1;
  //     //std::cout << hsv.x << std::endl;
  //     hsv.x = int(hsv.x + 180) % 360;
  //     c[i] = HsvToRgb(hsv);
  //   }
  // if (color_b_ != c[i])
  //   {
  //     color_a_ = color_b_;
  //     color_b_ = c[i];
  //     last_frame_ = glfwGetTime();
  //   }
  glm::vec3 rgb1 = tetro_colors_[static_cast<int>(tetris.Next())];
  glm::vec3 rgb2 = tetro_colors_[static_cast<int>(playfield.FallingTetro().Type())];

  glm::vec3 hsv1 = RgbToHsv(rgb1);
  glm::vec3 hsv2 = RgbToHsv(rgb2);

  hsv1.x = int(hsv1.x + 180) % 360;
  hsv2.x = int(hsv2.x + 180) % 360;
  hsv2.y = .5;
  hsv2.z = .5;

  glm::vec3 bcolor = MixHsv(hsv2, hsv1, (19 - playfield.FallingTetroRow()) / 19.f);
  
  // glm::vec3 bcolor = MixRgb(hsv1, hsv2, (19 - playfield.FallingTetroRow()) / 19.f);
  bcolor = HsvToRgb(bcolor);
  
  
  glClearColor(90/255.f, 91/255.f, 94/255.f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);
}

void HudRenderer::RenderHud(TetroType next, TetroType held, GLint score, GLint lines, GLint level)
{
  GLint y = hudy_ + 16 * tile_size_;
  glm::vec3 hsv = RgbToHsv(tetro_colors_[next]);
  hsv.x = int(hsv.x + 180) % 360;
  hsv.y = .2;
  hsv.z = .7;
  glm::vec3 next_background_color = HsvToRgb(hsv);

  tetro_texture_renderer_.ColorRender(next_background_color, 0, y, hudw_*4, tile_size_*5);
  y += 3 * tile_size_;
  tetromino_renderer_.RenderCentered(hudx_, y, hudw_, hudh_, next);
  y -= 2 * tile_size_;
  text_renderer_.RenderCentered("Next", hudx_, y, hudw_, hudh_, .4, glm::vec4(0,0,0,1));
  y -= 6 * tile_size_;

  if (held != kNone)
    {
      hsv = RgbToHsv(tetro_colors_[held]);
      hsv.x = int(hsv.x + 180) % 360;
      hsv.y = .2;
      hsv.z = .7;
      next_background_color = HsvToRgb(hsv);

      tetro_texture_renderer_.ColorRender(next_background_color, 0, y, hudw_*4, tile_size_*5);
    }
  y += 3 * tile_size_;
  tetromino_renderer_.RenderCentered(hudx_, y, hudw_, hudh_, held);
  y -= 2 * tile_size_;
  text_renderer_.RenderCentered("Hold", hudx_, y, hudw_, hudh_, .4, glm::vec4(0,0,0,1));
  
  y -= 2 * tile_size_;
  text_renderer_.RenderCentered("Score", hudx_, y, hudw_, hudh_, .4, glm::vec4(0,0,0,1));
  y -= tile_size_;
  text_renderer_.RenderCentered(std::to_string(score), hudx_, y, hudw_, hudh_, .3, glm::vec4(0,0,0,1));
  
  y -= 2 * tile_size_;
  text_renderer_.RenderCentered("Lines", hudx_, y, hudw_, hudh_, .4, glm::vec4(0,0,0,1));
  y -= tile_size_;
  text_renderer_.RenderCentered(std::to_string(lines), hudx_, y, hudw_, hudh_, .3, glm::vec4(0,0,0,1));
  
  y -= 2 * tile_size_;
  text_renderer_.RenderCentered("Level", hudx_, y, hudw_, hudh_, .4, glm::vec4(0,0,0,1));
  y -= tile_size_;
  text_renderer_.RenderCentered(std::to_string(level), hudx_, y, hudw_, hudh_, .3, glm::vec4(0,0,0,1));
}

HudRenderer::~HudRenderer()
{
  
}

glm::vec3 HudRenderer::MixHsv(glm::vec3 hsv1, glm::vec3 hsv2, GLfloat mix_coefficient)
{
  glm::vec3 out(0,hsv1.y,hsv1.z);
  if (hsv1.x < hsv2.x)
    {
      out.x = hsv1.x + (hsv2.x - hsv1.x) * mix_coefficient;
    }
  else if (hsv1.x > hsv2.x)
    {
      out.x = hsv1.x - (hsv1.x - hsv2.x) * mix_coefficient;
    }
  else
    {
      out.x = hsv1.x;
    }
  return out;
}

glm::vec3 HudRenderer::MixRgb(glm::vec3 rgb1, glm::vec3 rgb2, GLfloat mix_coefficient)
{
  glm::vec3 mixed(0);
  mixed.r = rgb1.r + (rgb2.r - rgb1.r) * mix_coefficient;
  mixed.g = rgb1.g + (rgb2.g - rgb1.g) * mix_coefficient;
  mixed.b = rgb1.b + (rgb2.b - rgb1.b) * mix_coefficient;
  return mixed;
}

// conversion functions courtesy of stack exchange
glm::vec3 HudRenderer::RgbToHsv(glm::vec3 rgb)
{
  glm::vec3         out;
  float      min, max, delta;

    min = rgb.r < rgb.g ? rgb.r : rgb.g;
    min = min  < rgb.b ? min  : rgb.b;

    max = rgb.r > rgb.g ? rgb.r : rgb.g;
    max = max  > rgb.b ? max  : rgb.b;

    out.z = max;                                // v
    delta = max - min;
    if (delta < 0.00001)
    {
        out.y = 0;
        out.x = 0; // undefined, maybe nan?
        return out;
    }
    if( max > 0.0 ) { // NOTE: if Max is == 0, this divide would cause a crash
        out.y = (delta / max);                  // s
    } else {
        // if max is 0, then r = g = b = 0              
        // s = 0, h is undefined
        out.y = 0.0;
        out.x = NAN;                            // its now undefined
        return out;
    }
    if( rgb.r >= max )                           // > is bogus, just keeps compilor happy
        out.x = ( rgb.g - rgb.b ) / delta;        // between yellow & magenta
    else
    if( rgb.g >= max )
        out.x = 2.0 + ( rgb.b - rgb.r ) / delta;  // between cyan & yellow
    else
        out.x = 4.0 + ( rgb.r - rgb.g ) / delta;  // between magenta & cyan

    out.x *= 60.0;                              // degrees

    if( out.x < 0.0 )
        out.x += 360.0;

    return out;
}

glm::vec3 HudRenderer::HsvToRgb(glm::vec3 hsv)
{
  float     hh, p, q, t, ff;
  long        i;
  glm::vec3       out;
    
    if(hsv.y <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = hsv.z;
        out.g = hsv.z;
        out.b = hsv.z;
        return out;
    }
    hh = hsv.x;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = hsv.z * (1.0 - hsv.y);
    q = hsv.z * (1.0 - (hsv.y * ff));
    t = hsv.z * (1.0 - (hsv.y * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = hsv.z;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = hsv.z;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = hsv.z;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = hsv.z;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = hsv.z;
        break;
    case 5:
    default:
        out.r = hsv.z;
        out.g = p;
        out.b = q;
        break;
    }
    return out;     
}
