#include "invaders_sim.h"
#include "common.h"
#include "invaders_enemy.h"
#include "invaders_missile.h"
#include "invaders_resources.h"
#include "invaders_renderer.h"
#include "invaders_player.h"
#include "invaders_explosion.h"

extern Res::ResourceManager gResourceManager;
extern Renderer::RendererManager gRendererManager;
extern Game::PlayerManager gPlayerManager;
extern Game::EnemyManager gEnemyManager;
extern Game::MissileManager gMissileManager;
extern Game::ExplosionManager gExplosionManager;

namespace Sim {
  using namespace Renderer;

  SimulationManager::SimulationManager()
  {
  }

  SimulationManager::~SimulationManager()
  {

  }

  void SimulationManager::init()
  {
    m_state = State::PLAY;
    m_end = false;
    // init scene's settings
    m_sceneWidth  = WINDOW_WIDTH;
    m_sceneHeight = WINDOW_HEIGHT;
  }

  void SimulationManager::close()
  {

  }

  void SimulationManager::update(const float delta)
  {
    if(m_state == State::PLAY) {
      // order is important: everything that moves and can collide to something else needs to be
      // updated before the grid. Explosions are an exception, for example, but put it before bc
      // it doesn't matter
      gPlayerManager.update(delta, m_sceneWidth, m_sceneHeight);
      gEnemyManager.update(delta);
      gMissileManager.update(delta, m_sceneHeight);
      gExplosionManager.update(delta);
      updateGrid();
      gRendererManager.render(RenderArgs{
        .aliensToDraw         = gEnemyManager.numAliveAliens(),
        .playerLivesToDraw    = gPlayerManager.currlives(),
        .explosionsToDraw     = gExplosionManager.numActiveExplosions(),
        .playerMissilesToDraw = gMissileManager.numActivePlayerMissiles(),
        .alienMissilesToDraw  = gMissileManager.numActiveAlienMissiles(),
        .playersToDraw        = 1
      });
    }
    checkGameState();
  }

  void SimulationManager::checkGameState()
  {
    if(gPlayerManager.currlives() == 0) {
      m_state = State::LOSE;
      return;
    }
  }
};
