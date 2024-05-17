#pragma once

#include "event.h"
#include "invaders.h"
#include "invaders_math.h"
#include "invaders_opengl.h"

#include <functional>
#include <unordered_map>
#include <memory>
#include <utility>
#include <string>

namespace Res {

  // NOTE: cursed code to be able to use the unordered_map with the pair
  class PairHash final {
  public:
    std::size_t operator()(const std::pair<unsigned int, std::string>& arg) const
    {
      // yoooo, these {} create an instance of the hash for the types in <>!
      const auto hash1 = std::hash<unsigned int>{}(arg.first);
      const auto hash2 = std::hash<std::string>{}(arg.second);
      return hash1 ^ (hash2 << 1);
    }
  };

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
  // - audio
  // etc
  // this class is omoii
  class ResourceManager final {
  public:
    ResourceManager(Ev::EventManager& eventManager);
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
    inline unsigned int getUniformLocation(const unsigned int id, const char* uniname)
    {
      const auto key = std::make_pair(id, uniname);
      const auto it  = m_uniforms.find(key);
      if(it == m_uniforms.end()) {
        const auto location = glGetUniformLocation(id, uniname);
        m_uniforms[key] = location;
        return location;
      }
      return it->second;
    }
    // NOTE: you can cache uniform locations to avoid a context switch
    inline auto setUniformFloat(const unsigned int id, const char* uniname, const float v)
    {
      glUniform1f(getUniformLocation(id, uniname), v);
    }
    inline auto setUniformMat4(const unsigned int id, const char* uniname, const Math::m4& m)
    {
      glUniformMatrix4fv(getUniformLocation(id, uniname), 1, false, m.elements);
    }
    inline auto setUniformInt(const unsigned int id, const char* uniname, const int value)
    {
      glUniform1i(getUniformLocation(id, uniname), value);
    }
    inline auto setUniformVec4(const unsigned int id, const char* uniname, const Math::v4& value)
    {
      glUniform4f(getUniformLocation(id, uniname), value.x, value.y, value.z, value.w);
    }
    void initAudio(Game::AudioDevice device);
    void playAudioTrack(const int sid, const bool loop) const noexcept;
    void stopAudioTrack(const int sid, const unsigned int delay) const noexcept;
    void increaseVolume() const noexcept;
    void decreaseVolume() const noexcept;
    float getVolume() const noexcept;
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
    Shader loadTextShader(const char* vertpath, const char* fragpath, const Math::m4& m);
    Shader loadMenuShader(const char* vertpath, const char* fragpath);
    Shader loadBasicShader(const char* vertpath, const char* fragpath);
    Ev::EventManager& m_eventManager;
    std::unordered_map<int, std::unique_ptr<Texture2D>> m_textures;
    std::unordered_map<int, std::unique_ptr<Shader>> m_shaders;
    std::unordered_map<int, std::unique_ptr<Game::AudioData>> m_audioTracks;
    std::unordered_map<std::pair<unsigned int, std::string>, unsigned int, PairHash> m_uniforms;
  };

};
