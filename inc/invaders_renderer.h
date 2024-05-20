#pragma once

#include "invaders_math.h"
#include "invaders_resources.h"
#include "invaders_text.h"
#include "invaders_menu.h"

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
    unsigned long long aliensToDraw;
    unsigned int playerLivesToDraw;
    unsigned long long explosionsToDraw;
    unsigned long long playerMissilesToDraw;
    unsigned long long alienMissilesToDraw;
    unsigned long playersToDraw;
    unsigned int playerPoints;
  };

  class RendererManager final {
  public:
    RendererManager(Res::ResourceManager& resourceManager,
                    TextRenderer& textRenderer,
                    const Game::MenuManager& menuManager);
    ~RendererManager();
    void render(const RenderArgs& args);
    void renderMenu();
    void renderEnd(const std::array<Game::ScoreEntry, 5>& scores, const bool topfive);
    void renderStart();
    void renderLevelLabel(const unsigned int currentLevel, const float alpha);
    void renderAudioDeviceSelection(const std::vector<Game::AudioDevice>& devices, const unsigned int currentOption);
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
    Res::Shader* m_menuShader;
    Res::ResourceManager& m_resourceManager;
    TextRenderer& m_textRenderer;
    const Game::MenuManager& m_menuManager;
    Res::Shader* m_basicShader;
    Res::Texture2D* m_invadersTexture;
  };

};
