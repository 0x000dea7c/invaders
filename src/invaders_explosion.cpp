#include "invaders_explosion.h"
#include "common.h"
#include "invaders_opengl.h"
#include "invaders_resources.h"

extern Res::ResourceManager gResourceManager;

namespace Game {
  using namespace Math;
  using namespace Renderer;

  ExplosionManager::ExplosionManager()
  {

  }

  ExplosionManager::~ExplosionManager()
  {

  }

  void ExplosionManager::init()
  {
    // aliens and player
    m_explosions.reserve(SIMUL_ALIENS_ALIVE + 1);
    m_explosionsInstanceData.reserve(SIMUL_ALIENS_ALIVE + 1);
    m_explosionWidth = gResourceManager.getTex(IDs::SID_TEX_EXPLOSION)->m_width;
  }

  void ExplosionManager::close()
  {

  }

  void ExplosionManager::update(const float delta)
  {
    // the explosion is just a .png whose alpha component will change overtime.
    for(unsigned int i{ 0 }; i < m_explosions.size(); ++i) {
      m_explosions[i].m_life -= delta;
      if(m_explosions[i].m_life <= 0.0f) {
        std::swap(m_explosions[i], m_explosions.back());
        std::swap(m_explosionsInstanceData[i], m_explosionsInstanceData.back());
        m_explosions.pop_back();
        m_explosionsInstanceData.pop_back();
        --i;
        continue;
      }
      m_explosionsInstanceData[i].m_colour.w -= 1.5f * delta;
      m_explosionsInstanceData[i].m_instanceData.m_vertexData = {{
        { .x = -1.0f, .y = -1.0f, .z = 0.0f, .w = 0.0f }, // bottom left
        { .x =  1.0f, .y = -1.0f, .z = 1.0f, .w = 0.0f }, // bottom right
        { .x = -1.0f, .y =  1.0f, .z = 0.0f, .w = 1.0f }, // top left
        { .x = -1.0f, .y =  1.0f, .z = 0.0f, .w = 1.0f }, // top left
        { .x =  1.0f, .y =  1.0f, .z = 1.0f, .w = 1.0f }, // top right
        { .x =  1.0f, .y = -1.0f, .z = 1.0f, .w = 0.0f }  // bottom right
      }};
      m_explosionsInstanceData[i].m_instanceData.m_model = identity();
      m_explosionsInstanceData[i].m_instanceData.m_model =
        scale(m_explosionsInstanceData[i].m_instanceData.m_model,
              v3{
                .x = m_explosions[i].m_size.x,
                .y = m_explosions[i].m_size.y,
                .z = 1.0f
              });
      m_explosionsInstanceData[i].m_instanceData.m_model =
        translate(m_explosionsInstanceData[i].m_instanceData.m_model,
                  v3{
                    .x = m_explosions[i].m_pos.x,
                    .y = m_explosions[i].m_pos.y,
                    .z = 0.0f
                  });
    }
    glBindBuffer(GL_ARRAY_BUFFER, gResourceManager.getShader(IDs::SID_SHADER_EXPLOSION)->m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    sizeof(ExplosionInstanceData) * m_explosionsInstanceData.size(),
                    m_explosionsInstanceData.data());
  }

  void ExplosionManager::spawnExplosion(const Math::v3 refPos)
  {
    m_explosions.emplace_back(Explosion
    {
      .m_pos    = v3{ refPos.x, refPos.y, 0.0f },
      .m_size   = v2{ m_explosionWidth * 0.1f, m_explosionWidth * 0.1f },
      .m_life   = 1.0f,
    });
    m_explosionsInstanceData.emplace_back(ExplosionInstanceData{});
  }

};
