#include "invaders_resources.h"
#include "common.h"
#include "invaders.h"
#include "invaders_opengl.h"
#include "invaders_math.h"
// NOTE: HEADS UP! need to include renderer to use InstanceData classes, 99.9% it's wrong
#include "invaders_renderer.h"

// TODO: all of this should be in a config file of some sort...
// textures
#define TEX_FILE_PLAYER          "./res/textures/player.png"
#define TEX_FILE_ALIEN_ATLAS     "./res/textures/aliens.png"
#define TEX_FILE_MAIN_BACKGROUND "./res/textures/space.jpg"
#define TEX_FILE_MISSILE_PLAYER  "./res/textures/missile_player.png"
#define TEX_FILE_MISSILE_ALIEN   "./res/textures/missile_alien.png"
#define TEX_FILE_EXPLOSION       "./res/textures/explosion.png"
// shaders
#define VERT_SHADER_FILE_MAIN_BACKGROUND "./res/shaders/bg.vert"
#define FRAG_SHADER_FILE_MAIN_BACKGROUND "./res/shaders/bg.frag"
#define VERT_SHADER_FILE_ALIEN           "./res/shaders/aliens.vert"
#define FRAG_SHADER_FILE_ALIEN           "./res/shaders/aliens.frag"
#define VERT_SHADER_FILE_PLAYER_LIVES    "./res/shaders/playerLives.vert"
#define FRAG_SHADER_FILE_PLAYER_LIVES    "./res/shaders/playerLives.frag"
#define VERT_SHADER_FILE_PLAYER          "./res/shaders/shader.vert"
#define FRAG_SHADER_FILE_PLAYER          "./res/shaders/shader.frag"
#define VERT_SHADER_FILE_EXPLOSION       "./res/shaders/explosion.vert"
#define FRAG_SHADER_FILE_EXPLOSION       "./res/shaders/explosion.frag"
#define VERT_SHADER_FILE_MISSILE_PLAYER  "./res/shaders/missiles.vert"
#define FRAG_SHADER_FILE_MISSILE_PLAYER  "./res/shaders/missiles.frag"
#define VERT_SHADER_FILE_MISSILE_ALIEN   "./res/shaders/missiles.vert"
#define FRAG_SHADER_FILE_MISSILE_ALIEN   "./res/shaders/missiles.frag"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <array>
#include <string>

namespace Res {
  using namespace Game;
  using namespace Math;
  using namespace Renderer;

  Texture2D::Texture2D(const int w, const int h, const int id)
    : m_width(w), m_height(h), m_id(id)
  {

  }

  Texture2D::~Texture2D()
  {
    glDeleteTextures(1, &m_id);
  }

  Shader::Shader(unsigned int id, unsigned int VAO, unsigned int VBO)
    : m_id(id), m_VAO(VAO), m_VBO(VBO)
  {

  }

  Shader::~Shader()
  {
    glDeleteProgram(m_id);
  }

  ResourceManager::ResourceManager()
  {

  }

  ResourceManager::~ResourceManager()
  {

  }

