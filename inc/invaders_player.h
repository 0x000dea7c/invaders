#pragma once

#include "invaders_math.h"
#include "invaders_renderer.h"

#include <vector>

namespace Game {

  // controls player's movement behavior
  struct MovSpec {
    Math::v2 m_dir;
    float m_dirScale; // how much player's input influences its movement
    float m_drag;
    bool m_unit; // when moving diagonally, do you want consistent speed
  };

  struct Player {
    MovSpec m_ms;
    Math::v3 m_pos;
    Math::v2 m_vel;
    Math::v2 m_size;
    int m_shootcd;
    int m_currcd;
    int m_currlives;
    bool m_shooting;
    bool m_destroyed;
  };

  class PlayerManager {
  public:
    PlayerManager();
    ~PlayerManager();
    void init();
    void close();
    void handleInput();
    void update(const float delta, const int rightLimit, const int topLimit);
    inline auto currlives() const { return m_player.m_currlives; };
  private:
    void updatePlayerLivesInstanceData();
    Renderer::InstanceData m_playerInstanceData;
    std::vector<Renderer::InstanceData> m_playerLivesInstanceData;
    Player m_player;
  };

};
