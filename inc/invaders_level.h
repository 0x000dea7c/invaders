#pragma once

#include "invaders_enemy.h"

namespace Game {

  class LevelManager final {
  public:
    explicit LevelManager(EnemyManager& enemyManager);
    ~LevelManager();
    void changeLevel();
    void reset();
    inline auto currentLevel() const noexcept { return m_currentLevel; }
  private:
    void loadLevel();
    AlienType getRandomAlien() const noexcept;
    EnemyManager& m_enemyManager;
    unsigned int m_currentLevel;
  };

};
