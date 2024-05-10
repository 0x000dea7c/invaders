#pragma once

#include "invaders_math.h"
#include <unordered_map>
#include <string>

namespace Renderer {

  class Character final {
  public:
    Math::v2 m_size;
    Math::v2 m_bearing;
    unsigned int m_texId;
    unsigned int m_advance;
  };

  class RenderTextArgs final {
  public:
    const std::string m_text;
    const Math::v4 m_colour;
    float m_x;
    const float m_y;
    const float m_scale;
    const unsigned int m_VBO;
    const unsigned int m_id;
  };

  class TextRenderer final {
  public:
    TextRenderer();
    ~TextRenderer();
    void renderText();
    std::unordered_map<unsigned char, Character> m_characters;
  };

};
