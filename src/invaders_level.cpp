#include "invaders_level.h"
#include "invaders_enemy.h"
#include "invaders_math.h"

namespace Game {
  using namespace Math;

  LevelManager::LevelManager(EnemyManager& enemyManager)
    : m_enemyManager{ enemyManager },
      m_currentLevel{ 0 }
  {
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
    // for now let's keep it simple
    static constexpr AlienType first[4][4] {
      { AlienType::BEIGE,  AlienType::BEIGE,  AlienType::BEIGE,  AlienType::BEIGE  },
      { AlienType::BEIGE,  AlienType::BEIGE,  AlienType::BEIGE,  AlienType::BEIGE  },
      { AlienType::YELLOW, AlienType::YELLOW, AlienType::YELLOW, AlienType::YELLOW },
      { AlienType::YELLOW, AlienType::YELLOW, AlienType::YELLOW, AlienType::YELLOW }
    };
    static constexpr AlienType second[4][4] {
      { AlienType::BLUE,   AlienType::BLUE,   AlienType::BLUE,   AlienType::BLUE   },
      { AlienType::YELLOW, AlienType::YELLOW, AlienType::YELLOW, AlienType::YELLOW },
      { AlienType::YELLOW, AlienType::BEIGE,  AlienType::YELLOW, AlienType::BEIGE  },
      { AlienType::BEIGE,  AlienType::BEIGE,  AlienType::BEIGE,  AlienType::BEIGE  },
    };
    static constexpr AlienType third[4][4] {
      { AlienType::PINK,   AlienType::PINK,   AlienType::PINK,   AlienType::PINK   },
      { AlienType::BLUE,   AlienType::BLUE,   AlienType::BLUE,   AlienType::BLUE   },
      { AlienType::BEIGE,  AlienType::BEIGE,  AlienType::BEIGE,  AlienType::BEIGE  },
      { AlienType::YELLOW, AlienType::YELLOW, AlienType::YELLOW, AlienType::YELLOW },
    };
    static constexpr AlienType fourth[4][4] {
      { AlienType::PINK,   AlienType::PINK,   AlienType::PINK,   AlienType::PINK   },
      { AlienType::BLUE,   AlienType::BLUE,   AlienType::BLUE,   AlienType::BLUE   },
      { AlienType::GREEN,  AlienType::GREEN,  AlienType::GREEN,  AlienType::GREEN  },
      { AlienType::YELLOW, AlienType::YELLOW, AlienType::YELLOW, AlienType::YELLOW },
    };
    const AlienType (*levels[])[4][4] {
      &first,
      &second,
      &third,
      &fourth
    };
    static constexpr float Xspacing{ 10.0f };
    static constexpr float Yspacing{ 120.0f };
    // yooo can't be bothered, ideally you should have an in-game editor to place entities there,
    // but you don't
    for(int i{ 0 }; i < 4; ++i) {
      for(int j{ 0 }; j < 4; ++j) {
        const auto alienType = (*levels[m_currentLevel])[i][j];
        const auto pos = v3{
          .x = 450.0f + Xspacing * j * 10.0f,
          .y = 870.0f - i * Yspacing,
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
};
