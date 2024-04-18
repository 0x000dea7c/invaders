#pragma once

// PLATFORM DEPENDENT function declarations
namespace Game {

    void init_OpenGL_fptrs();

    struct Texture_Info final {
        i32 width{ 0 };
        i32 height{ 0 };
        i32 channels{ 0 };
        u32 id{ 0 };
    };

    Texture_Info load_texture(char const* filepath, const bool alpha, const bool flip);
};
