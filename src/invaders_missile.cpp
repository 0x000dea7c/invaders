#include "invaders_missile.h"
#include "common.h"
#include "invaders_resources.h"
#include "invaders_opengl.h"

extern Res::ResourceManager gResourceManager;

#define MAX_

namespace Game {
  using namespace Math;
  using namespace Renderer;

  MissileManager::MissileManager()
  {

  }

  MissileManager::~MissileManager()
  {

  }

  void MissileManager::init()
  {
    m_playerMissiles.reserve(SIMUL_MISSILES_ALIVE);
    m_playerMissilesInstanceData.reserve(SIMUL_MISSILES_ALIVE);
    m_alienMissiles.reserve(SIMUL_MISSILES_ALIVE);
    m_alienMissilesInstanceData.reserve(SIMUL_MISSILES_ALIVE);
  }

  void MissileManager::close()
  {

  }

  void MissileManager::update(const float delta, const int topLimit)
  {
    //
    // TODO: besides changing the way you obtain nearby entities, this function can be
    // simplified because the logic of player's and alien's missiles is similar.
    //
    // std::set<Entity_Grid_Data, Entity_Grid_Data_Comp> nearby;
    //
    // update player missiles
    //
    for(unsigned int i{ 0 }; i < m_playerMissiles.size(); ++i) {
      m_playerMissiles[i].m_pos.y += m_playerMissiles[i].m_vel.y * delta;
      if(m_playerMissiles[i].m_pos.y >= topLimit) {
        m_playerMissiles[i].m_destroyed = true;
      }
      if(!m_playerMissiles[i].m_destroyed) {
        // get nearby aliens by querying adjacent cells
        // TODO: redo all of this
        // const auto missile_pos_px = v2{
        //   .x = m_playerMissiles[i].m_pos.x,
        //   .y = m_playerMissiles[i].m_pos.y,
        // };
        // const auto missile_AABB = AABB{
        //   .min = v2{ g.player_missiles.data[i].pos.x - g.player_missiles.data[i].size.x,
        //     g.player_missiles.data[i].pos.y - g.player_missiles.data[i].size.y },
        //     .max = v2{ g.player_missiles.data[i].pos.x + g.player_missiles.data[i].size.x,
        //       g.player_missiles.data[i].pos.y + g.player_missiles.data[i].size.y }
        // };
        // tragic
        // get_nearby_ent_type(g.grid, missile_pos_px, Entity_Type::ALIEN, nearby);
        // for(const auto& e : nearby) {
        //   Alien* a = (Alien*)e.data;
        //   // TODO: should you pass the previous position? before applying vel...
        //   const auto alien_AABB = AABB{
        //     .min = v2{ a->pos.x - a->size.x,
        //       a->pos.y - a->size.y },
        //       .max = v2{ a->pos.x + a->size.x,
        //         a->pos.y + a->size.y }
        //   };
        //   if(aabb_aabb_test(missile_AABB,
        //     alien_AABB,
        //     v2{ 0.f, g.player_missiles.data[i].vel.y * dt },
        //     4)) {
        //     g.player_missiles.data[i].destroyed = true;
        //   a->destroyed = true;
        //   spawn_explosion(g.explosions, a->pos);
        //     }
        // }
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
    glBindBuffer(GL_ARRAY_BUFFER, gResourceManager.getShader(IDs::SID_SHADER_MISSILE_PLAYER)->m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    sizeof(InstanceData) * m_playerMissilesInstanceData.size(),
                    m_playerMissilesInstanceData.data());
    //
    // now alien's missiles, almost the same thing
    //
    for(unsigned int i{ 0 }; i < m_alienMissiles.size(); ++i) {
      m_alienMissiles[i].m_pos.y += m_alienMissiles[i].m_vel.y * delta;
      if(m_alienMissiles[i].m_pos.y < 0.f) {
        m_alienMissiles[i].m_destroyed = true;
      }
      if(!m_alienMissiles[i].m_destroyed) {
        // TODO: need to redo this part
        // const v2 missile_pos_px{
        //   .x = g.alien_missiles.data[i].pos.x * Meters_per_pixel,
        //   .y = g.alien_missiles.data[i].pos.y * Meters_per_pixel,
        // };
        // get_nearby_ent_type(g.grid, missile_pos_px, Entity_Type::PLAYER, nearby);
        // if(!nearby.empty()) {
        //   const auto missile_AABB = AABB{
        //     .min = v2{ g.alien_missiles.data[i].pos.x - g.alien_missiles.data[i].size.x,
        //       g.alien_missiles.data[i].pos.y - g.alien_missiles.data[i].size.y },
        //       .max = v2{ g.alien_missiles.data[i].pos.x + g.alien_missiles.data[i].size.x,
        //         g.alien_missiles.data[i].pos.y + g.alien_missiles.data[i].size.y }
        //   };
        //   const auto player_AABB = AABB{
        //     .min = v2{ g.player.pos.x - g.player.size.x, g.player.pos.y - g.player.size.y },
        //     .max = v2{ g.player.pos.x + g.player.size.x, g.player.pos.y + g.player.size.y }
        //   };
        //   if(aabb_aabb_test(missile_AABB,
        //     player_AABB,
        //     v2{ 0.f, g.alien_missiles.data[i].vel.y * dt },
        //     4)) {
        //     g.alien_missiles.data[i].destroyed = true;
        //   g.player.destroyed = true;
        //   spawn_explosion(g.explosions, g.player.pos);
        //     }
        // }
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
    glBindBuffer(GL_ARRAY_BUFFER, gResourceManager.getShader(IDs::SID_SHADER_MISSILE_ALIEN)->m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    sizeof(InstanceData) * m_alienMissilesInstanceData.size(),
                    m_alienMissilesInstanceData.data());
  }

  void MissileManager::spawnAlienMissile(const v3 refPos)
  {
    static const auto missileTex    = gResourceManager.getTex(IDs::SID_TEX_MISSILE_ALIEN);
    static const auto missileWidth  = missileTex->m_width;
    static const auto missileHeight = missileTex->m_height;
    static const auto missileSizeX  = missileWidth * 0.3f;
    static const auto missileSizeY  = missileHeight * 0.3f;
    m_alienMissiles.emplace_back(Missile{
      .m_pos    = refPos,
      .m_size   = v2{ missileSizeX, missileSizeY },
      .m_vel    = v2{ 0.f, -10.0f }, // TODO: put this under params.h?
      .m_width  = static_cast<float>(missileWidth),
      .m_height = static_cast<float>(missileHeight)
    });
    m_alienMissilesInstanceData.emplace_back(InstanceData{});
  }

  void MissileManager::spawnPlayerMissiles(const v3 refPos, const v2 refSize)
  {
    static const auto missileTex    = gResourceManager.getTex(IDs::SID_TEX_MISSILE_PLAYER);
    static const auto missileWidth  = missileTex->m_width;
    static const auto missileHeight = missileTex->m_height;
    static const auto missileSizeX  = missileWidth * 0.3f;
    static const auto missileSizeY  = missileHeight * 0.3f;
    // spawn a pair
    const auto left = v2{ .x = refPos.x + 0.1f, .y = refPos.y + refSize.y };
    const auto right = v2{ .x = refPos.x + refSize.x - 0.1f, .y = refPos.y + refSize.y };
    m_playerMissiles.emplace_back(Missile
    {
      .m_pos    = v3{ left.x , left.y, 0.0f },
      .m_size   = v2{ missileSizeX, missileSizeY },
      .m_vel    = v2{ 0.0f, 12.0f }, // TODO: put this under params.h?
      .m_width  = static_cast<float>(missileWidth),
      .m_height = static_cast<float>(missileHeight)
    });
    m_playerMissiles.emplace_back(Missile
    {
      .m_pos    = v3{ right.x , right.y, 0.0f },
      .m_size   = v2{ missileSizeX, missileSizeY },
      .m_vel    = v2{ 0.0f, 12.0f }, // TODO: put this under params.h?
      .m_width  = static_cast<float>(missileWidth),
      .m_height = static_cast<float>(missileHeight)
    });
    m_playerMissilesInstanceData.emplace_back(InstanceData{});
    m_playerMissilesInstanceData.emplace_back(InstanceData{});
  }
};
