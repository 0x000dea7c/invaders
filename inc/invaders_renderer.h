#pragma once

#include "invaders_math.h"
#include "invaders_resources.h"
#include <array>

namespace Res {
  class Shader;
  class Texture2D;
};

namespace Renderer {

  class InstanceData final {
  public:
    Math::m4 m_model{ Math::identity() };
    std::array<Math::v4, 6> m_vertexData;
  };

  class ExplosionInstanceData final {
  public:
    InstanceData m_instanceData;
    Math::v4 m_colour{ 1.0f, 1.0f, 1.0f, 1.0f };
  };

  class RenderArgs final {
  public:
    unsigned long aliensToDraw;
    int playerLivesToDraw;
    unsigned long explosionsToDraw;
    unsigned long playerMissilesToDraw;
    unsigned long alienMissilesToDraw;
    unsigned long playersToDraw;
  };

  // renders all given data, no ifs, no updates, just render. This class is omoiiiiiiiiiiiiii
  class RendererManager final {
  public:
    RendererManager(const Res::ResourceManager& resourceManager);
    ~RendererManager();
    // no idea what's the good approach here, but for now it's basic
    void render(const RenderArgs& args);
  private:
    // TODO: these are all references, not fucking pointers
    Res::Shader* m_backgroundShader;
    Res::Texture2D* m_backgroundTex;
    Res::Shader* m_alienShader;
    Res::Texture2D* m_alienTex;
    Res::Shader* m_playerLivesShader;
    Res::Texture2D* m_playerLivesTex;
    Res::Shader* m_playerShader;
    Res::Texture2D* m_playerTex;
    Res::Shader* m_explosionShader;
    Res::Texture2D* m_explosionTex;
    Res::Shader* m_missilePlayerShader;
    Res::Texture2D* m_missilePlayerTex;
    Res::Shader* m_missileAlienShader;
    Res::Texture2D* m_missileAlienTex;
    const Res::ResourceManager& m_resourceManager;
  };

};
