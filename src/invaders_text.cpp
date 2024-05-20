#include "invaders_text.h"
#include "invaders_opengl.h"
#include "common.h"

#include <GL/gl.h>
#include <ft2build.h>
#include FT_FREETYPE_H

#include <iostream>
#include <string>

namespace Renderer {
  using namespace Math;

  TextRenderer::TextRenderer(const char* fontPath,
                             const unsigned int fontSize,
                             Res::ResourceManager& resourceManager)
    : m_resourceManager{ resourceManager },
      m_textShader{ m_resourceManager.getShader(IDs::SID_SHADER_TEXT) }
  {
    FT_Library ft;
    if(FT_Init_FreeType(&ft)) {
      std::cerr << "Couldn't initialize FreeType.\n";
      std::exit(EXIT_FAILURE);
    }
    FT_Face face;
    if(FT_New_Face(ft, fontPath, 0, &face)) {
      std::cerr << "Couldn't create new face from path " << fontPath << '\n';
      std::exit(EXIT_FAILURE);
    }
    // NOTE when specifying 0, it lets compute the width dynamically based on the height
    FT_Set_Pixel_Sizes(face, 0, fontSize);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    for(unsigned char c{ 0 }; c < 128; ++c) {
      if(FT_Load_Char(face, c, FT_LOAD_RENDER)) {
        std::cerr << "Couldn't load glyph with index " << c << '\n';
        continue;
      }
      unsigned int id;
      glGenTextures(1, &id);
      glBindTexture(GL_TEXTURE_2D, id);
      glTexImage2D(GL_TEXTURE_2D,
                   0,
                   GL_RED,
                   static_cast<int>(face->glyph->bitmap.width),
                   static_cast<int>(face->glyph->bitmap.rows),
                   0,
                   GL_RED,
                   GL_UNSIGNED_BYTE,
                   face->glyph->bitmap.buffer);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      Character ch = {
        .m_sizeX    = face->glyph->bitmap.width,
        .m_sizeY    = face->glyph->bitmap.rows,
        .m_bearingX = face->glyph->bitmap_left,
        .m_bearingY = face->glyph->bitmap_top,
        .m_texId   = id,
        .m_advance = static_cast<unsigned int>(face->glyph->advance.x),
      };
      m_characters[c] = ch;
    }
    // @NOTE: adding size.x to the space bc freetype doesn't
    m_characters[' '].m_sizeX = m_characters['x'].m_sizeX;
    glBindTexture(GL_TEXTURE_2D, 0);
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
  }

  TextRenderer::~TextRenderer()
  {
    for(auto& c : m_characters) {
      glDeleteTextures(1, &c.second.m_texId);
    }
  }

  void TextRenderer::renderText(const std::string& text,
                                const Math::v4& colour,
                                float x,
                                const float y,
                                const float scale)
  {
    // NOTE: you're not activating the texture bc you did it before
    m_resourceManager.useShaderProgram(m_textShader->m_id);
    m_resourceManager.setUniformVec4(m_textShader->m_id, "textColour", colour);
    glBindVertexArray(m_textShader->m_VAO);
    for(auto c = text.cbegin(); c != text.cend(); ++c) {
      const Character& ch = m_characters.at(*c);
      const auto w  = static_cast<float>(ch.m_sizeX) * scale;
      const auto h  = static_cast<float>(ch.m_sizeY) * scale;
      auto xPos     = x + ch.m_bearingX * scale;
      auto yPos     = y - (m_characters['H'].m_bearingY - ch.m_bearingY) * scale;
      const float vertices[] = {
        xPos,     yPos - h, 0.0f, 1.0f,
        xPos + w, yPos - h, 1.0f, 1.0f,
        xPos,     yPos,     0.0f, 0.0f,

        xPos,     yPos,     0.0f, 0.0f,
        xPos + w, yPos,     1.0f, 0.0f,
        xPos + w, yPos - h, 1.0f, 1.0f
      };
      glBindTexture(GL_TEXTURE_2D, ch.m_texId);
      glBindBuffer(GL_ARRAY_BUFFER, m_textShader->m_VBO);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
      glBindBuffer(GL_ARRAY_BUFFER, 0);
      glDrawArrays(GL_TRIANGLES, 0, 6);
      // NOTE: bitshift by 6 (dividing over 1/2^6) to get value in pixels
      // why 6? because the advance value is given in a unit called "1/64th of a pixel.
      x += (ch.m_advance >> 6) * scale;
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
  }

};
