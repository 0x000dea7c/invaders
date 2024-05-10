#pragma once

#include "invaders_math.h"
#include "invaders_opengl.h"
#include <unordered_map>
#include <memory>

namespace Res {

  class Texture2D final {
  public:
    Texture2D(const int w, const int h, const unsigned int id);
    int m_width;
    int m_height;
    unsigned int m_id;
  };

  enum class ShaderType {
    VERTEX,
    FRAGMENT,
    GEOMETRY,
    PROGRAM,
  };

  class Shader final {
  public:
    Shader(unsigned int id, unsigned int VAO, unsigned int VBO);
    unsigned int m_id;
    unsigned int m_VAO;
    unsigned int m_VBO;
  };

  // handles every game resource:
  // - textures
  // - fonts
  // - shaders
  // - etc
  // this class is also omoiiiiiiiiiiiiiiiii
  class ResourceManager final {
  public:
    ResourceManager();
    ~ResourceManager();
    inline auto getTex(const int sid) const noexcept
    {
      return m_textures.at(sid).get();
    }
    inline auto getShader(const int sid) const noexcept
    {
      return m_shaders.at(sid).get();
    }
    inline auto useShaderProgram(const unsigned int id) const noexcept
    {
      glUseProgram(id);
    }
    inline auto setUniformFloat(const unsigned int id, const char* uniname, const float v) const noexcept
    {
      glUniform1f(glGetUniformLocation(id, uniname), v);
    }
    inline auto setUniformMat4(const unsigned int id, const char* uniname, const Math::m4& m) const noexcept
    {
      glUniformMatrix4fv(glGetUniformLocation(id, uniname), 1, false, m.elements);
    }
    inline auto setUniformInt(const unsigned int id, const char* uniname, const int value) const noexcept
    {
      glUniform1i(glGetUniformLocation(id, uniname), value);
    }
  private:
    // if anything fails, game will close with diag msg
    unsigned int loadCompileShaders(const char* vertpath, const char* fragpath); // returns shader program id
    bool checkCompileErrors(const unsigned int object, const ShaderType type);
    Shader loadBackgroundShader(const char* vertpath, const char* fragpath);
    Shader loadAlienShader(const char* vertpath, const char* fragpath, const Math::m4& m);
    Shader loadPlayerLivesShader(const char* vertpath, const char* fragpath, const Math::m4& m);
    Shader loadPlayerShader(const char* vertpath, const char* fragpath, const Math::m4& m);
    Shader loadExplosionShader(const char* vertpath, const char* fragpath, const Math::m4& m);
    Shader loadMissileShader(const char* vertpath, const char* fragpath, const Math::m4& m);
    std::unordered_map<int, std::unique_ptr<Texture2D>> m_textures;
    std::unordered_map<int, std::unique_ptr<Shader>> m_shaders;
  };

};
