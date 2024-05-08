#pragma once

#include "invaders_math.h"

#include <unordered_map>
#include <memory>

namespace Res {

  class Texture2D {
  public:
    Texture2D(const int w, const int h, const int id);
    ~Texture2D();
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

  class Shader {
  public:
    Shader(unsigned int id, unsigned int VAO, unsigned int VBO);
    ~Shader();
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
  class ResourceManager {
  public:
    ResourceManager();
    ~ResourceManager();
    void init();
    void close();
    Texture2D* getTex(const int sid);
    // shader utils -> TODO: uniforms should be on renderer?
    Shader* getShader(const int sid);
    void useShaderProgram(const unsigned int id);
    void setUniformFloat(const int id, const char* uniname, const float v);
    void setUniformMat4(const int id, const char* uniname, const Math::m4& m);
    void setUniformInt(const int id, const char* uniname, const int value);
  private:
    // if anything fails, game will close with diag msg
    unsigned int loadCompileShaders(const char* vertpath, const char* fragpath); // returns shader program id
    bool compile_errors(const unsigned int object, const ShaderType type);
    Shader loadBackgroundShader(const char* vertpath, const char* fragpath, const Math::m4& m);
    Shader loadAlienShader(const char* vertpath, const char* fragpath, const Math::m4& m);
    Shader loadPlayerLivesShader(const char* vertpath, const char* fragpath, const Math::m4& m);
    Shader loadPlayerShader(const char* vertpath, const char* fragpath, const Math::m4& m);
    Shader loadExplosionShader(const char* vertpath, const char* fragpath, const Math::m4& m);
    Shader loadMissileShader(const char* vertpath, const char* fragpath, const Math::m4& m);
    std::unordered_map<int, std::unique_ptr<Texture2D>> m_textures;
    std::unordered_map<int, std::unique_ptr<Shader>> m_shaders;
  };

};
