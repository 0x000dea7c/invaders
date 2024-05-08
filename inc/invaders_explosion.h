#pragma once

#include "invaders_math.h"
#include "invaders_renderer.h"

#include <vector>

namespace Game {

  struct Explosion final {
    Math::v3 m_pos;
    Math::v2 m_size;
    float m_life;
  };

  class ExplosionManager final {
  public:
    ExplosionManager();
    ~ExplosionManager();
    void init();
    void close();
    void update(const float delta);
    void spawnExplosion(const Math::v3 refPos);
    inline auto numActiveExplosions() const { return m_explosions.size(); }
  private:
    std::vector<Explosion> m_explosions;
    std::vector<Renderer::ExplosionInstanceData> m_explosionsInstanceData;
    int m_explosionWidth;
  };

};
