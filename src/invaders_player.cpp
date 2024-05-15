#include "invaders_player.h"
#include "common.h"
#include "invaders_math.h"
#include "invaders_opengl.h"

namespace Game {
  using namespace Math;
  using namespace Renderer;
  using namespace Input;
  using namespace Res;
  using namespace Ev;

  PlayerManager::PlayerManager(const ResourceManager& resourceManager,
                               const InputManager& inputManager,
                               MissileManager& missileManager,
                               GridManager& gridManager,
                               EventManager& eventManager)
    : m_player{ Player{} },
      m_resourceManager{ resourceManager },
      m_inputManager{ inputManager },
      m_missileManager{ missileManager },
      m_gridManager{ gridManager },
      m_eventManager{ eventManager }
  {
    // init player position and other settings
    m_player.m_pos = v3{ 0.0f, 0.0f, 0.0f };
    m_player.m_vel = v2{ 0.0f, 0.0f };
    const auto* playerTex = m_resourceManager.getTex(IDs::SID_TEX_PLAYER);
    m_player.m_size = v2{ static_cast<float>(playerTex->m_width)  * 0.4f,
                          static_cast<float>(playerTex->m_height) * 0.4f };
    m_player.m_shootcd = 30;
    m_player.m_currcd = 0;
    m_player.m_currlives = MAX_PLAYER_LIVES;
    m_player.m_shooting = false;
    m_player.m_destroyed = false;
    m_player.m_points = 0;
    m_playerLivesInstanceData.reserve(MAX_PLAYER_LIVES);
    m_eventManager.subscribe(EventType::PlayerDestroyed, [this](const Event&){
      destroyPlayer();
    });
    updatePlayerLivesInstanceData();
  }

  PlayerManager::~PlayerManager()
  {

  }

