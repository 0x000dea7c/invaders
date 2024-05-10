#include "invaders_sim.h"
#include "invaders_enemy.h"
#include "invaders_input.h"
#include "invaders_missile.h"
#include "invaders_resources.h"
#include "invaders_renderer.h"
#include "invaders_player.h"
#include "invaders_explosion.h"
#include "invaders_grid.h"

#include <iostream>

namespace Sim {
  using namespace Renderer;

  SimulationManager::SimulationManager(const Res::ResourceManager& resourceManager,
                                       Input::InputManager& inputManager,
                                       Game::PlayerManager& playerManager,
                                       Game::EnemyManager& enemyManager,
                                       Game::MissileManager& missileManager,
                                       Game::ExplosionManager& explosionManager,
                                       Game::GridManager& gridManager,
                                       Renderer::RendererManager& renderManager,
                                       const int sceneWidth,
                                       const int sceneHeight)
    : m_resourceManager{ resourceManager },
      m_inputManager{ inputManager },
      m_playerManager{ playerManager },
      m_enemyManager{ enemyManager },
      m_missileManager{ missileManager },
      m_explosionManager{ explosionManager },
      m_gridManager{ gridManager },
      m_renderManager{ renderManager },
      m_sceneWidth{ sceneWidth },
      m_sceneHeight{ sceneHeight },
      m_state{ State::PLAY },
      m_end{ false }
  {

  }

  SimulationManager::~SimulationManager()
  {

  }

  void SimulationManager::update(const float delta)
  {
    if(m_inputManager.isKeyPressed(Input::Key::KEY_ESCAPE)) {
      m_end = true;
      return;
    }
    if(m_state == State::PLAY) {
      // order is important: everything that moves and can collide to something else needs to be
      // updated before the grid. Explosions are an exception, for example, but put it before bc
      // it doesn't matter
      m_gridManager.beginFrame();
      m_playerManager.update(delta, m_sceneWidth, m_sceneHeight);
      m_enemyManager.update(delta);
      m_missileManager.update(delta, m_sceneHeight);
      m_explosionManager.update(delta);
      m_renderManager.render(RenderArgs{
        .aliensToDraw         = m_enemyManager.numAliveAliens(),
        .playerLivesToDraw    = m_playerManager.currlives(),
        .explosionsToDraw     = m_explosionManager.numActiveExplosions(),
        .playerMissilesToDraw = m_missileManager.numActivePlayerMissiles(),
        .alienMissilesToDraw  = m_missileManager.numActiveAlienMissiles(),
        .playersToDraw        = 1
      });
    }
    checkGameState();
  }

  void SimulationManager::checkGameState()
  {
    if(m_playerManager.currlives() == 0) {
      m_state = State::LOSE;
      return;
    }
  }
};
