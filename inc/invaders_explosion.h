#pragma once

#include "invaders_math.h"
#include "invaders_renderer.h"
#include "invaders_resources.h"

#include <vector>

namespace Game {

  class Explosion final {
  public:
    Math::v3 m_pos;
    Math::v2 m_size;
    float m_life;
  };

  class ExplosionManager final {
  public:
    ExplosionManager(const Res::ResourceManager& resourceManager);
    ~ExplosionManager();
    void update(const float delta);
    void spawnExplosion(const Math::v3& refPos);
    inline auto numActiveExplosions() const { return m_explosions.size(); }
    void reset();
  private:
    std::vector<Explosion> m_explosions;
    std::vector<Renderer::ExplosionInstanceData> m_explosionsInstanceData;
    const Res::ResourceManager& m_resourceManager;
    int m_explosionWidth;
  };

};
