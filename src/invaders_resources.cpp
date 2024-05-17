#include "invaders_resources.h"
#include "common.h"
#include "invaders.h"
#include "invaders_math.h"

// NOTE: HEADS UP! need to include renderer to use InstanceData classes, 99.9% it's wrong
#include "invaders_renderer.h"

// textures
#define TEX_FILE_PLAYER          "./res/textures/player.png"
#define TEX_FILE_ALIEN_ATLAS     "./res/textures/aliens.png"
#define TEX_FILE_MAIN_BACKGROUND "./res/textures/space.jpg"
#define TEX_FILE_MISSILE_PLAYER  "./res/textures/missile_player.png"
#define TEX_FILE_MISSILE_ALIEN   "./res/textures/missile_alien.png"
#define TEX_FILE_EXPLOSION       "./res/textures/explosion.png"
#define TEX_FILE_PLAYER_LIVES    "./res/textures/heart.png"
#define TEX_FILE_INVADERS        "./res/textures/invaders.png"
// shaders
#define VERT_SHADER_FILE_MAIN_BACKGROUND "./res/shaders/bg.vert"
#define FRAG_SHADER_FILE_MAIN_BACKGROUND "./res/shaders/bg.frag"
#define VERT_SHADER_FILE_ALIEN           "./res/shaders/aliens.vert"
#define FRAG_SHADER_FILE_ALIEN           "./res/shaders/aliens.frag"
#define VERT_SHADER_FILE_PLAYER_LIVES    "./res/shaders/playerLives.vert"
#define FRAG_SHADER_FILE_PLAYER_LIVES    "./res/shaders/playerLives.frag"
#define VERT_SHADER_FILE_PLAYER          "./res/shaders/player.vert"
#define FRAG_SHADER_FILE_PLAYER          "./res/shaders/player.frag"
#define VERT_SHADER_FILE_EXPLOSION       "./res/shaders/explosion.vert"
#define FRAG_SHADER_FILE_EXPLOSION       "./res/shaders/explosion.frag"
#define VERT_SHADER_FILE_MISSILE_PLAYER  "./res/shaders/missiles.vert"
#define FRAG_SHADER_FILE_MISSILE_PLAYER  "./res/shaders/missiles.frag"
#define VERT_SHADER_FILE_MISSILE_ALIEN   "./res/shaders/missiles.vert"
#define FRAG_SHADER_FILE_MISSILE_ALIEN   "./res/shaders/missiles.frag"
#define VERT_SHADER_FILE_TEXT            "./res/shaders/text.vert"
#define FRAG_SHADER_FILE_TEXT            "./res/shaders/text.frag"
#define VERT_SHADER_FILE_MENU            "./res/shaders/menu.vert"
#define FRAG_SHADER_FILE_MENU            "./res/shaders/menu.frag"
#define VERT_SHADER_FILE_BASIC           "./res/shaders/basic.vert"
#define FRAG_SHADER_FILE_BASIC           "./res/shaders/basic.frag"
// Audio
#define AUDIO_FILE_BG_MUSIC   "./res/audio/music_zapsplat_game_music_action_retro_8_bit_repeating_016.ogg"
#define AUDIO_FILE_EXPLOSION  "./res/audio/mixkit-war-explosions-2773.ogg"
#define AUDIO_FILE_LOSE_GAME  "./res/audio/RetroNegativeMelody02.ogg"
#define AUDIO_FILE_WIN_GAME   "./res/audio/mixkit-game-level-completed-2059.ogg"
#define AUDIO_FILE_PLAYER_DIE "./res/audio/RetronegativeShort23.ogg"
#define AUDIO_FILE_WIN_LEVEL  "./res/audio/RetroBlopStereoUP04.ogg"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <array>
#include <string>

