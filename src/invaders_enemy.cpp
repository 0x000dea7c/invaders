#include "invaders_enemy.h"
#include "common.h"
#include "params.h"
#include "invaders_opengl.h"
#include "invaders_physics.h"

namespace Game {
  using namespace Math;
  using namespace Res;
  using namespace Ev;

  EnemyManager::EnemyManager(const ResourceManager& resourceManager,
                             MissileManager& missileManager,
                             GridManager& gridManager,
                             EventManager& eventManager)
    : m_resourceManager{ resourceManager },
      m_missileManager{ missileManager },
      m_gridManager{ gridManager },
      m_eventManager{ eventManager }
  {
    m_aliens.reserve(MAX_ALIENS_ALIVE);
    m_aliensInstanceData.reserve(MAX_ALIENS_ALIVE);
    m_alienAtlasWidth = m_resourceManager.getTex(IDs::SID_TEX_ALIEN_ATLAS)->m_width;
    eventManager.subscribe(EventType::AlienDestroyed, [this](const Event& e){
      auto data = e.getData();
      if(std::holds_alternative<Alien*>(data)) {
	destroyAlien(std::get<Alien*>(data));
      }
    });
  }

  EnemyManager::~EnemyManager()
  {

  }

  void EnemyManager::update(const float delta)
  {
    if(shouldSpawn(10)) {
      spawnUFO();
      m_resourceManager.playAudioTrack(IDs::SID_AUDIO_UFO_SPAWNED, false);
    }
    if(shouldSpawn(10)) {
      spawnRoswell();
      m_resourceManager.playAudioTrack(IDs::SID_AUDIO_ROSWELL_SPAWNED, false);
    }
    for(unsigned long i{ 0 }; i < m_aliens.size(); ++i) {
      if(m_aliens[i].m_destroyed) {
	std::iter_swap(m_aliens.begin() + i, m_aliens.end() - 1);
	m_aliens.pop_back();
	std::iter_swap(m_aliensInstanceData.begin() + i, m_aliensInstanceData.end() - 1);
	m_aliensInstanceData.pop_back();
	continue;
      }
      // this sucks, figure out another way to do this for future games, apparently polymorphism
      // is not an option, but this is terribad
      if(m_aliens[i].m_type == AlienType::UFO) {
	// the UFO only moves from one side of the screen to the other and it doesn't shoot
	m_aliens[i].m_pos.x += m_aliens[i].m_vel.x * delta;
	if(m_aliens[i].m_pos.x < 0 || m_aliens[i].m_pos.x > GAME_WIDTH_UNITS) {
	  m_aliens[i].m_destroyed = true;
	}
      } else if(m_aliens[i].m_type == AlienType::ROSWELL) {
        m_aliens[i].m_pos.x += m_aliens[i].m_vel.x * delta * m_aliens[i].m_dir.x;
        m_aliens[i].m_pos.y += m_aliens[i].m_vel.y * delta * m_aliens[i].m_dir.y;
        if(m_aliens[i].m_pos.y <= 0.0f) {
          m_aliens[i].m_destroyed = true;
        } else {
          const auto nearbyEnts = m_gridManager.getNearby(m_aliens[i].m_pos, EntityType::PLAYER);
          for(const auto& ent : nearbyEnts) {
            Player* p = (Player*)ent.data;
            const auto alienAABB = Phys::AABB{
              .min = v2{ m_aliens[i].m_pos.x - m_aliens[i].m_size.x * 0.5f,
                         m_aliens[i].m_pos.y - m_aliens[i].m_size.y * 0.5f },
              .max = v2{ m_aliens[i].m_pos.x + m_aliens[i].m_size.x * 0.5f,
                         m_aliens[i].m_pos.y + m_aliens[i].m_size.y * 0.5f }
            };
            const auto playerAABB = Phys::AABB{
              .min = v2{ p->m_pos.x - p->m_size.x * 0.5f,
                         p->m_pos.y - p->m_size.y * 0.5f },
              .max = v2{ p->m_pos.x + p->m_size.x * 0.5f,
                         p->m_pos.y + p->m_size.y * 0.5f }
            };
            if(Phys::aabb_aabb_test(alienAABB, playerAABB)) {
              m_aliens[i].m_destroyed = true;
              m_eventManager.post(Event(EventType::PlayerDestroyed, p));
              m_resourceManager.playAudioTrack(IDs::SID_AUDIO_ROSWELL_IMPACT, false);
            }
          }
        }
      } else {
	// check if it's time to spawn missiles
	if(m_aliens[i].m_currcd > m_aliens[i].m_firecd) {
	  m_missileManager.spawnAlienMissile(m_aliens[i].m_pos);
	  m_aliens[i].m_currcd = 0;
	} else {
	  ++m_aliens[i].m_currcd;
	}
	// move
	if(m_aliens[i].m_pos.x <= m_aliens[i].m_minX) {
	  m_aliens[i].m_pos.x = m_aliens[i].m_minX;
	  m_aliens[i].m_dir.x = -m_aliens[i].m_dir.x;
	} else if(m_aliens[i].m_pos.x >= m_aliens[i].m_maxX) {
	  m_aliens[i].m_pos.x = m_aliens[i].m_maxX;
	  m_aliens[i].m_dir.x = -m_aliens[i].m_dir.x;
	}
	// this is why you needed game units!
	m_aliens[i].m_pos.x = m_aliens[i].m_pos.x + m_aliens[i].m_vel.x * m_aliens[i].m_dir.x * delta;
	m_aliens[i].m_pos.y = std::sin(m_aliens[i].m_pos.x * ALIEN_Y_MOV_FREQUENCY) * ALIEN_Y_MOV_AMPLITUDE + m_aliens[i].m_initY;
      }
      // transforms
      if(!m_aliens[i].m_destroyed) {
	m_aliensInstanceData[i].m_model = identity();
	m_aliensInstanceData[i].m_model = scale(m_aliensInstanceData[i].m_model, v3{
	    .x = worldToPixels(m_aliens[i].m_size.x),
	    .y = worldToPixels(m_aliens[i].m_size.y),
	    .z = 1.0f
	  });
	m_aliensInstanceData[i].m_model = translate(m_aliensInstanceData[i].m_model, v3{
	    .x = worldToPixels(m_aliens[i].m_pos.x),
	    .y = worldToPixels(m_aliens[i].m_pos.y),
	    .z = 0.0f
	  });
        if(m_aliens[i].m_type == AlienType::ROSWELL) {
          m_aliensInstaceData[i].m_model = rotate(m_aliensInstaceData[i].m_model, 90.0f, v3i{0, 1, 0});
        }
	// needed to grab the texture from the atlas...
	m_aliensInstanceData[i].m_vertexData = {{
 	    // bottom left
	    { .x = -1.f,
	      .y = -1.f,
	      .z = (m_aliens[i].m_idx.x * m_aliens[i].m_dims.x) / m_alienAtlasWidth,
	      .w = ((m_aliens[i].m_idx.y + 1) * m_aliens[i].m_dims.y) / m_alienAtlasWidth },
	    // bottom right
	    { .x =  1.f,
	      .y = -1.f,
	      .z = ((m_aliens[i].m_idx.x + 1) * m_aliens[i].m_dims.x) / m_alienAtlasWidth,
	      .w = ((m_aliens[i].m_idx.y + 1) * m_aliens[i].m_dims.y) / m_alienAtlasWidth },
	    // top left
	    { .x = -1.f,
	      .y =  1.f,
	      .z = (m_aliens[i].m_idx.x * m_aliens[i].m_dims.x) / m_alienAtlasWidth,
	      .w = (m_aliens[i].m_idx.y * m_aliens[i].m_dims.y) / m_alienAtlasWidth },
	    // top left
	    { .x = -1.f,
	      .y =  1.f,
	      .z = (m_aliens[i].m_idx.x * m_aliens[i].m_dims.x) / m_alienAtlasWidth,
	      .w = (m_aliens[i].m_idx.y * m_aliens[i].m_dims.y) / m_alienAtlasWidth },
	    // top right
	    { .x =  1.f,
	      .y =  1.f,
	      .z = ((m_aliens[i].m_idx.x + 1) * m_aliens[i].m_dims.x) / m_alienAtlasWidth,
	      .w = (m_aliens[i].m_idx.y * m_aliens[i].m_dims.y) / m_alienAtlasWidth },
	    // bottom right
	    { .x =  1.f,
	      .y = -1.f,
	      .z = ((m_aliens[i].m_idx.x + 1) * m_aliens[i].m_dims.x) / m_alienAtlasWidth,
	      .w = ((m_aliens[i].m_idx.y + 1) * m_aliens[i].m_dims.y) / m_alienAtlasWidth }
	  }};
        m_gridManager.update(m_aliens[i].m_pos, &m_aliens[i], EntityType::ALIEN);
      }
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_resourceManager.getShader(IDs::SID_SHADER_ALIEN)->m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER,
                    0,
                    sizeof(Renderer::InstanceData) * m_aliensInstanceData.size(),
                    m_aliensInstanceData.data());
  }

