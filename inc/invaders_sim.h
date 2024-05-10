#pragma once

#include "invaders_enemy.h"
#include "invaders_explosion.h"
#include "invaders_grid.h"
#include "invaders_input.h"
#include "invaders_missile.h"
#include "invaders_player.h"
#include "invaders_renderer.h"
#include "invaders_resources.h"

namespace Sim {
  // NOTE: since the game doesn't allow window resizing (bc don't know how to handle
  // it properly yet) all game units will be pixels.

  enum class State {
    PLAY,
    MENU,
    WIN_LEVEL,
    WIN_GAME,
    LOSE,
  };

  class SimulationManager final {
  public:
    SimulationManager(const Res::ResourceManager& resourceManager,
                      Input::InputManager& inputManager,
                      Game::PlayerManager& playerManager,
                      Game::EnemyManager& enemyManager,
                      Game::MissileManager& missileManager,
                      Game::ExplosionManager& explosionManager,
                      Game::GridManager& gridManager,
                      Renderer::RendererManager& renderManager,
                      const int sceneWidth,
                      const int sceneHeight);
    ~SimulationManager();
    void update(const float delta);
    inline auto shouldEnd()   const { return m_end; }
    inline auto sceneWidth()  const { return m_sceneWidth; }
    inline auto sceneHeight() const { return m_sceneHeight; }
    inline void setShouldEnd(const bool end) { m_end = end; }
  private:
    void checkGameState();
    const Res::ResourceManager& m_resourceManager;
    Input::InputManager& m_inputManager;
    Game::PlayerManager& m_playerManager;
    Game::EnemyManager& m_enemyManager;
    Game::MissileManager& m_missileManager;
    Game::ExplosionManager& m_explosionManager;
    Game::GridManager& m_gridManager;
    Renderer::RendererManager& m_renderManager;
    int m_sceneWidth;
    int m_sceneHeight;
    State m_state;
    bool m_end;
  };

};
