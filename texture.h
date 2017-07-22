#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <GL/glew.h>

/*
  A class for loading an image from file,
  setting relevant texture parameters,
  sending the image data to the gpu,
  and binding the texture for rendering
*/

class Texture
{
public:
  Texture(const std::string& filename, GLenum format);
  Texture(const Texture& other);
  virtual ~Texture();

  void Bind(unsigned int unit) const;

  GLuint GetId() const { return id_; }
  
private:
  void operator=(const Texture&);

  GLuint id_;
};


#endif //TEXTURE_H
