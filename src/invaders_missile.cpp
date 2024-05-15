#include "invaders_missile.h"
#include "common.h"
#include "invaders_grid.h"
#include "invaders_opengl.h"
#include "invaders_physics.h"
#include "invaders_enemy.h"
#include "invaders_player.h"

namespace Game {
  using namespace Math;
  using namespace Renderer;
  using namespace Res;
  using namespace Ev;

  MissileManager::MissileManager(const ResourceManager& resourceManager,
                                 GridManager& gridManager,
                                 ExplosionManager& explosionManager,
                                 EventManager& eventManager)
    : m_resourceManager{ resourceManager },
      m_gridManager{ gridManager },
      m_explosionManager{ explosionManager },
      m_eventManager{ eventManager }
  {
    m_playerMissiles.reserve(SIMUL_MISSILES_ALIVE);
    m_playerMissilesInstanceData.reserve(SIMUL_MISSILES_ALIVE);
    m_alienMissiles.reserve(SIMUL_MISSILES_ALIVE);
    m_alienMissilesInstanceData.reserve(SIMUL_MISSILES_ALIVE);
  }

  MissileManager::~MissileManager()
  {

  }

  void MissileManager::update(const float delta, const int topLimit)
  {
    //
    // update player missiles
    //
    for(unsigned int i{ 0 }; i < m_playerMissiles.size(); ++i) {
      m_playerMissiles[i].m_pos.y += m_playerMissiles[i].m_vel.y * delta;
      if(m_playerMissiles[i].m_pos.y >= topLimit) {
        m_playerMissiles[i].m_destroyed = true;
      }
      if(!m_playerMissiles[i].m_destroyed) {
        const auto missileAABB = Phys::AABB{
          .min = v2{ m_playerMissiles[i].m_pos.x - m_playerMissiles[i].m_size.x * 0.5f,
                     m_playerMissiles[i].m_pos.y - m_playerMissiles[i].m_size.y * 0.5f},
          .max = v2{ m_playerMissiles[i].m_pos.x + m_playerMissiles[i].m_size.x * 0.5f,
                     m_playerMissiles[i].m_pos.y + m_playerMissiles[i].m_size.y * 0.5f}
        };
        const auto nearbyEnts = m_gridManager.getNearby(m_playerMissiles[i].m_pos, EntityType::ALIEN);
        for(const auto& e: nearbyEnts) {
          auto* a = (Alien*)e.data;
          const auto alienAABB = Phys::AABB{
            .min = v2{ a->m_pos.x - a->m_size.x * 0.5f, a->m_pos.y - a->m_size.y * 0.5f },
            .max = v2{ a->m_pos.x + a->m_size.x * 0.5f, a->m_pos.y + a->m_size.y * 0.5f }
          };
          if(Phys::aabb_aabb_test(missileAABB, alienAABB, m_playerMissiles[i].m_vel, 4)) {
            // the reason why you're not using events all over the place (instead of injecting deps) is that you need to
            // figure out a way to pass a variable number of arguments and handle them accordingly. For now, evenst are
            // associated with functions with no args.
            m_eventManager.post(Event(EventType::AlienDestroyed, a));
            m_playerMissiles[i].m_destroyed = true;
            m_explosionManager.spawnExplosion(a->m_pos);
            m_resourceManager.playAudioTrack(IDs::SID_AUDIO_EXPLOSION, false);
          }
        }
      }
      // if the missile collided with an alien, remove it
      if(m_playerMissiles[i].m_destroyed) {
        std::swap(m_playerMissiles[i], m_playerMissiles.back());
        std::swap(m_playerMissilesInstanceData[i], m_playerMissilesInstanceData.back());
        m_playerMissiles.pop_back();
        m_playerMissilesInstanceData.pop_back();
        --i;
        continue;
      }
      // if not, update it
      m_playerMissilesInstanceData[i].m_vertexData = {{
        { .x = -1.f, .y = -1.f, .z = 0.f, .w = 0.f }, // bottom left
        { .x =  1.f, .y = -1.f, .z = 1.f, .w = 0.f }, // bottom right
        { .x = -1.f, .y =  1.f, .z = 0.f, .w = 1.f }, // top left
        { .x = -1.f, .y =  1.f, .z = 0.f, .w = 1.f }, // top left
        { .x =  1.f, .y =  1.f, .z = 1.f, .w = 1.f }, // top right
        { .x =  1.f, .y = -1.f, .z = 1.f, .w = 0.f }  // bottom right
      }};
      m_playerMissilesInstanceData[i].m_model = identity();
      m_playerMissilesInstanceData[i].m_model =
        scale(m_playerMissilesInstanceData[i].m_model,
              v3{
                .x = m_playerMissiles[i].m_size.x,
                .y = m_playerMissiles[i].m_size.y,
                .z = 1.f
              });
      m_playerMissilesInstanceData[i].m_model =
        translate(m_playerMissilesInstanceData[i].m_model,
                  v3{
                    .x = m_playerMissiles[i].m_pos.x,
                    .y = m_playerMissiles[i].m_pos.y,
                    .z = 0.f
                  });
    }
    // update player missile's instance data
    glBindBuffer(GL_ARRAY_BUFFER, m_resourceManager.getShader(IDs::SID_SHADER_MISSILE_PLAYER)->m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    sizeof(InstanceData) * m_playerMissilesInstanceData.size(),
                    m_playerMissilesInstanceData.data());
    //
    // now alien's missiles, almost the same thing
    //
    for(unsigned long i{ 0 }; i < m_alienMissiles.size(); ++i) {
      m_alienMissiles[i].m_pos.y += m_alienMissiles[i].m_vel.y * delta;
      if(m_alienMissiles[i].m_pos.y < 0.0f) {
        m_alienMissiles[i].m_destroyed = true;
      }
      if(!m_alienMissiles[i].m_destroyed) {
        const auto missileAABB = Phys::AABB{
          .min = v2{ m_alienMissiles[i].m_pos.x - m_alienMissiles[i].m_size.x * 0.5f,
                     m_alienMissiles[i].m_pos.y - m_alienMissiles[i].m_size.y * 0.5f },
          .max = v2{ m_alienMissiles[i].m_pos.x + m_alienMissiles[i].m_size.x * 0.5f,
                     m_alienMissiles[i].m_pos.y + m_alienMissiles[i].m_size.y * 0.5f }
        };
        const auto nearbyEnts = m_gridManager.getNearby(m_alienMissiles[i].m_pos, EntityType::PLAYER);
        for(const auto& e: nearbyEnts) {
          auto* p = (Player*)e.data;
          const auto playerAABB = Phys::AABB{
            .min = v2{ p->m_pos.x - p->m_size.x * 0.5f, p->m_pos.y - p->m_size.y * 0.5f },
            .max = v2{ p->m_pos.x + p->m_size.x * 0.5f, p->m_pos.y + p->m_size.y * 0.5f }
          };
          if(Phys::aabb_aabb_test(missileAABB, playerAABB, m_alienMissiles[i].m_vel, 4)) {
            m_eventManager.post(Event(EventType::PlayerDestroyed));
            m_alienMissiles[i].m_destroyed = true;
            m_explosionManager.spawnExplosion(p->m_pos);
            clearMissiles();
            m_resourceManager.playAudioTrack(IDs::SID_AUDIO_PLAYER_DIE, false);
            return;
          }
        }
      }
      if(m_alienMissiles[i].m_destroyed) {
        std::swap(m_alienMissiles[i], m_alienMissiles.back());
        std::swap(m_alienMissilesInstanceData[i], m_alienMissilesInstanceData.back());
        m_alienMissiles.pop_back();
        m_alienMissilesInstanceData.pop_back();
        --i;
        continue;
      }
      m_alienMissilesInstanceData[i].m_vertexData = {{
        { .x = -1.f, .y = -1.f, .z = 0.f, .w = 0.f }, // bottom left
        { .x =  1.f, .y = -1.f, .z = 1.f, .w = 0.f }, // bottom right
        { .x = -1.f, .y =  1.f, .z = 0.f, .w = 1.f }, // top left
        { .x = -1.f, .y =  1.f, .z = 0.f, .w = 1.f},  // top left
        { .x =  1.f, .y =  1.f, .z = 1.f, .w = 1.f }, // top right
        { .x =  1.f, .y = -1.f, .z = 1.f, .w = 0.f }  // bottom right
      }};
      m_alienMissilesInstanceData[i].m_model = identity();
      m_alienMissilesInstanceData[i].m_model =
        scale(m_alienMissilesInstanceData[i].m_model,
              v3{
                .x = m_alienMissiles[i].m_size.x,
                .y = m_alienMissiles[i].m_size.y,
                .z = 1.f
              });
      m_alienMissilesInstanceData[i].m_model =
        translate(m_alienMissilesInstanceData[i].m_model,
                  v3{
                    .x = m_alienMissiles[i].m_pos.x,
                    .y = m_alienMissiles[i].m_pos.y,
                    .z = 0.f
                  });
    }
    // update alien's missiles instance data
    glBindBuffer(GL_ARRAY_BUFFER, m_resourceManager.getShader(IDs::SID_SHADER_MISSILE_ALIEN)->m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    sizeof(InstanceData) * m_alienMissilesInstanceData.size(),
                    m_alienMissilesInstanceData.data());
  }