namespace IDs {
  // texture ids
  int SID_TEX_PLAYER;
  int SID_TEX_ALIEN_ATLAS;
  int SID_TEX_MAIN_BACKGROUND;
  int SID_TEX_MISSILE_PLAYER;
  int SID_TEX_MISSILE_ALIEN;
  int SID_TEX_EXPLOSION;
  int SID_TEX_PLAYER_LIVES;
  int SID_TEX_INVADERS;
  // shader ids
  int SID_SHADER_MAIN_BACKGROUND;
  int SID_SHADER_ALIEN;
  int SID_SHADER_PLAYER_LIVES;
  int SID_SHADER_PLAYER;
  int SID_SHADER_EXPLOSION;
  int SID_SHADER_MISSILE_PLAYER;
  int SID_SHADER_MISSILE_ALIEN;
  int SID_SHADER_TEXT;
  int SID_SHADER_MENU;
  int SID_SHADER_BASIC;
  // audio
  int SID_AUDIO_BG_MUSIC;
  int SID_AUDIO_EXPLOSION;
  int SID_AUDIO_LOSE_GAME;
  int SID_AUDIO_WIN_GAME;
  int SID_AUDIO_PLAYER_DIE;
  int SID_AUDIO_WIN_LEVEL;
};

namespace Res {
  using namespace Game;
  using namespace Math;
  using namespace Renderer;
  using namespace Ev;

  Texture2D::Texture2D(const int w, const int h, const unsigned int id)
    : m_width{ w }, m_height{ h }, m_id{ id }
  {

  }

  Shader::Shader(unsigned int id, unsigned int VAO, unsigned int VBO)
    : m_id{ id }, m_VAO{ VAO }, m_VBO{ VBO }
  {}

