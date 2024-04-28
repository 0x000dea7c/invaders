#include "invaders_text_renderer.h"
#include "invaders_math.h"
#include "invaders_types.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include "invaders_opengl.h"

#include <iostream>

namespace Game {

bool init_text_renderer(Text_Renderer &t, char const *fontpath,
                        const u32 fontsize) {
  FT_Library ft;

  if (FT_Init_FreeType(&ft)) {
    std::cerr << "Couldn't initialize FreeType.\n";
    return false;
  }

  FT_Face face;
  if (FT_New_Face(ft, fontpath, 0, &face)) {
    std::cerr << "Couldn't create new face from path " << fontpath << '\n';
    return false;
  }

  // NOTE when specifying 0, it lets compute the width dynamically based on the
  // height
  FT_Set_Pixel_Sizes(face, 0, fontsize);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  for (unsigned char c{0}; c < 128; ++c) {
    if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
      std::cerr << "Couldn't load glyph with index " << c << '\n';
      continue;
    }
    u32 id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED,
                 static_cast<i32>(face->glyph->bitmap.width),
                 static_cast<i32>(face->glyph->bitmap.rows), 0, GL_RED,
                 GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    Character ch{
        .size =
            v2{(f32)face->glyph->bitmap.width, (f32)face->glyph->bitmap.rows},
        .bearing =
            v2{(f32)face->glyph->bitmap_left, (f32)face->glyph->bitmap_top},
        .tex_id = id,
        .advance = static_cast<u32>(face->glyph->advance.x),
    };

    t.characters[c] = ch;
  }

  glBindTexture(GL_TEXTURE_2D, 0);
  FT_Done_Face(face);
  FT_Done_FreeType(ft);

  return true;
}

void render_text(Text_Renderer &t, const std::string &text, f32 x, const f32 y,
                 const f32 scale, const u32 VBO) {
  for (auto c = text.cbegin(); c != text.cend(); ++c) {
    const auto ch = t.characters.at(*c);

    const f32 w{static_cast<f32>(ch.size.x) * scale};
    const f32 h{static_cast<f32>(ch.size.y) * scale};

    f32 xPos = x + static_cast<f32>(ch.bearing.x) * scale;
    f32 yPos =
        y +
        (static_cast<f32>(t.characters['H'].bearing.y - ch.bearing.y)) * scale;

    const f32 vertices[] = {
        xPos,     yPos + h, 0.f, 0.f, // top left
        xPos + w, yPos,     1.f, 1.f, // bottom right
        xPos,     yPos,     0.f, 1.f, // bottom left

        xPos,     yPos + h, 0.f, 0.f, // top left
        xPos + w, yPos + h, 1.f, 0.f, // top right
        xPos + w, yPos,     1.f, 1.f, // bottom right
    };

    glBindTexture(GL_TEXTURE_2D, ch.tex_id);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // NOTE: bitshift by 6 (dividing over 1/2^6) to get value in pixels
    // why 6? because the advance value is given in a unit called "1/64th of a
    // pixel.
    x += (ch.advance >> 6) * scale;
  }
}
} // namespace Game
