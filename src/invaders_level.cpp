#include "invaders_level.h"
#include "invaders_enemy.h"
#include "invaders_math.h"

#include <cstdlib>

static constexpr auto kXspacing = 10.0f;
static constexpr auto kYspacing = 120.0f;

namespace Game {
  using namespace Math;

  LevelManager::LevelManager(EnemyManager& enemyManager)
    : m_enemyManager{ enemyManager },
      m_currentLevel{ 0 }
  {
    // ensure that you get different aliens per level each time you run the game
    std::srand(std::time(0));
    loadLevel();
  }

  LevelManager::~LevelManager()
  {

  }

  void LevelManager::changeLevel()
  {
    ++m_currentLevel;
    loadLevel();
  }

  void LevelManager::loadLevel()
  {
    // generate a "random" level every time
    for(int i{ 0 }; i < 4; ++i) {
      for(int j{ 0 }; j < 4; ++j) {
        const auto alienType = getRandomAlien();
        const auto pos = v3{
          .x = 450.0f + kXspacing * j * 10.0f,
          .y = 870.0f - i * kYspacing,
          .z = 0.0f
        };
        m_enemyManager.spawnAlien(pos, alienType);
      }
    }
  }

  void LevelManager::reset()
  {
    m_currentLevel = 0;
    loadLevel();
  }

  AlienType LevelManager::getRandomAlien() const noexcept
  {
    return static_cast<AlienType>(std::rand() % (static_cast<int>(AlienType::COUNT) - 1));
  }

};
