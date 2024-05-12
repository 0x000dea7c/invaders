#pragma once

#include "invaders_enemy.h"

namespace Game {

  constexpr unsigned int kLevels{ 3 };

  class LevelManager final {
  public:
    LevelManager(EnemyManager& enemyManager);
    ~LevelManager();
    void changeLevel();
    inline auto lastLevel() const noexcept { return m_currentLevel == kLevels; }
    void reset();
  private:
    void loadLevel();
    EnemyManager& m_enemyManager;
    unsigned int m_currentLevel;
  };

};