  void EnemyManager::spawnAlien(const v3& pos, const AlienType type)
  {
    const auto alienSize = getAlienSize(type);
    m_aliens.emplace_back(Alien{
      .m_pos       = pos,
      .m_dims      = alienSize,
      .m_size      = { pixelsToWorld(alienSize.x * 0.3f),
		       pixelsToWorld(alienSize.y * 0.3f) },
      .m_vel       = getAlienVel(type),
      .m_idx       = { getAtlasIdx(type), 0 },
      .m_dir       = { -1, -1 },
      .m_minX      = pos.x - 15.0f,
      .m_maxX      = pos.x + 15.0f,
      .m_initX     = pos.x,
      .m_initY     = pos.y,
      .m_type      = type,
      .m_firecd    = getAlienFireCd(type),
      .m_currcd    = 0,
      .m_destroyed = false
    });
    m_aliensInstanceData.emplace_back(Renderer::InstanceData{});
  }

  v2 EnemyManager::getAlienSize(const AlienType type) const
  {
    switch(type) {
    case AlienType::YELLOW: {
      return { 125.0f, 108.0f };
    } break;
    case AlienType::BEIGE: {
      return { 125.0f, 122.0f };
    } break;
    case AlienType::GREEN: {
      return { 125.0f, 123.0f };
    } break;
    case AlienType::PINK: {
      return { 125.0f, 127.0f };
    } break;
    case AlienType::BLUE: {
      return { 125.0f, 144.0f };
    } break;
    case AlienType::UFO: {
      return { 125.0f, 70.0f };
    } break;
    case AlienType::ROSWELL: {
      return { 125.0f, 70.0f };
    } break;
    default: {
      assert(false && "die biatch");
    } break;
    }
  }