  void MissileManager::spawnAlienMissile(const v3 refPos)
  {
    static const auto missileTex = m_resourceManager.getTex(IDs::SID_TEX_MISSILE_ALIEN);
    m_alienMissiles.emplace_back(Missile{
      .m_pos    = refPos,
      .m_size   = v2{ missileTex->m_width * 0.3f, missileTex->m_height * 0.3f },
      .m_vel    = v2{ 0.f, -400.0f }, // TODO: put this under params.h?
      .m_width  = static_cast<float>(missileTex->m_height),
      .m_height = static_cast<float>(missileTex->m_height),
      .m_destroyed = false
    });
    m_alienMissilesInstanceData.emplace_back(InstanceData{});
  }

  void MissileManager::spawnPlayerMissiles(const v3 refPos, const v2 refSize)
  {
    static const auto missileTex = m_resourceManager.getTex(IDs::SID_TEX_MISSILE_PLAYER);
    // spawn a pair
    const auto left = v2{ .x = refPos.x + 5.0f, .y = refPos.y + refSize.y };
    const auto right = v2{ .x = refPos.x + refSize.x - 5.0f, .y = refPos.y + refSize.y };
    m_playerMissiles.emplace_back(Missile
    {
      .m_pos    = v3{ left.x , left.y, 0.0f },
      .m_size   = v2{ missileTex->m_width * 0.2f, missileTex->m_height * 0.2f },
      .m_vel    = v2{ 0.0f, 400.0f }, // TODO: put this under params.h?
      .m_width  = static_cast<float>(missileTex->m_width),
      .m_height = static_cast<float>(missileTex->m_height),
      .m_destroyed = false
    });
    m_playerMissiles.emplace_back(Missile
    {
      .m_pos    = v3{ right.x , right.y, 0.0f },
      .m_size   = v2{ missileTex->m_width * 0.2f, missileTex->m_height * 0.2f },
      .m_vel    = v2{ 0.0f, 400.0f }, // TODO: put this under params.h?
      .m_width  = static_cast<float>(missileTex->m_width),
      .m_height = static_cast<float>(missileTex->m_height),
      .m_destroyed = false
    });
    m_playerMissilesInstanceData.emplace_back(InstanceData{});
    m_playerMissilesInstanceData.emplace_back(InstanceData{});
  }

  void MissileManager::clearMissiles()
  {
    m_playerMissiles.clear();
    m_playerMissilesInstanceData.clear();
    m_alienMissiles.clear();
    m_alienMissilesInstanceData.clear();
  }
};
