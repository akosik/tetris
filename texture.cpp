#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <assert.h>
#include <iostream>

#include "texture.h"

Texture::Texture(const std::string& filename, GLenum format)
{
  stbi_set_flip_vertically_on_load(true);  
  int height, width, ncolor_channels;
  unsigned char* image_data = stbi_load(filename.c_str(), &width, &height, &ncolor_channels, 4);

  if(image_data == NULL)
    std::cerr << "Error: Texture failed to load: " << filename << std::endl;
  else
    {
      glGenTextures(1, &id_);
      glBindTexture(GL_TEXTURE_2D, id_);

      // Set behavior for filling beyond the texture border
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      
      // Set filtering type for minifying/magnifying textures w/ mipmaps
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

      if(format == GL_RGBA)
	{
	  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	}
      else
	{
	  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
	}
      
      glGenerateMipmap(GL_TEXTURE_2D);
    }
  stbi_image_free(image_data);
}

Texture::~Texture()
{

}

void Texture::Bind(unsigned int unit) const
{
  assert(unit >= 0 && unit < 16);
  glActiveTexture(GL_TEXTURE0 + unit);
  glBindTexture(GL_TEXTURE_2D, id_);
}

Texture::Texture(const Texture& t)
{
  id_ = t.GetId();
}

void Texture::operator =(const Texture&) {
  
}
