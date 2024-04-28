#pragma once

#include "invaders_opengl.h"
#include "invaders_types.h"
#include "invaders_math.h"

namespace Game {

  // TODO: storing uniform locations in a map and then querying them (cache) might be a better option
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

  inline void set_uniform_float(const u32 id,
                                const char* name,
                                const f32 value)
  {
    glUniform1f(glGetUniformLocation(id, name), value);
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

  inline void set_uniform_vec4(const i32 id,
                               const char* name,
                               const v4& v)
  {
    glUniform4f(glGetUniformLocation(id, name), v.x, v.y, v.z, v.w);
  }

};
