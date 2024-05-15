#include "invaders_sim.h"
#include "common.h"
#include "invaders_enemy.h"
#include "invaders_input.h"
#include "invaders_menu.h"
#include "invaders_missile.h"
#include "invaders_resources.h"
#include "invaders_renderer.h"
#include "invaders_player.h"
#include "invaders_explosion.h"
#include "invaders_grid.h"

namespace Sim {
  using namespace Renderer;
  using namespace Res;
  using namespace Input;
  using namespace Game;
  using namespace Ev;

  SimulationManager::SimulationManager(const Res::ResourceManager& resourceManager,
                                       InputManager& inputManager,
                                       PlayerManager& playerManager,
                                       EnemyManager& enemyManager,
                                       MissileManager& missileManager,
                                       ExplosionManager& explosionManager,
                                       GridManager& gridManager,
                                       RendererManager& renderManager,
                                       MenuManager& menuManager,
                                       EventManager& eventManager,
                                       LevelManager& levelManager,
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
      m_menuManager{ menuManager },
      m_eventManager{ eventManager },
      m_levelManager{ levelManager },
      m_levelLabelAlpha { 1.0f },
      m_sceneWidth{ sceneWidth },
      m_sceneHeight{ sceneHeight },
      m_state{ State::START },
      m_end{ false },
      m_renderNewLevelLabel{ false },
      m_playedEffect{ false },
      m_playedBgMusic{ false }
  {
    m_eventManager.subscribe(EventType::MenuQuit, [this](const Event&){
      setShouldEnd(true);
    });
    m_eventManager.subscribe(EventType::MenuContinue, [this](const Event&){
      m_state = State::PLAY;
    });
    // here? or in resources??? :thinking:
    m_eventManager.subscribe(EventType::MenuIncreaseVolume, [this](const Event&){
      m_resourceManager.increaseVolume();
    });
    m_eventManager.subscribe(EventType::MenuDecreaseVolume, [this](const Event&){
      m_resourceManager.decreaseVolume();
    });
    m_eventManager.subscribe(EventType::AlienDestroyed, [this](const Event& ev){
      increasePlayerPoints(ev);
    });
  }

  SimulationManager::~SimulationManager()
  {

  }

  void SimulationManager::update(const float delta)
  {
    if(m_inputManager.isKeyPressed(Key::KEY_Q)) {
      m_end = true;
      return;
    }
    if(m_state == State::PLAY) {
      if(!m_playedBgMusic) {
	m_resourceManager.playAudioTrack(IDs::SID_AUDIO_BG_MUSIC, true);
	m_playedBgMusic = true;
      }
      m_playedEffect = false;
      if(m_inputManager.isKeyPressed(Key::KEY_ESCAPE)) {
        m_state = State::MENU;
        return;
      }
      // order is important: everything that moves and can collide to something else needs to be
      // updated before the grid. Explosions are an exception bc there's no need to track them
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
        .playersToDraw        = 1,
	.playerPoints         = m_playerManager.getPlayerPoints()
      });
      if(m_renderNewLevelLabel) {
        m_renderManager.renderLevelLabel(m_levelManager.currentLevel(), m_levelLabelAlpha);
        m_levelLabelAlpha -= delta;
        if(m_levelLabelAlpha < 0.0f) {
          m_renderNewLevelLabel = false;
        }
      }
    } else if(m_state == State::MENU) {
      m_menuManager.update();
      m_renderManager.renderMenu();
    } else if(m_state == State::WIN_GAME) {
      m_playedBgMusic = false;
      // don't know where to put this, state manager? for now it will be here
      winScreenHandleInput();
      m_renderManager.renderWinScreen();
      // TODO: there's a problem here, need to get this bool out somwhow; problem
      // is that the state transition is not instant, so you can't just keep playing
      // the sound; that doesn't happen with the win level sound bc the state changes
      // in one frame
      if(!m_playedEffect) {
	m_resourceManager.stopAudioTrack(IDs::SID_AUDIO_BG_MUSIC, 0);
	m_resourceManager.playAudioTrack(IDs::SID_AUDIO_WIN_GAME, false);
	m_playedEffect = true;
      }
    } else if(m_state == State::WIN_LEVEL) {
      m_levelManager.changeLevel();
      m_state = State::PLAY;
      clearLevel();
      m_renderNewLevelLabel = true;
      m_levelLabelAlpha = 1.0f;
      m_resourceManager.playAudioTrack(IDs::SID_AUDIO_WIN_LEVEL, false);
    } else if(m_state == State::LOSE) {
      m_playedBgMusic = false;
      loseScreenHandleInput();
      m_renderManager.renderLoseScreen();
      // TODO: remove bool here too
      if(!m_playedEffect) {
	m_resourceManager.stopAudioTrack(IDs::SID_AUDIO_BG_MUSIC, 0);
	m_resourceManager.playAudioTrack(IDs::SID_AUDIO_LOSE_GAME, false);
	m_playedEffect = true;
      }
    } else if(m_state == State::START) {
      startScreenHandleInput();
      m_renderManager.renderStartScreen();
    }
    checkGameState();
  }

  void SimulationManager::checkGameState()
  {
    if(m_playerManager.currlives() == 0) {
      m_state = State::LOSE;
    } else if(m_enemyManager.numAliveAliens() == 0 && m_levelManager.lastLevel()) {
      m_state = State::WIN_GAME;
    } else if(m_enemyManager.numAliveAliens() == 0) {
      m_state = State::WIN_LEVEL;
    }
  }

  void SimulationManager::clearLevel()
  {
    // when a new game level is loaded, you need to clear previous' frame mess
    // and also reset player's position, aliens are loaded by the level manager
    m_playerManager.resetPos();
    m_missileManager.clearMissiles();
  }

  void SimulationManager::winScreenHandleInput()
  {
    if(m_inputManager.isKeyPressed(Key::KEY_SPACE) || m_inputManager.isKeyPressed(Key::KEY_ENTER)) {
      resetGame();
      m_state = State::PLAY;
    } else if(m_inputManager.isKeyPressed(Key::KEY_Q) || m_inputManager.isKeyPressed(Key::KEY_ESCAPE)) {
      m_end = true;
    }
  }

  void SimulationManager::loseScreenHandleInput()
  {
    if(m_inputManager.isKeyPressed(Key::KEY_SPACE) || m_inputManager.isKeyPressed(Key::KEY_ENTER)) {
      resetGame();
      m_state = State::PLAY;
    } else if(m_inputManager.isKeyPressed(Key::KEY_Q) || m_inputManager.isKeyPressed(Key::KEY_ESCAPE)) {
      m_end = true;
    }
  }

  void SimulationManager::startScreenHandleInput()
  {
    if(m_inputManager.isKeyPressed(Key::KEY_SPACE) || m_inputManager.isKeyPressed(Key::KEY_ENTER)) {
      m_state = State::PLAY;
      m_renderNewLevelLabel = true;
      m_levelLabelAlpha = 1.0f;
    } else if(m_inputManager.isKeyPressed(Key::KEY_Q) || m_inputManager.isKeyPressed(Key::KEY_ESCAPE)) {
      m_end = true;
    }
  }

  void SimulationManager::resetGame()
  {
    // NOTE: careful, first you reset enemies, then level (bc lvl loads enemies)
    m_enemyManager.reset();
    m_levelManager.reset();
    m_playerManager.reset();
    m_missileManager.clearMissiles();
    m_explosionManager.reset();
  }

  void SimulationManager::increasePlayerPoints(const Event& event)
  {
    unsigned int pts;
    // bit of a cursed code
    switch(reinterpret_cast<Alien*>(event.getEntity())->m_type) {
    case AlienType::YELLOW:
      pts = 50;
      break;
    case AlienType::BEIGE:
      pts = 100;
      break;
    case AlienType::GREEN:
      pts = 100;
      break;
    case AlienType::PINK:
      pts = 200;
      break;
    case AlienType::BLUE:
      pts = 200;
      break;
    }
    m_playerManager.increasePoints(pts);
  }
};