  v2 EnemyManager::getAlienVel(const AlienType type) const
  {
    switch(type) {
    case AlienType::ROSWELL:
      return { 25.0f, 25.0f };
    case AlienType::UFO:
      return { 30.0f, 0.0f };
    default:
      return { 15.0f, 0.0f };
    }
  }

  int EnemyManager::getAtlasIdx(const AlienType type) const
  {
    // NOTE: in your atlas there's only a single row, the only thing that changes
    // are columns.
    switch(type) {
    case AlienType::YELLOW: {
      return 0;
    } break;
    case AlienType::BEIGE: {
      return 1;
    } break;
    case AlienType::GREEN: {
      return 2;
    } break;
    case AlienType::PINK: {
      return 3;
    } break;
    case AlienType::BLUE: {
      return 4;
    } break;
    case AlienType::UFO: {
      return 5;
    } break;
    case AlienType::ROSWELL: {
      return 6;
    } break;
    default: {
      assert(false && "Invalid alien type!");
    } break;
    }
  }

  int EnemyManager::getAlienFireCd(const AlienType type) const
  {
    // different aliens have different fire cooldown; blue and pink aliens are supposed
    // to be "challenging".
    switch(type) {
    case AlienType::YELLOW: {
      return (rand() % 2000) + 100;
    } break;
    case AlienType::BEIGE: {
      return (rand() % 2000) + 100;
    } break;
    case AlienType::GREEN: {
      return (rand() % 1500) + 50;
    } break;
    case AlienType::PINK: {
      return (rand() % 1000) + 50;
    } break;
    case AlienType::BLUE: {
      return (rand() % 1000) + 50;
    } break;
    case AlienType::UFO: { // UFOs don't shoot
      return 9999;
    } break;
    case AlienType::ROSWELL: {
      return 9999;
    } break;
    default: {
      assert(false && "Invalid alien type!");
    } break;
    }
  }

  void EnemyManager::destroyAlien(Alien* a)
  {
    a->m_destroyed = true;
  }

  void EnemyManager::reset()
  {
    m_aliens.clear();
    m_aliensInstanceData.clear();
  }

  bool EnemyManager::shouldSpawn(const unsigned int chance) const noexcept
  {
    static unsigned int timer = 0;
    if(timer >= 300) {
      const auto random = std::rand() % chance;
      timer = 0;
      return random == 0;
    }
    ++timer;
    return false;
  }

  void EnemyManager::spawnUFO()
  {
    spawnAlien(v3{ 0.0f, GAME_HEIGHT_UNITS - 5.0f, 0.0f }, AlienType::UFO);
  }

  void EnemyManager::spawnRoswell()
  {
    spawnAlien(v3{ GAME_WIDTH_UNITS - 5.0f, GAME_HEIGHT_UNITS - 5.0f, 0.0f }, AlienType::ROSWELL);
  }
};
