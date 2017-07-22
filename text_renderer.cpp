#include <iostream>
#include <glm/ext.hpp>

#include "text_renderer.h"

TextRenderer::TextRenderer(const std::string& font_filename, unsigned int fontsize, const glm::mat4& projection, const std::string& shaders_prefix) : projection_(projection), shader_(shaders_prefix)
{
  FT_Library ft;
  if (FT_Init_FreeType(&ft))
    std::cout << "Error: Could not initialize FreeType Library" << std::endl;

  FT_Face face;
  if (FT_New_Face(ft, font_filename.c_str(), 0, &face))
    std::cout << "Error: Failed to load font" << std::endl;

  FT_Set_Pixel_Sizes(face, 0, fontsize);  

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  
  for (GLubyte c = 0; c < 128; c++)
    {
      if (FT_Load_Char(face, c, FT_LOAD_RENDER))
	{
	  std::cout << "Error: Failed to load Glyph" << std::endl;
	  continue;
	}

      GLuint texture;
      glGenTextures(1, &texture);
      glBindTexture(GL_TEXTURE_2D, texture);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
    
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      Character character = {
        texture, 
        glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
        glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
        static_cast<GLint>(face->glyph->advance.x)
      };
    
      characters_.insert(std::pair<GLchar, Character>(c, character));
    }

  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  glGenVertexArrays(1, &vao_);
  glGenBuffers(1, &vbo_);
  glBindVertexArray(vao_);
  glBindBuffer(GL_ARRAY_BUFFER, vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  shader_.Bind();
  shader_.SetTransform("projection", projection_);
}

void TextRenderer::Render(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec4 rgba) const
{
  shader_.Bind();
  glUniform4f(glGetUniformLocation(shader_.GetId(), "text_rgba"), rgba.r, rgba.g, rgba.b, rgba.a );
  glActiveTexture(GL_TEXTURE0);
  glBindVertexArray(vao_);

  // Iterate through all characters
  for (auto c = text.begin(); c != text.end(); ++c)
    {
      Character ch = characters_.at(*c);

      GLfloat xpos = x + ch.bearing.x * scale;
      GLfloat ypos = y - (ch.size.y - ch.bearing.y) * scale;

      GLfloat w = ch.size.x * scale;
      GLfloat h = ch.size.y * scale;
      // Update vbo_ for each character
      GLfloat vertices[6][4] = {
	{ xpos,     ypos + h,   0.0, 0.0 },            
	{ xpos,     ypos,       0.0, 1.0 },
	{ xpos + w, ypos,       1.0, 1.0 },

	{ xpos,     ypos + h,   0.0, 0.0 },
	{ xpos + w, ypos,       1.0, 1.0 },
	{ xpos + w, ypos + h,   1.0, 0.0 }           
      };
      // Render glyph texture over quad
      glBindTexture(GL_TEXTURE_2D, ch.texture_id);
      // Update content of vbo_ memory
      glBindBuffer(GL_ARRAY_BUFFER, vbo_);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); 
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      // Render quad
      glDrawArrays(GL_TRIANGLES, 0, 6);
      // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
      x += (ch.advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
    }
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::RenderCentered(std::string text, GLfloat x, GLfloat y, GLfloat width, GLfloat height, GLfloat scale, glm::vec4 rgba) const
{
  GLuint w = GetTextWidth(text, scale);
  x += (width - w) / 2;
  Render(text, x, y, scale, rgba);
}

GLint TextRenderer::GetTextWidth(const std::string& text, GLfloat scale) const
{
  GLint w = 0;
  for (auto c = text.begin(); c != text.end(); ++c)
    {
      w += (characters_.at(*c).advance >> 6) * scale;
    }
  return w;
}

GLint TextRenderer::GetTextHeight(const std::string& text, GLfloat scale) const
{
  GLint h = 0;
  for (auto c = text.begin(); c != text.end(); ++c)
    {
      std::max(h, characters_.at(*c).size.y);
    }
  
  return h * scale;
}

TextRenderer::~TextRenderer()
{

}
