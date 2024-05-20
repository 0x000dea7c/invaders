#pragma once

#include "invaders_math.h"
#include "invaders_missile.h"
#include "invaders_renderer.h"
#include "invaders_resources.h"
#include "invaders_player.h"
#include "event.h"

#include <vector>

namespace Game {

  enum class AlienType : unsigned int {
    YELLOW = 0,
    BEIGE,
    GREEN,
    PINK,
    BLUE,
    COUNT,
    UFO,
    ROSWELL,
  };

  class Alien final {
  public:
    Math::v3 m_pos;
    Math::v2 m_dims;            // needed for atlas query
    Math::v2 m_size;
    Math::v2 m_vel;
    Math::v2i m_idx;            // needed for atlas query
    Math::v2i m_dir;            // movement
    float m_minX;
    float m_maxX;
    float m_initX;
    float m_initY;
    AlienType m_type;
    int m_firecd;
    int m_currcd;
    bool m_destroyed;
  };

  class EnemyManager final {
  public:
    EnemyManager(const Res::ResourceManager& resourceManager,
                 MissileManager& missileManager,
                 GridManager& gridManager,
                 Ev::EventManager& eventManager);
    ~EnemyManager();
    void init();
    void close();
    void update(const float delta);
    inline auto numAliveAliens() const { return m_aliens.size(); }
    void reset();
    void spawnAlien(const Math::v3& pos, const AlienType type, const Math::v2i& dir);
  private:
    void destroyAlien(Alien* a);
    Math::v2 getAlienSize(const AlienType type)    const;
    Math::v2 getAlienVel(const AlienType type)     const;
    int getAtlasIdx(const AlienType type)          const;
    int getAlienFireCd(const AlienType type)       const;
    bool shouldSpawn(const unsigned int chance) const noexcept;
    void spawnUFO();
    void spawnRoswell();
    std::vector<Alien> m_aliens;
    std::vector<Renderer::InstanceData> m_aliensInstanceData;
    const Res::ResourceManager& m_resourceManager;
    MissileManager& m_missileManager;
    GridManager& m_gridManager;
    Ev::EventManager& m_eventManager;
    float m_alienAtlasWidth;
  };

};