  void ResourceManager::init()
  {
    // -----------------------------------------------
    // textures
    // -----------------------------------------------
    // this is all manual, perhaps not the best idea
    // player
    TexInfo t = loadTexFromFile(LoadTexFromFileArgs{
      .filepath = TEX_FILE_PLAYER,
      .alpha    = true,
      .flip     = false,
      .wrapS    = GL_CLAMP_TO_EDGE,
      .wrapT    = GL_CLAMP_TO_EDGE
    });
    IDs::SID_TEX_PLAYER = fnv1a(TEX_FILE_PLAYER);
    m_textures[IDs::SID_TEX_PLAYER] = std::make_unique<Texture2D>(t.width, t.height, t.id);
    // aliens (atlas)
    t = loadTexFromFile(LoadTexFromFileArgs{
      .filepath = TEX_FILE_ALIEN_ATLAS,
      .alpha    = true,
      .flip     = false,
      .wrapS    = GL_CLAMP_TO_EDGE,
      .wrapT    = GL_CLAMP_TO_EDGE
    });
    IDs::SID_TEX_ALIEN_ATLAS = fnv1a(TEX_FILE_ALIEN_ATLAS);
    m_textures[IDs::SID_TEX_ALIEN_ATLAS] = std::make_unique<Texture2D>(t.width, t.height, t.id);
    // background
    t = loadTexFromFile(LoadTexFromFileArgs{
      .filepath = TEX_FILE_MAIN_BACKGROUND,
      .alpha    = false,
      .flip     = false,
      .wrapS    = GL_REPEAT,
      .wrapT    = GL_REPEAT
    });
    IDs::SID_TEX_MAIN_BACKGROUND = fnv1a(TEX_FILE_MAIN_BACKGROUND);
    m_textures[IDs::SID_TEX_MAIN_BACKGROUND] = std::make_unique<Texture2D>(t.width, t.height, t.id);
    // missiles: player's and aliens'
    t = loadTexFromFile(LoadTexFromFileArgs{
      .filepath = TEX_FILE_MISSILE_PLAYER,
      .alpha    = true,
      .flip     = false,
      .wrapS    = GL_CLAMP_TO_EDGE,
      .wrapT    = GL_CLAMP_TO_EDGE
    });
    IDs::SID_TEX_MISSILE_PLAYER = fnv1a(TEX_FILE_MISSILE_PLAYER);
    m_textures[IDs::SID_TEX_MISSILE_PLAYER] = std::make_unique<Texture2D>(t.width, t.height, t.id);
    t = loadTexFromFile(LoadTexFromFileArgs{
      .filepath = TEX_FILE_MISSILE_ALIEN,
      .alpha    = true,
      .flip     = false,
      .wrapS    = GL_CLAMP_TO_EDGE,
      .wrapT    = GL_CLAMP_TO_EDGE
    });
    IDs::SID_TEX_MISSILE_ALIEN = fnv1a(TEX_FILE_MISSILE_ALIEN);
    m_textures[IDs::SID_TEX_MISSILE_ALIEN] = std::make_unique<Texture2D>(t.width, t.height, t.id);
    // explosion
    t = loadTexFromFile(LoadTexFromFileArgs{
      .filepath = TEX_FILE_EXPLOSION,
      .alpha    = true,
      .flip     = false,
      .wrapS    = GL_CLAMP_TO_EDGE,
      .wrapT    = GL_CLAMP_TO_EDGE
    });
    IDs::SID_TEX_EXPLOSION = fnv1a(TEX_FILE_EXPLOSION);
    m_textures[IDs::SID_TEX_EXPLOSION] = std::make_unique<Texture2D>(t.width, t.height, t.id);
    // -----------------------------------------------
    // shaders, since every shader is different, call different functions...
    // -----------------------------------------------
    // it doesn't matter if you use the vertex or fragment shader file name
    // for the id of the shader, what matters is if it's unique
    // define common proj matrix
    //
    const m4 m = orthographic({
      .left   =  0.f,
      .right  = WINDOW_WIDTH,
      .bottom =  0.f,
      .top    = WINDOW_HEIGHT,
      .zNear  = -1.f,
      .zFar   =  1.f
    });
    IDs::SID_SHADER_MAIN_BACKGROUND = fnv1a(VERT_SHADER_FILE_MAIN_BACKGROUND);
    m_shaders[IDs::SID_SHADER_MAIN_BACKGROUND] = std::make_unique<Shader>(
      loadBackgroundShader(VERT_SHADER_FILE_MAIN_BACKGROUND,
                           FRAG_SHADER_FILE_MAIN_BACKGROUND,
                           m)
    );
    IDs::SID_SHADER_ALIEN = fnv1a(VERT_SHADER_FILE_ALIEN);
    m_shaders[IDs::SID_SHADER_ALIEN] = std::make_unique<Shader>(
      loadAlienShader(VERT_SHADER_FILE_ALIEN,
                      FRAG_SHADER_FILE_ALIEN,
                      m)
    );
    IDs::SID_SHADER_PLAYER_LIVES = fnv1a(VERT_SHADER_FILE_PLAYER_LIVES);
    m_shaders[IDs::SID_SHADER_ALIEN] = std::make_unique<Shader>(
      loadPlayerLivesShader(VERT_SHADER_FILE_PLAYER_LIVES,
                            FRAG_SHADER_FILE_PLAYER_LIVES,
                            m)
    );
    IDs::SID_SHADER_PLAYER = fnv1a(VERT_SHADER_FILE_PLAYER);
    m_shaders[IDs::SID_SHADER_PLAYER] = std::make_unique<Shader>(
      loadPlayerShader(VERT_SHADER_FILE_PLAYER,
                       FRAG_SHADER_FILE_PLAYER,
                       m)
    );
    IDs::SID_SHADER_EXPLOSION = fnv1a(VERT_SHADER_FILE_EXPLOSION);
    m_shaders[IDs::SID_SHADER_EXPLOSION] = std::make_unique<Shader>(
      loadExplosionShader(VERT_SHADER_FILE_EXPLOSION,
                          FRAG_SHADER_FILE_EXPLOSION,
                          m)
    );
    IDs::SID_SHADER_MISSILE_PLAYER = fnv1a(VERT_SHADER_FILE_MISSILE_PLAYER);
    m_shaders[IDs::SID_SHADER_MISSILE_PLAYER] = std::make_unique<Shader>(
      loadMissileShader(VERT_SHADER_FILE_MISSILE_PLAYER,
                        FRAG_SHADER_FILE_MISSILE_PLAYER,
                        m)
    );
    IDs::SID_SHADER_MISSILE_ALIEN = fnv1a(VERT_SHADER_FILE_MISSILE_ALIEN);
    m_shaders[IDs::SID_SHADER_MISSILE_ALIEN] = std::make_unique<Shader>(
      loadMissileShader(VERT_SHADER_FILE_MISSILE_ALIEN,
                        FRAG_SHADER_FILE_MISSILE_ALIEN,
                        m)
    );
    // -----------------------------------------------
    // fonts
    // -----------------------------------------------

  }

