#include "invaders_explosion.h"
#include "common.h"
#include "invaders_opengl.h"
#include "invaders_resources.h"

#include <iostream>

namespace Game {
  using namespace Math;
  using namespace Renderer;
  using namespace Ev;

  ExplosionManager::ExplosionManager(const Res::ResourceManager& resourceManager,
				     EventManager& eventManager)
    : m_resourceManager{ resourceManager },
      m_eventManager{ eventManager }
  {
    // aliens and player
    m_explosions.reserve(MAX_ALIENS_ALIVE + 1);
    m_explosionsInstanceData.reserve(MAX_ALIENS_ALIVE + 1);
    m_explosionWidth = pixelsToWorld(m_resourceManager.getTex(IDs::SID_TEX_EXPLOSION)->m_width * 0.1f);
    m_eventManager.subscribe(EventType::AlienDestroyed, [this](const Event& ev) {
      auto data = ev.getData();
      if(std::holds_alternative<Alien*>(data)) {
	auto pos = std::get<Alien*>(data)->m_pos;
	spawnExplosion(pos);
      }
    });
    m_eventManager.subscribe(EventType::PlayerDestroyed, [this](const Event& ev) {
      auto data = ev.getData();
      if(std::holds_alternative<Player*>(data)) {
	auto pos = std::get<Player*>(data)->m_pos;
	spawnExplosion(pos);
      }
    });
  }

  ExplosionManager::~ExplosionManager()
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
        continue;
      }
      m_explosionsInstanceData[i].m_colour.w -= 1.0f * delta;
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
                .x = worldToPixels(m_explosions[i].m_size.x),
                .y = worldToPixels(m_explosions[i].m_size.y),
                .z = 1.0f
              });
      m_explosionsInstanceData[i].m_instanceData.m_model =
        translate(m_explosionsInstanceData[i].m_instanceData.m_model,
                  v3{
                    .x = worldToPixels(m_explosions[i].m_pos.x),
                    .y = worldToPixels(m_explosions[i].m_pos.y),
                    .z = 0.0f
                  });
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_resourceManager.getShader(IDs::SID_SHADER_EXPLOSION)->m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(ExplosionInstanceData) * m_explosionsInstanceData.size(), m_explosionsInstanceData.data());
  }

  void ExplosionManager::spawnExplosion(const v3& refPos)
  {
    m_explosions.emplace_back(Explosion
    {
      .m_pos    = refPos,
      .m_size   = v2{ static_cast<float>(m_explosionWidth),
		      static_cast<float>(m_explosionWidth) },
      .m_life   = 1.0f,
    });
    m_explosionsInstanceData.emplace_back(ExplosionInstanceData{});
  }

  void ExplosionManager::reset()
  {
    m_explosions.clear();
    m_explosionsInstanceData.clear();
  }
};
