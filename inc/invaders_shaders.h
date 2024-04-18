#pragma once

#include "invaders_opengl.h"
#include "invaders_types.h"
#include "invaders_math.h"

namespace Game {

    enum class ShaderType {
        VERTEX,
        FRAGMENT,
        GEOMETRY,
        PROGRAM,
    };

    u32 load_compile_shaders(const char* vert_file,
                             const char* frag_file);

    inline void set_uniform_int(const u32 id,
                                const char* name,
                                const i32 value)
    {
        glUniform1i(glGetUniformLocation(id, name), value);
    }

    inline void use_program(const u32 id)
    {
        glUseProgram(id);
    }

    inline void set_uniform_mat4(const i32 id,
                                 const char* name,
                                 const m4& m)
    {
        glUniformMatrix4fv(glGetUniformLocation(id, name),
                           1,
                           false,
                           m.elements);
    }

};