  void ResourceManager::close()
  {
  }

  Texture2D* ResourceManager::getTex(const int sid)
  {
    return m_textures.at(sid).get();
  }

  Shader* ResourceManager::getShader(const int sid)
  {
    return m_shaders.at(sid).get();
  }

  void ResourceManager::useShaderProgram(const unsigned int id)
  {
    glUseProgram(id);
  }

  void ResourceManager::setUniformFloat(const int id, const char* uniname, const float v)
  {
    glUniform1f(glGetUniformLocation(id, uniname), v);
  }

  void ResourceManager::setUniformMat4(const int id, const char* uniname, const Math::m4& m)
  {
    glUniformMatrix4fv(glGetUniformLocation(id, uniname), 1, false, m.elements);
  }

  void ResourceManager::setUniformInt(const int id, const char* uniname, const int value)
  {
    glUniform1i(glGetUniformLocation(id, uniname), value);
  }

  bool compile_errors(const unsigned int object, const ShaderType type)
  {
    int success{ 0 };
    std::array<char, 512> log{ '\0' };
    std::string shader;
    switch(type) {
      case ShaderType::VERTEX:
        shader = "VERTEX";
        // fall through
      case ShaderType::FRAGMENT:
        shader = "FRAGMENT";
        // fall through
      case ShaderType::GEOMETRY:
        shader = "GEOMETRY";
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if(!success) {
          glGetShaderInfoLog(object, log.size(), nullptr, &log[0]);
          std::cerr << "Couldn't compile shader type " << shader << " : " << log.data() << '\n';
          return true;
        }
        break;
      case ShaderType::PROGRAM:
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if(!success) {
          glGetProgramInfoLog(object, log.size(), nullptr, &log[0]);
          std::cerr << "Couldn't link program: " << log.data() << '\n';
          return true;
        }
        break;
    }
    return false;
  }

