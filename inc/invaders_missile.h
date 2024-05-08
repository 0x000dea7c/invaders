#pragma once

#include "invaders_math.h"
#include "invaders_renderer.h"

#include <vector>

namespace Game {

  struct Missile final {
    Math::v3 m_pos;
    Math::v2 m_size;
    Math::v2 m_vel;
    float m_width;
    float m_height;
    bool m_destroyed;
  };

  class MissileManager final {
  public:
    MissileManager();
    ~MissileManager();
    void init();
    void close();
    void update(const float delta, const int topLimit);
    void spawnAlienMissile(const Math::v3 refPos);
    void spawnPlayerMissiles(const Math::v3 refPos, const Math::v2 refSize);
    inline auto numActivePlayerMissiles() const { return m_playerMissiles.size(); }
    inline auto numActiveAlienMissiles() const { return m_alienMissiles.size(); }
  private:
    // it's convenient storing different kind of missiles in separate vectors
    std::vector<Missile> m_playerMissiles;
    std::vector<Renderer::InstanceData> m_playerMissilesInstanceData;
    std::vector<Missile> m_alienMissiles;
    std::vector<Renderer::InstanceData> m_alienMissilesInstanceData;
  };

};
