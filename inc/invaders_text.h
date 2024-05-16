#pragma once

#include "invaders_math.h"
#include "invaders_resources.h"
#include <unordered_map>
#include <string>

namespace Renderer {

  class Character final {
  public:
    unsigned int m_sizeX;
    unsigned int m_sizeY;
    int m_bearingX;
    int m_bearingY;
    unsigned int m_texId;
    unsigned int m_advance;
  };

  // only handles ASCII
  class TextRenderer final {
  public:
    TextRenderer(const char* fontPath,
                 const unsigned int fontSize,
                 Res::ResourceManager& resourceManager);
    ~TextRenderer();
    void renderText(const std::string& text,
                    const Math::v4& colour,
                    float x,
                    const float y,
                    const float scale);
    inline float getTextWidth(const std::string& text) const noexcept
    {
      auto sum = 0.0f;
      for(const auto& c : text) {
	sum += m_characters.at(c).m_sizeX;
      }
      return sum;
    }
  private:
    std::unordered_map<unsigned char, Character> m_characters;
    Res::ResourceManager& m_resourceManager;
    Res::Shader* m_textShader;
  };

};