  unsigned int ResourceManager::loadCompileShaders(const char* vertpath, const char* fragpath)
  {
    std::ifstream vertFileStream(vertpath);
    if(!vertFileStream) {
      std::cerr << __FUNCTION__ << " Couldn't open vertex file: " << vertpath << '\n';
      std::exit(EXIT_FAILURE);
    }
    std::ifstream fragFileStream(fragpath);
    if(!fragFileStream) {
      std::cerr << __FUNCTION__ << "Couldn't open fragment file: " << fragpath << '\n';
      std::exit(EXIT_FAILURE);
    }
    std::stringstream vertFileSS;
    vertFileSS << vertFileStream.rdbuf();
    std::stringstream fragFileSS;
    fragFileSS << fragFileStream.rdbuf();
    unsigned int vertId, fragId;
    const std::string vertCode = vertFileSS.str();
    const std::string fragCode = fragFileSS.str();
    const char* vertCodeC = vertCode.c_str();
    const char* fragCodeC = fragCode.c_str();
    vertId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertId, 1, &vertCodeC, nullptr);
    glCompileShader(vertId);
    bool errors = compile_errors(vertId, ShaderType::VERTEX);
    if(errors) {
      glDeleteShader(vertId);
      std::exit(EXIT_FAILURE);
    }
    fragId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragId, 1, &fragCodeC, nullptr);
    glCompileShader(fragId);
    errors = compile_errors(fragId, ShaderType::FRAGMENT);
    if(errors) {
      glDeleteShader(vertId);
      glDeleteShader(fragId);
      std::exit(EXIT_FAILURE);
    }
    unsigned int programId = glCreateProgram();
    glAttachShader(programId, vertId);
    glAttachShader(programId, fragId);
    glLinkProgram(programId);
    errors = compile_errors(programId, ShaderType::PROGRAM);
    if(errors) {
      glDeleteShader(vertId);
      glDeleteShader(fragId);
      std::exit(EXIT_FAILURE);
    }
    glDeleteShader(vertId);
    glDeleteShader(fragId);
    return programId;
  }

  Shader ResourceManager::loadBackgroundShader(const char* vertpath,
                                               const char* fragpath,
                                               const m4& m)
  {
    const unsigned int id = loadCompileShaders(vertpath, fragpath);
    if(!id) {
      std::cerr << __FUNCTION__ << " Error creating shader\n";
      exit(EXIT_FAILURE);
    }
    constexpr float vertices[] {
      // pos      -- tex
      -1.f, -1.f, 0.f, 0.f,  // bottom left
       1.f, -1.f, 1.f, 0.f,  // bottom right
      -1.f,  1.f, 0.f, 1.f,  // top left

      -1.f,  1.f, 0.f, 1.f,  // top left
       1.f,  1.f, 1.f, 1.f,  // top right
       1.f, -1.f, 1.f, 0.f,  // bottom right
    };
    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    useShaderProgram(id);
    setUniformInt(id, "image", 0);
    setUniformMat4(id, "projection", m);
    return Shader(id, VAO, VBO);
  }

  Shader ResourceManager::loadAlienShader(const char* vertpath, const char* fragpath, const Math::m4& m)
  {
    const unsigned int id = loadCompileShaders(vertpath, fragpath);
    if(!id) {
      std::cerr << __FUNCTION__ << " Error creating shader\n";
      exit(EXIT_FAILURE);
    }
    unsigned int VAO;
    unsigned int VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(InstanceData) * SIMUL_ALIENS_ALIVE, nullptr, GL_DYNAMIC_DRAW);
    // model matrix
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(0));
    glVertexAttribDivisor(0, 1);  // one value per instance
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(v4)));
    glVertexAttribDivisor(1, 1);  // one value per instance
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(v4) * 2));
    glVertexAttribDivisor(2, 1);  // one value per instance
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(v4) * 3));
    glVertexAttribDivisor(3, 1);  // one value per instance
    // texture coords
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4)));
    glVertexAttribDivisor(4, 1);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4) + sizeof(v4)));
    glVertexAttribDivisor(5, 1);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4) + sizeof(v4) * 2));
    glVertexAttribDivisor(6, 1);
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4) + sizeof(v4) * 3));
    glVertexAttribDivisor(7, 1);
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4) + sizeof(v4) * 4));
    glVertexAttribDivisor(8, 1);
    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4) + sizeof(v4) * 5));
    glVertexAttribDivisor(9, 1);
    useShaderProgram(id);
    setUniformInt(id, "image", 0);
    setUniformMat4(id, "projection", m);
    return Shader(id, VAO, VBO);
  }

  Shader ResourceManager::loadPlayerLivesShader(const char* vertpath, const char* fragpath, const Math::m4& m)
  {
    const unsigned int id = loadCompileShaders(vertpath, fragpath);
    if(!id) {
      std::cerr << __FUNCTION__ << " Error creating shader\n";
      exit(EXIT_FAILURE);
    }
    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(InstanceData) * 3, nullptr, GL_STATIC_DRAW);
    // model
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(0));
    glVertexAttribDivisor(0, 1);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(v4)));
    glVertexAttribDivisor(1, 1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(v4) * 2));
    glVertexAttribDivisor(2, 1);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(v4) * 3));
    glVertexAttribDivisor(3, 1);
    // quad pos and texture coords
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4)));
    glVertexAttribDivisor(4, 1);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4) + sizeof(v4)));
    glVertexAttribDivisor(5, 1);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4) + sizeof(v4) * 2));
    glVertexAttribDivisor(6, 1);
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4) + sizeof(v4) * 3));
    glVertexAttribDivisor(7, 1);
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4) + sizeof(v4) * 4));
    glVertexAttribDivisor(8, 1);
    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4) + sizeof(v4) * 5));
    glVertexAttribDivisor(9, 1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    useShaderProgram(id);
    setUniformInt(id, "image", 0);
    setUniformMat4(id, "projection", m);
    return Shader(id, VAO, VBO);
  }

  Shader ResourceManager::loadPlayerShader(const char* vertpath, const char* fragpath, const m4& m)
  {
    const auto id = loadCompileShaders(vertpath, fragpath);
    if(!id) {
      std::cerr << __FUNCTION__ << " Error creating shader\n";
      exit(EXIT_FAILURE);
    }
    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(InstanceData), nullptr, GL_STATIC_DRAW);
    // model
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(0));
    glVertexAttribDivisor(0, 1);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(v4)));
    glVertexAttribDivisor(1, 1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(v4) * 2));
    glVertexAttribDivisor(2, 1);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(v4) * 3));
    glVertexAttribDivisor(3, 1);
    // quad pos and texture coords
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4)));
    glVertexAttribDivisor(4, 1);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4) + sizeof(v4)));
    glVertexAttribDivisor(5, 1);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4) + sizeof(v4) * 2));
    glVertexAttribDivisor(6, 1);
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4) + sizeof(v4) * 3));
    glVertexAttribDivisor(7, 1);
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4) + sizeof(v4) * 4));
    glVertexAttribDivisor(8, 1);
    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4) + sizeof(v4) * 5));
    glVertexAttribDivisor(9, 1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    useShaderProgram(id);
    setUniformInt(id, "image", 0);
    setUniformMat4(id, "projection", m);
    return Shader(id, VAO, VBO);
  }

  Shader ResourceManager::loadExplosionShader(const char* vertpath, const char* fragpath, const m4& m)
  {
    const unsigned int id = loadCompileShaders(vertpath, fragpath);
    if(!id) {
      std::cerr << __FUNCTION__ << " Error creating shader\n";
      exit(EXIT_FAILURE);
    }
    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ExplosionInstanceData) * SIMUL_ALIENS_ALIVE, nullptr, GL_DYNAMIC_DRAW);
    // model matrix
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(ExplosionInstanceData), (void*)(0));
    glVertexAttribDivisor(0, 1);  // one value per instance
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(ExplosionInstanceData), (void*)(sizeof(v4)));
    glVertexAttribDivisor(1, 1);  // one value per instance
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(ExplosionInstanceData), (void*)(sizeof(v4) * 2));
    glVertexAttribDivisor(2, 1);  // one value per instance
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(ExplosionInstanceData), (void*)(sizeof(v4) * 3));
    glVertexAttribDivisor(3, 1);  // one value per instance
    // quad pos and texture coords
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(ExplosionInstanceData), (void*)(sizeof(m4)));
    glVertexAttribDivisor(4, 1);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(ExplosionInstanceData), (void*)(sizeof(m4) + sizeof(v4)));
    glVertexAttribDivisor(5, 1);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(ExplosionInstanceData), (void*)(sizeof(m4) + sizeof(v4) * 2));
    glVertexAttribDivisor(6, 1);
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(ExplosionInstanceData), (void*)(sizeof(m4) + sizeof(v4) * 3));
    glVertexAttribDivisor(7, 1);
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(ExplosionInstanceData), (void*)(sizeof(m4) + sizeof(v4) * 4));
    glVertexAttribDivisor(8, 1);
    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(ExplosionInstanceData), (void*)(sizeof(m4) + sizeof(v4) * 5));
    glVertexAttribDivisor(9, 1);
    // colour
    glEnableVertexAttribArray(10);
    glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(ExplosionInstanceData), (void*)(sizeof(m4) + sizeof(v4) * 6));
    glVertexAttribDivisor(10, 1);
    useShaderProgram(id);
    setUniformInt(id, "image", 0);
    setUniformMat4(id, "projection", m);
    return Shader(id, VAO, VBO);
  }

  Shader ResourceManager::loadMissileShader(const char* vertpath, const char* fragpath, const Math::m4& m)
  {
    const unsigned int id = loadCompileShaders(vertpath, fragpath);
    if(!id) {
      std::cerr << "Error creating missile shader\n";
      exit(EXIT_FAILURE);
    }
    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(InstanceData) * SIMUL_MISSILES_ALIVE, nullptr, GL_DYNAMIC_DRAW);
    // model matrix
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(0));
    glVertexAttribDivisor(0, 1);  // one value per instance
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(v4)));
    glVertexAttribDivisor(1, 1);  // one value per instance
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(v4) * 2));
    glVertexAttribDivisor(2, 1);  // one value per instance
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(v4) * 3));
    glVertexAttribDivisor(3, 1);  // one value per instance
    // texture coords
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4)));
    glVertexAttribDivisor(4, 1);
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4) + sizeof(v4)));
    glVertexAttribDivisor(5, 1);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4) + sizeof(v4) * 2));
    glVertexAttribDivisor(6, 1);
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4) + sizeof(v4) * 3));
    glVertexAttribDivisor(7, 1);
    glEnableVertexAttribArray(8);
    glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4) + sizeof(v4) * 4));
    glVertexAttribDivisor(8, 1);
    glEnableVertexAttribArray(9);
    glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), (void*)(sizeof(m4) + sizeof(v4) * 5));
    glVertexAttribDivisor(9, 1);
    useShaderProgram(id);
    setUniformInt(id, "image", 0);
    setUniformMat4(id, "projection", m);
    return Shader(id, VAO, VBO);
  }

};