  ResourceManager::ResourceManager(EventManager& eventManager)
    : m_eventManager{ eventManager }
  {
    // -----------------------------------------------
    // textures
    // -----------------------------------------------
    // this is all manual, not the best idea
    // player
    auto t = loadTexFromFile(LoadTexFromFileArgs{
      .m_filepath = TEX_FILE_PLAYER,
      .m_alpha    = true,
      .m_flip     = false,
      .m_wrapS    = GL_CLAMP_TO_EDGE,
      .m_wrapT    = GL_CLAMP_TO_EDGE
    });
    IDs::SID_TEX_PLAYER = fnv1a(TEX_FILE_PLAYER);
    m_textures[IDs::SID_TEX_PLAYER] = std::make_unique<Texture2D>(t.m_width, t.m_height, t.m_id);
    // aliens (atlas)
    t = loadTexFromFile(LoadTexFromFileArgs{
      .m_filepath = TEX_FILE_ALIEN_ATLAS,
      .m_alpha    = true,
      .m_flip     = false,
      .m_wrapS    = GL_CLAMP_TO_EDGE,
      .m_wrapT    = GL_CLAMP_TO_EDGE
    });
    IDs::SID_TEX_ALIEN_ATLAS = fnv1a(TEX_FILE_ALIEN_ATLAS);
    m_textures[IDs::SID_TEX_ALIEN_ATLAS] = std::make_unique<Texture2D>(t.m_width, t.m_height, t.m_id);
    // background
    t = loadTexFromFile(LoadTexFromFileArgs{
      .m_filepath = TEX_FILE_MAIN_BACKGROUND,
      .m_alpha    = false,
      .m_flip     = false,
      .m_wrapS    = GL_REPEAT,
      .m_wrapT    = GL_REPEAT
    });
    IDs::SID_TEX_MAIN_BACKGROUND = fnv1a(TEX_FILE_MAIN_BACKGROUND);
    m_textures[IDs::SID_TEX_MAIN_BACKGROUND] = std::make_unique<Texture2D>(t.m_width, t.m_height, t.m_id);
    // missiles: player's and aliens'
    t = loadTexFromFile(LoadTexFromFileArgs{
      .m_filepath = TEX_FILE_MISSILE_PLAYER,
      .m_alpha    = true,
      .m_flip     = true,
      .m_wrapS    = GL_CLAMP_TO_EDGE,
      .m_wrapT    = GL_CLAMP_TO_EDGE
    });
    IDs::SID_TEX_MISSILE_PLAYER = fnv1a(TEX_FILE_MISSILE_PLAYER);
    m_textures[IDs::SID_TEX_MISSILE_PLAYER] = std::make_unique<Texture2D>(t.m_width, t.m_height, t.m_id);
    t = loadTexFromFile(LoadTexFromFileArgs{
      .m_filepath = TEX_FILE_MISSILE_ALIEN,
      .m_alpha    = true,
      .m_flip     = false,
      .m_wrapS    = GL_CLAMP_TO_EDGE,
      .m_wrapT    = GL_CLAMP_TO_EDGE
    });
    IDs::SID_TEX_MISSILE_ALIEN = fnv1a(TEX_FILE_MISSILE_ALIEN);
    m_textures[IDs::SID_TEX_MISSILE_ALIEN] = std::make_unique<Texture2D>(t.m_width, t.m_height, t.m_id);
    // explosion
    t = loadTexFromFile(LoadTexFromFileArgs{
      .m_filepath = TEX_FILE_EXPLOSION,
      .m_alpha    = true,
      .m_flip     = false,
      .m_wrapS    = GL_CLAMP_TO_EDGE,
      .m_wrapT    = GL_CLAMP_TO_EDGE
    });
    IDs::SID_TEX_EXPLOSION = fnv1a(TEX_FILE_EXPLOSION);
    m_textures[IDs::SID_TEX_EXPLOSION] = std::make_unique<Texture2D>(t.m_width, t.m_height, t.m_id);
    // player lives
    t = loadTexFromFile(LoadTexFromFileArgs{
      .m_filepath = TEX_FILE_PLAYER_LIVES,
      .m_alpha    = true,
      .m_flip     = true,
      .m_wrapS    = GL_CLAMP_TO_EDGE,
      .m_wrapT    = GL_CLAMP_TO_EDGE
    });
    IDs::SID_TEX_PLAYER_LIVES = fnv1a(TEX_FILE_PLAYER_LIVES);
    m_textures[IDs::SID_TEX_PLAYER_LIVES] = std::make_unique<Texture2D>(t.m_width, t.m_height, t.m_id);
    // invaders main image
    t = loadTexFromFile(LoadTexFromFileArgs{
      .m_filepath = TEX_FILE_INVADERS,
      .m_alpha    = true,
      .m_flip     = true,
      .m_wrapS    = GL_CLAMP_TO_EDGE,
      .m_wrapT    = GL_CLAMP_TO_EDGE
    });
    IDs::SID_TEX_INVADERS = fnv1a(TEX_FILE_INVADERS);
    m_textures[IDs::SID_TEX_INVADERS] = std::make_unique<Texture2D>(t.m_width, t.m_height, t.m_id);
    // -----------------------------------------------
    // shaders, since every shader is different, call different functions...
    // -----------------------------------------------
    // it doesn't matter if you use the vertex or fragment shader file name
    // for the id of the shader, what matters is if it's unique
    // define common proj matrix
    //
    const auto m = orthographic({
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
                           FRAG_SHADER_FILE_MAIN_BACKGROUND)
    );
    IDs::SID_SHADER_ALIEN = fnv1a(VERT_SHADER_FILE_ALIEN);
    m_shaders[IDs::SID_SHADER_ALIEN] = std::make_unique<Shader>(
      loadAlienShader(VERT_SHADER_FILE_ALIEN,
                      FRAG_SHADER_FILE_ALIEN,
                      m)
    );
    IDs::SID_SHADER_PLAYER_LIVES = fnv1a(VERT_SHADER_FILE_PLAYER_LIVES);
    m_shaders[IDs::SID_SHADER_PLAYER_LIVES] = std::make_unique<Shader>(
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
    // NOTE: since missile and alien shaders are the same, you need to use different
    // names for the hash to work, otherwise you'll be overriding them!
    IDs::SID_SHADER_MISSILE_ALIEN = fnv1a(FRAG_SHADER_FILE_MISSILE_ALIEN);
    m_shaders[IDs::SID_SHADER_MISSILE_ALIEN] = std::make_unique<Shader>(
      loadMissileShader(VERT_SHADER_FILE_MISSILE_ALIEN,
                        FRAG_SHADER_FILE_MISSILE_ALIEN,
                        m)
    );
    // -----------------------------------------------
    // text menu
    // -----------------------------------------------
    IDs::SID_SHADER_TEXT = fnv1a(VERT_SHADER_FILE_TEXT);
    m_shaders[IDs::SID_SHADER_TEXT] = std::make_unique<Shader>(loadTextShader(VERT_SHADER_FILE_TEXT, FRAG_SHADER_FILE_TEXT, m));
    // -----------------------------------------------
    // menu
    // -----------------------------------------------
    IDs::SID_SHADER_MENU = fnv1a(VERT_SHADER_FILE_MENU);
    m_shaders[IDs::SID_SHADER_MENU] = std::make_unique<Shader>(loadMenuShader(VERT_SHADER_FILE_MENU, FRAG_SHADER_FILE_MENU));
    //  -----------------------------------------------
    // basic shader, this is used for rendering text perhaps w/ a bg image
    //  -----------------------------------------------
    IDs::SID_SHADER_BASIC = fnv1a(VERT_SHADER_FILE_BASIC);
    m_shaders[IDs::SID_SHADER_BASIC] = std::make_unique<Shader>(loadBasicShader(VERT_SHADER_FILE_BASIC, FRAG_SHADER_FILE_BASIC));
    // uniform caching
    m_uniforms.reserve(8);
    m_eventManager.subscribe(EventType::MenuIncreaseVolume, [this](const Event&){
      increaseVolume();
    });
    m_eventManager.subscribe(EventType::MenuDecreaseVolume, [this](const Event&){
      decreaseVolume();
    });
  }

  ResourceManager::~ResourceManager()
  {
    for(auto& t : m_textures) {
      glDeleteTextures(1, &t.second->m_id);
    }
    for(auto& s : m_shaders) {
      glDeleteProgram(s.second->m_id);
    }
    closeAudioSystem();
  }

  bool ResourceManager::checkCompileErrors(const unsigned int object, const ShaderType type)
  {
    int success{ 0 };
    std::array<char, 512> log{ '\0' };
    std::string shader;
    switch(type) {
      case ShaderType::VERTEX:
        shader = "VERTEX";
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if(success != GL_TRUE) {
          glGetShaderInfoLog(object, log.size(), nullptr, &log[0]);
          std::cerr << "Couldn't compile shader type " << shader << " : " << log.data() << '\n';
          return true;
        }
        break;
      case ShaderType::FRAGMENT:
        shader = "FRAGMENT";
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if(success != GL_TRUE) {
          glGetShaderInfoLog(object, log.size(), nullptr, &log[0]);
          std::cerr << "Couldn't compile shader type " << shader << " : " << log.data() << '\n';
          return true;
        }
        break;
      case ShaderType::GEOMETRY:
        shader = "GEOMETRY";
        glGetShaderiv(object, GL_COMPILE_STATUS, &success);
        if(success != GL_TRUE) {
          glGetShaderInfoLog(object, log.size(), nullptr, &log[0]);
          std::cerr << "Couldn't compile shader type " << shader << " : " << log.data() << '\n';
          return true;
        }
        break;
      case ShaderType::PROGRAM:
        glGetProgramiv(object, GL_LINK_STATUS, &success);
        if(success != GL_TRUE) {
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
    bool errors = checkCompileErrors(vertId, ShaderType::VERTEX);
    if(errors) {
      glDeleteShader(vertId);
      std::exit(EXIT_FAILURE);
    }
    fragId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragId, 1, &fragCodeC, nullptr);
    glCompileShader(fragId);
    errors = checkCompileErrors(fragId, ShaderType::FRAGMENT);
    if(errors) {
      glDeleteShader(vertId);
      glDeleteShader(fragId);
      std::exit(EXIT_FAILURE);
    }
    unsigned int programId = glCreateProgram();
    glAttachShader(programId, vertId);
    glAttachShader(programId, fragId);
    glLinkProgram(programId);
    errors = checkCompileErrors(programId, ShaderType::PROGRAM);
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
                                               const char* fragpath)
  {
    const auto id = loadCompileShaders(vertpath, fragpath);
    if(!id) {
      std::cerr << __FUNCTION__ << " Error creating shader\n";
      exit(EXIT_FAILURE);
    }
    constexpr float vertices[] {
      // pos        -- tex
      -1.0f, -1.0f, 0.0f, 0.0f,  // bottom left
       1.0f, -1.0f, 1.0f, 0.0f,  // bottom right
      -1.0f,  1.0f, 0.0f, 1.0f,  // top left

      -1.0f,  1.0f, 0.0f, 1.0f,  // top left
       1.0f,  1.0f, 1.0f, 1.0f,  // top right
       1.0f, -1.0f, 1.0f, 0.0f,  // bottom right
    };
    unsigned int VBO, VAO;
    glGenBuffers(1, &VBO);
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    useShaderProgram(id);
    setUniformInt(id, "image", 0);
    return Shader(id, VAO, VBO);
  }

  Shader ResourceManager::loadAlienShader(const char* vertpath, const char* fragpath, const Math::m4& m)
  {
    const auto id = loadCompileShaders(vertpath, fragpath);
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
    const auto id = loadCompileShaders(vertpath, fragpath);
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

  Shader ResourceManager::loadTextShader(const char* vertpath, const char* fragpath, const m4& m)
  {
    const auto id = loadCompileShaders(vertpath, fragpath);
    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    useShaderProgram(id);
    setUniformInt(id, "image", 0);
    setUniformMat4(id, "projection", m);
    return Shader(id, VAO, VBO);
  }

  Shader ResourceManager::loadMenuShader(const char* vertpath, const char* fragpath)
  {
    const auto id = loadCompileShaders(vertpath, fragpath);
    unsigned int VAO, VBO;
    constexpr float vertices[] = {
      -0.5f, -0.5f,
       0.5f,  0.5f,
      -0.5f,  0.5f,

      -0.5f, -0.5f,
       0.5f, -0.5f,
       0.5f,  0.5f
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(0));
    return Shader(id, VAO, VBO);
  }

  Shader ResourceManager::loadBasicShader(const char* vertpath, const char* fragpath)
  {
    const auto id = loadCompileShaders(vertpath, fragpath);
    unsigned int VAO, VBO;
    constexpr float vertices[] = {
      -1.0f, -1.0f, 0.0f, 0.0f,
       1.0f,  1.0f, 1.0f, 1.0f,
      -1.0f,  1.0f, 0.0f, 1.0f,

      -1.0f, -1.0f, 0.0f, 0.0f,
       1.0f, -1.0f, 1.0f, 0.0f,
       1.0f,  1.0f, 1.0f, 1.0f,
    };
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0));
    useShaderProgram(id);
    setUniformInt(id, "image", 0);
    return Shader(id, VAO, VBO);
  }

  void ResourceManager::playAudioTrack(const int sid, const bool loop) const noexcept
  {
    Game::playAudioTrack(m_audioTracks.at(sid).get(), loop);
  }

  void ResourceManager::stopAudioTrack(const int sid, const unsigned int delay) const noexcept
  {
    Game::stopAudioTrack(m_audioTracks.at(sid).get(), delay);
  }

  void ResourceManager::increaseVolume() const noexcept
  {
    Game::increaseVolume();
  }

  void ResourceManager::decreaseVolume() const noexcept
  {
    Game::decreaseVolume();
  }

  float ResourceManager::getVolume() const noexcept
  {
    return Game::getNormalizedVolumeValue();
  }

  void ResourceManager::initAudio(AudioDevice audioDevice)
  {
    IDs::SID_AUDIO_BG_MUSIC   = fnv1a(AUDIO_FILE_BG_MUSIC);
    IDs::SID_AUDIO_EXPLOSION  = fnv1a(AUDIO_FILE_EXPLOSION);
    IDs::SID_AUDIO_LOSE_GAME  = fnv1a(AUDIO_FILE_LOSE_GAME);
    IDs::SID_AUDIO_WIN_GAME   = fnv1a(AUDIO_FILE_WIN_GAME);
    IDs::SID_AUDIO_PLAYER_DIE = fnv1a(AUDIO_FILE_PLAYER_DIE);
    IDs::SID_AUDIO_WIN_LEVEL  = fnv1a(AUDIO_FILE_WIN_LEVEL);
    Game::initAudioSystem(audioDevice);
    m_audioTracks[IDs::SID_AUDIO_BG_MUSIC]   = Game::openAudioFile(AUDIO_FILE_BG_MUSIC,   AudioType::MUSIC);
    m_audioTracks[IDs::SID_AUDIO_EXPLOSION]  = Game::openAudioFile(AUDIO_FILE_EXPLOSION,  AudioType::EFFECT);
    m_audioTracks[IDs::SID_AUDIO_LOSE_GAME]  = Game::openAudioFile(AUDIO_FILE_LOSE_GAME,  AudioType::EFFECT);
    m_audioTracks[IDs::SID_AUDIO_WIN_GAME]   = Game::openAudioFile(AUDIO_FILE_WIN_GAME,   AudioType::EFFECT);
    m_audioTracks[IDs::SID_AUDIO_PLAYER_DIE] = Game::openAudioFile(AUDIO_FILE_PLAYER_DIE, AudioType::EFFECT);
    m_audioTracks[IDs::SID_AUDIO_WIN_LEVEL]  = Game::openAudioFile(AUDIO_FILE_WIN_LEVEL,  AudioType::EFFECT);
  }
};
