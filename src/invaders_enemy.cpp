#include "invaders_enemy.h"
#include "common.h"
#include "invaders_opengl.h"
#include "invaders_resources.h"
#include "invaders_missile.h"

extern Game::MissileManager gMissileManager;
extern Res::ResourceManager gResourceManager;

#define MAX_ALIENS_ALIVE 16

namespace Game {
  using namespace Math;

  EnemyManager::EnemyManager()
  {

  }

  EnemyManager::~EnemyManager()
  {

  }

  void EnemyManager::init()
  {
    m_aliens.reserve(MAX_ALIENS_ALIVE);
    m_aliensInstanceData.reserve(MAX_ALIENS_ALIVE);
    m_alienAtlasWidth = gResourceManager.getTex(IDs::SID_TEX_ALIEN_ATLAS)->m_width;
  }

  void EnemyManager::close()
  {

  }

  void EnemyManager::update(const float delta)
  {
    for(unsigned int i = 0; i < m_aliens.size(); ++i) {
      if(m_aliens[i].m_destroyed) {
        std::swap(m_aliens[i], m_aliens.back());
        std::swap(m_aliensInstanceData[i], m_aliensInstanceData.back());
        m_aliens.pop_back();
        m_aliensInstanceData.pop_back();
        --i;
        continue;
      }
      // check if it's time to spawn missiles
      if(m_aliens[i].m_currcd > m_aliens[i].m_firecd) {
        gMissileManager.spawnAlienMissile(m_aliens[i].m_pos);
        m_aliens[i].m_currcd = 0;
      } else {
        ++m_aliens[i].m_currcd;
      }
      if(m_aliens[i].m_pos.x <= m_aliens[i].m_minX || m_aliens[i].m_pos.x >= m_aliens[i].m_maxX) {
        m_aliens[i].m_dir = -m_aliens[i].m_dir;
        m_aliens[i].m_vel = scale(m_aliens[i].m_vel, m_aliens[i].m_dir);
      }
      m_aliens[i].m_pos.x += m_aliens[i].m_vel.x * delta;
      // transforms
      m_aliensInstanceData[i].m_model = identity();
      m_aliensInstanceData[i].m_model = scale(m_aliensInstanceData[i].m_model, v3{
        .x = m_aliens[i].m_size.x,
        .y = m_aliens[i].m_size.y,
        .z = 1.f
      });
      m_aliensInstanceData[i].m_model = translate(m_aliensInstanceData[i].m_model, v3{
        .x = m_aliens[i].m_pos.x,
        .y = m_aliens[i].m_pos.y,
        .z = 0.f
      });
      // TODO: nasty, there's probably a better way but don't know it yet, this is used to pick the right texture
      // within the atlas
      m_aliensInstanceData[i].m_vertexData = {{
        // bottom left
        { .x = -1.f,
          .y = -1.f,
          .z = (m_aliens[i].m_idxX * m_aliens[i].m_width) / m_alienAtlasWidth,
          .w = ((m_aliens[i].m_idxY + 1) * m_aliens[i].m_height) / m_alienAtlasWidth },
        // bottom right
        { .x =  1.f,
          .y = -1.f,
          .z = ((m_aliens[i].m_idxX + 1) * m_aliens[i].m_width) / m_alienAtlasWidth,
          .w = ((m_aliens[i].m_idxY + 1) * m_aliens[i].m_height) / m_alienAtlasWidth },
        // top left
        { .x = -1.f,
          .y =  1.f,
          .z = (m_aliens[i].m_idxX * m_aliens[i].m_width) / m_alienAtlasWidth,
          .w = (m_aliens[i].m_idxY * m_aliens[i].m_height) / m_alienAtlasWidth },
        // top left
        { .x = -1.f,
          .y =  1.f,
          .z = (m_aliens[i].m_idxX * m_aliens[i].m_width) / m_alienAtlasWidth,
          .w = (m_aliens[i].m_idxY * m_aliens[i].m_height) / m_alienAtlasWidth },
        // top right
        { .x =  1.f,
          .y =  1.f,
          .z = ((m_aliens[i].m_idxX + 1) * m_aliens[i].m_width) / m_alienAtlasWidth,
          .w = (m_aliens[i].m_idxY * m_aliens[i].m_height) / m_alienAtlasWidth },
        // bottom right
        { .x =  1.f,
          .y = -1.f,
          .z = ((m_aliens[i].m_idxX + 1) * m_aliens[i].m_width) / m_alienAtlasWidth,
          .w = ((m_aliens[i].m_idxY + 1) * m_aliens[i].m_height) / m_alienAtlasWidth }
      }};
    }
    // update vbo with new data
    glBindBuffer(GL_ARRAY_BUFFER, gResourceManager.getShader(IDs::SID_SHADER_ALIEN)->m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    sizeof(Renderer::InstanceData) * m_aliensInstanceData.size(),
                    m_aliensInstanceData.data());
  }

};