  void PlayerManager::update(const float delta, const int rightLimit, const int topLimit)
  {
    handleInput();
    if(m_player.m_shooting && m_player.m_currcd > m_player.m_shootcd) {
      m_missileManager.spawnPlayerMissiles(m_player.m_pos, m_player.m_size);
      m_player.m_currcd = 0;
    } else {
      ++m_player.m_currcd;
    }
    if(m_player.m_destroyed) {
      // reset player's on the screen
      m_player.m_pos.x = 0.0f;
      m_player.m_pos.y = 0.0f;
      m_player.m_destroyed = false;
      --m_player.m_currlives;
      updatePlayerLivesInstanceData();
    } else {
      auto direction = m_player.m_ms.m_dir;
      // normalize direction if required
      if(m_player.m_ms.m_unit) {
        direction = normalize(direction);
      }
      // compute acceleration
      auto acc = scale(direction, m_player.m_ms.m_dirScale);
      // compute drag based on current velocity
      const auto drag = scale(m_player.m_vel, -m_player.m_ms.m_drag);
      acc = add(acc, drag);
      // update velocity by applying acceleration
      m_player.m_vel = add(m_player.m_vel, scale(acc, delta));
      // compute displacement due to new velocity for *this frame
      const auto velocity_dt = scale(m_player.m_vel, delta);
      // additional position adjustment due to acceleration using the kinematic equation: Δx = ½at²
      const auto acc_dt = scale(acc, 0.5f * Math::square(delta));
      // update position by adding both velocity displacement and acceleration displacement
      // clamp pos with screen dimensions
      m_player.m_pos = add(m_player.m_pos, velocity_dt);
      m_player.m_pos = add(m_player.m_pos, acc_dt);
      if(m_player.m_pos.x + m_player.m_size.x >= rightLimit) {
        m_player.m_pos.x = rightLimit - m_player.m_size.x;
      } else if(m_player.m_pos.x - m_player.m_size.x < 0.0f) {
        m_player.m_pos.x = m_player.m_size.x;
      }
      if(m_player.m_pos.y + m_player.m_size.y > topLimit * 0.3f) {
        m_player.m_pos.y = topLimit * 0.3f - m_player.m_size.y;
      } else if(m_player.m_pos.y - m_player.m_size.y < 0.0f) {
        m_player.m_pos.y = m_player.m_size.y;
      }
      // transforms
      m_playerInstanceData.m_vertexData = {{
        { .x = -1.f, .y = -1.f, .z = 0.f, .w = 0.f }, // bottom left
        { .x =  1.f, .y = -1.f, .z = 1.f, .w = 0.f }, // bottom right
        { .x = -1.f, .y =  1.f, .z = 0.f, .w = 1.f }, // top left
        { .x = -1.f, .y =  1.f, .z = 0.f, .w = 1.f }, // top left
        { .x =  1.f, .y =  1.f, .z = 1.f, .w = 1.f }, // top right
        { .x =  1.f, .y = -1.f, .z = 1.f, .w = 0.f }  // bottom right
      }};
      m_playerInstanceData.m_model = identity();
      m_playerInstanceData.m_model = scale(m_playerInstanceData.m_model, v3{
        .x = m_player.m_size.x,
        .y = m_player.m_size.y,
        .z = 1.0f
      });
      m_playerInstanceData.m_model = translate(m_playerInstanceData.m_model, v3{
        .x = m_player.m_pos.x,
        .y = m_player.m_pos.y,
        .z = m_player.m_pos.z
      });
      glBindBuffer(GL_ARRAY_BUFFER, m_resourceManager.getShader(IDs::SID_SHADER_PLAYER)->m_VBO);
      glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(InstanceData), &m_playerInstanceData);
    }
    m_gridManager.update(m_player.m_pos, &m_player, EntityType::PLAYER);
  }

  void PlayerManager::updatePlayerLivesInstanceData()
  {
    m_playerLivesInstanceData.clear();
    for(auto i = 0; i < m_player.m_currlives; ++i) {
      auto model = identity();
      model = scale(model, v3{ 30.f, 30.f, 0.f });
      model = translate(model, v3{ i * 70.f + 40.f, 50.f, 0.f });
      m_playerLivesInstanceData.emplace_back(InstanceData{
        .m_model = model,
        .m_vertexData = {{
          { .x = -1.f, .y = -1.f, .z = 0.f, .w = 0.f }, // bottom left
          { .x =  1.f, .y = -1.f, .z = 1.f, .w = 0.f }, // bottom right
          { .x = -1.f, .y =  1.f, .z = 0.f, .w = 1.f }, // top left
          { .x = -1.f, .y =  1.f, .z = 0.f, .w = 1.f }, // top left
          { .x =  1.f, .y =  1.f, .z = 1.f, .w = 1.f }, // top right
          { .x =  1.f, .y = -1.f, .z = 1.f, .w = 0.f }  // bottom right
        }}
      });
    }
    glBindBuffer(GL_ARRAY_BUFFER, m_resourceManager.getShader(IDs::SID_SHADER_PLAYER_LIVES)->m_VBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(InstanceData) * m_playerLivesInstanceData.size(), m_playerLivesInstanceData.data());
  }

  void PlayerManager::handleInput()
  {
    // init player movement parameters
    m_player.m_ms = MovSpec{
      .m_dir      = v2{ 0.0f, 0.0f },
      .m_dirScale = 10000.0f,
      .m_drag     = 1.5f,
      .m_unit     = true
    };
    if(m_inputManager.isKeyHeld(Input::Key::KEY_A) || m_inputManager.isKeyHeld(Input::Key::KEY_LEFT)) {
      m_player.m_ms.m_dir.x = -1.0f;
    }
    if(m_inputManager.isKeyHeld(Input::Key::KEY_D) || m_inputManager.isKeyHeld(Input::Key::KEY_RIGHT)) {
      m_player.m_ms.m_dir.x =  1.0f;
    }
    if(m_inputManager.isKeyHeld(Input::Key::KEY_W) || m_inputManager.isKeyHeld(Input::Key::KEY_UP)) {
      m_player.m_ms.m_dir.y =  1.0f;
    }
    if(m_inputManager.isKeyHeld(Input::Key::KEY_S) || m_inputManager.isKeyHeld(Input::Key::KEY_DOWN)) {
      m_player.m_ms.m_dir.y = -1.0f;
    }
    if(m_inputManager.isKeyHeld(Input::Key::KEY_SPACE)) {
      m_player.m_shooting = true;
    } else {
      m_player.m_shooting = false;
    }
  }

  void PlayerManager::reset()
  {
    // reset all (game or w/e)
    resetPos();
    m_player.m_currlives = MAX_PLAYER_LIVES;
    m_player.m_destroyed = false;
    m_player.m_shooting = false;
    m_player.m_points = 0;
  }

  void PlayerManager::increasePoints(unsigned int pts)
  {
    m_player.m_points += pts;
  }

};
