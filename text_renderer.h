#ifndef TEXT_RENDERER_H
#define TEXT_RENDERER_H

#include "shader.h"

#include <string>
#include <map>
#include <glm/gtc/matrix_transform.hpp>
#include <ft2build.h>
#include FT_FREETYPE_H

struct Character {
    GLuint     texture_id;  // ID handle of the glyph texture
    glm::ivec2 size;       // Size of glyph
    glm::ivec2 bearing;    // Offset from baseline to left/top of glyph
    GLint     advance;    // Offset to advance to next glyph
};

class TextRenderer
{
public:
  TextRenderer(const std::string& font_filename, unsigned int fontsize, const glm::mat4& projection, const std::string& shaders_prefix);

  void Render(std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec4 rgba) const;
  void RenderCentered(std::string text, GLfloat x, GLfloat y, GLfloat width, GLfloat height, GLfloat scale, glm::vec4 rgba) const;
  
  GLint GetTextWidth(const std::string& text, GLfloat scale) const;
  GLint GetTextHeight(const std::string& text, GLfloat scale) const;
  
  ~TextRenderer();
private:
  Shader shader_;
  //glm::mat4 projection_;
  glm::mat4 projection_ = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
  GLuint vao_, vbo_;
  std::map<GLchar, Character> characters_;
};
#endif // TEXT_H
