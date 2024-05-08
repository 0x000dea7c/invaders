#pragma once

#include "invaders_math.h"
#include "invaders_renderer.h"

#include <vector>

namespace Game {

  struct Alien {
    Math::v3 m_pos;
    Math::v2 m_size;
    Math::v2 m_vel;
    float m_width;
    float m_height;
    float m_maxX;
    float m_minX;
    int m_dir;
    int m_idxX;
    int m_idxY;
    int m_firecd;
    int m_currcd;
    bool m_destroyed;
  };

  class EnemyManager {
  public:
    EnemyManager();
    ~EnemyManager();
    void init();
    void close();
    void update(const float delta);
    inline auto numAliveAliens() const { return m_aliens.size(); }
  private:
    std::vector<Alien> m_aliens;
    std::vector<Renderer::InstanceData> m_aliensInstanceData;
    float m_alienAtlasWidth;
  };

};
