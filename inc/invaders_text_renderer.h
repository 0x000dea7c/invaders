#pragma once

#include "invaders_types.h"
#include "invaders_math.h"

#include <unordered_map>
#include <string>

namespace Game {

  struct Character final {
    v2 size;
    v2 bearing;
    u32 tex_id;
    u32 advance;
  };

  struct Text_Renderer final {
    std::unordered_map<unsigned char, Character> characters;
  };

  bool init_text_renderer(Text_Renderer& t, char const* fontpath, const u32 fontsize);
  void render_text(Text_Renderer& t, const std::string& text, f32 x, const f32 y, const f32 scale, const u32 VBO, const u32 id, const v4 colour);

};
