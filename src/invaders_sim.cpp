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

  SimulationManager::SimulationManager(Res::ResourceManager& resourceManager,
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
    m_currentAudioSelection{ 0 },
    m_state{ State::AUDIO_DEV_SELECTION },
    m_end{ false },
    m_renderNewLevelLabel{ false },
    m_playedEffect{ false },
    m_playedBgMusic{ false },
    m_topfive{ false }
  {
    m_eventManager.subscribe(EventType::MenuQuit, [this](const Event&){
      setShouldEnd(true);
    });
    m_eventManager.subscribe(EventType::MenuContinue, [this](const Event&){
      m_state = State::PLAY;
    });
    m_eventManager.subscribe(EventType::AlienDestroyed, [this](const Event& ev){
      auto data = ev.getData();
      if(std::holds_alternative<Alien*>(data)) {
	increasePlayerPoints(std::get<Alien*>(data)->m_type);
      }
    });
  }

  SimulationManager::~SimulationManager()
  {

  }

  void SimulationManager::processPlayState(const float delta)
  {
    m_topfive = false;
    if(!m_playedBgMusic) {
      m_resourceManager.playAudioTrack(IDs::SID_AUDIO_BG_MUSIC, true);
      m_playedBgMusic = true;
    }
    m_playedEffect = false;
    if(m_inputManager.isKeyPressed(Key::KEY_ESCAPE)) {
      m_state = State::MENU;
      return;
    }
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
  }

  void SimulationManager::processMenuState()
  {
    m_menuManager.update();
    m_renderManager.renderMenu();
  }

  void SimulationManager::processEndGameState()
  {
    m_playedBgMusic = false;
    if(m_inputManager.isKeyPressed(Key::KEY_ENTER)) {
      resetGame();
      m_state = State::PLAY;
    } else if(m_inputManager.isKeyPressed(Key::KEY_ESCAPE)) {
      m_end = true;
    }
    // @YOLO: you're using the same bool flag for two things, disaster waiting to happen
    if(!m_playedEffect) {
      m_playedEffect = true;
      m_resourceManager.stopAudioTrack(IDs::SID_AUDIO_BG_MUSIC, 0);
      m_topfive = Game::saveAndGetScores(m_playerManager.getPlayerPoints(), m_scores);
      if(m_topfive) {
	m_resourceManager.playAudioTrack(IDs::SID_AUDIO_WIN_GAME, false);
      } else {
	m_resourceManager.playAudioTrack(IDs::SID_AUDIO_LOSE_GAME, false);
      }
    }
    m_renderManager.renderEnd(m_scores, m_topfive);
  }

  void SimulationManager::processWinLevelState()
  {
    m_levelManager.changeLevel();
    m_state = State::PLAY;
    clearLevel();
    m_renderNewLevelLabel = true;
    m_levelLabelAlpha = 1.0f;
    m_resourceManager.playAudioTrack(IDs::SID_AUDIO_WIN_LEVEL, false);
  }

  void SimulationManager::processStartState()
  {
    if(m_inputManager.isKeyPressed(Key::KEY_SPACE) || m_inputManager.isKeyPressed(Key::KEY_ENTER)) {
      m_state = State::PLAY;
      m_renderNewLevelLabel = true;
      m_levelLabelAlpha = 1.0f;
    } else if(m_inputManager.isKeyPressed(Key::KEY_ESCAPE)) {
      m_end = true;
    }
    m_renderManager.renderStart();
  }

  void SimulationManager::processAudioDeviceSelectionState()
  {
    auto devices = Game::getAudioDevices();
    if(m_inputManager.isKeyPressed(Key::KEY_UP) || m_inputManager.isKeyPressed(Key::KEY_W)) {
      m_currentAudioSelection = ((m_currentAudioSelection - 1) + (devices.size())) % (devices.size());
    } else if(m_inputManager.isKeyPressed(Key::KEY_DOWN) || m_inputManager.isKeyPressed(Key::KEY_S)) {
      m_currentAudioSelection = (m_currentAudioSelection + 1) % (devices.size());
    } else if(m_inputManager.isKeyPressed(Key::KEY_ENTER) || m_inputManager.isKeyPressed(Key::KEY_SPACE)) {
      m_resourceManager.initAudio(devices[m_currentAudioSelection]);
      m_state = State::START;
    }
    m_renderManager.renderAudioDeviceSelection(devices, m_currentAudioSelection);
  }

  void SimulationManager::update(const float delta)
  {
    if(m_inputManager.isKeyPressed(Key::KEY_Q)) {
      m_end = true;
      return;
    }
    if(m_state == State::PLAY) {
      processPlayState(delta);
    } else if(m_state == State::MENU) {
      processMenuState();
    } else if(m_state == State::END) {
      processEndGameState();
    } else if(m_state == State::WIN_LEVEL) {
      processWinLevelState();
    } else if(m_state == State::START) {
      processStartState();
    } else if(m_state == State::AUDIO_DEV_SELECTION) {
      processAudioDeviceSelectionState();
    }
    checkGameState();
  }

  void SimulationManager::checkGameState()
  {
    if(m_playerManager.currlives() == 0) {
      m_state = State::END;
    } else if(m_enemyManager.numAliveAliens() == 0) {
      m_state = State::WIN_LEVEL;
    }
  }

  void SimulationManager::clearLevel()
  {
    m_playerManager.resetPos();
    m_missileManager.clearMissiles();
  }

  void SimulationManager::resetGame()
  {
    // @NOTE: careful, first you reset enemies, then level (bc lvl loads enemies)
    m_enemyManager.reset();
    m_levelManager.reset();
    m_playerManager.reset();
    m_missileManager.clearMissiles();
    m_explosionManager.reset();
  }

  void SimulationManager::increasePlayerPoints(const AlienType type)
  {
    unsigned int pts;
    switch(type) {
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
