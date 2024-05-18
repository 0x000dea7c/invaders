#pragma once

#include "event.h"
#include "invaders_input.h"
#include "invaders_math.h"
#include "invaders_missile.h"
#include "invaders_renderer.h"
#include "invaders_resources.h"
#include "invaders_grid.h"

#include <vector>

namespace Game {

  // controls player's movement behaviour
  class MovSpec final {
  public:
    Math::v2 m_dir;
    float m_dirScale; // how much player's input influences its movement
    float m_drag;
    bool m_unit; // when moving diagonally, do you want consistent speed
  };

  class Player final {
  public:
    MovSpec m_ms;
    Math::v3 m_pos;
    Math::v2 m_vel;
    Math::v2 m_size;
    int m_shootcd;
    int m_currcd;
    int m_currlives;
    unsigned int m_points;
    bool m_shooting;
    bool m_destroyed;
  };

  class PlayerManager final {
  public:
    PlayerManager(const Res::ResourceManager& resourceManager,
                  const Input::InputManager& inputManager,
                  MissileManager& missileManager,
                  GridManager& gridManager,
                  Ev::EventManager& eventManager);
    ~PlayerManager();
    void update(const float delta, const int rightLimit, const int topLimit);
    void handleInput();
    inline auto currlives() const noexcept { return m_player.m_currlives; };
    void destroyPlayer() noexcept;
    inline void resetPos() noexcept
    {
      m_player.m_pos.x = m_player.m_pos.y = m_player.m_vel.x = m_player.m_vel.y = 0.0f;
    }
    void reset();
    void increasePoints(const unsigned int pts) noexcept;
    inline auto getPlayerPoints() const noexcept { return m_player.m_points; }
  private:
    void updatePlayerData(const float delta, const int rightLimit, const int topLimit);
    void updatePlayerInstanceData();
    void updatePlayerLivesInstanceData();
    Renderer::InstanceData m_playerInstanceData;
    std::vector<Renderer::InstanceData> m_playerLivesInstanceData;
    Player m_player;
    const Res::ResourceManager& m_resourceManager;
    const Input::InputManager& m_inputManager;
    MissileManager& m_missileManager;
    GridManager& m_gridManager;
    Ev::EventManager& m_eventManager;
  };

};
