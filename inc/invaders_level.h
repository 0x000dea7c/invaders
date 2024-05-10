#pragma once

#include "invaders_enemy.h"

namespace Game {

  class LevelManager final {
  public:
    LevelManager(EnemyManager& enemyManager);
    ~LevelManager();
    void changeLevel();
  private:
    void loadLevel();
    EnemyManager& m_enemyManager;
    unsigned int m_currentLevel;
  };

};
