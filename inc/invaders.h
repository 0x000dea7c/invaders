#pragma once

#include "invaders_text_renderer.h"
#include "invaders_types.h"
#include "invaders_math.h"
#include "invaders_world.h"
#include "invaders_text_renderer.h"

#include <unordered_map>
#include <vector>
#include <array>

#define WINDOW_WIDTH 1280
#define WINDOW_HEIGHT 960

// PLATFORM DEPENDENT function declarations
namespace Game {
  // TODO big cleanup needed

  // 1 meter in the game corresponds to ~30.7 pixels
  constexpr f32 Meters_per_pixel{ 50.f };

  void init_OpenGL_fptrs();

  struct Texture_Info final {
    i32 width{ 0 };
    i32 height{ 0 };
    i32 channels{ 0 };
    u32 id{ 0 };
  };

  Texture_Info load_texture(char const* filepath,
                            const bool alpha,
                            const bool flip,
                            const i32 wrap_s,
                            const i32 wrap_t);

  enum class Game_State {
    LOST,
    PAUSED,
    WIN_GAME,
    WIN_LEVEL,
    PLAYING,
    MENU
  };

  enum Key {
    KEY_Q,
    KEY_A,
    KEY_W,
    KEY_S,
    KEY_D,
    KEY_ESCAPE,
    KEY_LEFT,
    KEY_UP,
    KEY_RIGHT,
    KEY_DOWN,
    KEY_SPACE,
    KEY_ENTER,
    KEY_P,

    KEY_UNHANDLED
  };

  struct Instance_Data final {
    m4 model{ identity() };
    std::array<v4, 6> vertex_data;
  };

  struct Explosion_Instance_Data final {
    m4 model{ identity() };
    std::array<v4, 6> vertex_data;
    v4 colour{ 1.f, 0.f, 0.f, 1.f };
  };

  struct Player_Lives_Instance_Data final {
    m4 model;
    std::array<v4, 6> vertex_data;
  };

  struct Explosion final {
    v3 pos{ 0.f, 0.f };
    v2 vel{ 0.f, 0.f };
    v2 size{ 0.f, 0.f };
    f32 life{ 0.f };
  };

  struct Explosions final {
    std::vector<Explosion_Instance_Data> instance_data;
    std::vector<Explosion> data;
  };

  struct Missile final {
    v3 pos{ 0.f, 0.f, 0.f};
    v2 size{ 0.f, 0.f };
    v2 vel{ 0.f, 0.f };
    f32 width{ 0.f };
    f32 height{ 0.f };
    bool destroyed{ false };
  };

  struct Missiles final {
    std::vector<Instance_Data> instance_data;
    std::vector<Missile> data;
  };

  struct Alien final {
    v3 pos{ 0.f, 0.f };
    v2 size{ 0.f, 0.f };
    v2 vel{ 0.f, 0.f };
    f32 width{ 0.f };
    f32 height{ 0.f };
    f32 max_x{ 0.f };
    f32 min_x{ 0.f };
    i32 dir{ 0 };
    i32 idx_x{ 0 };
    i32 idx_y{ 0 };
    i32 firecd{ 50 };
    i32 currcd{ 0 };
    bool destroyed{ false };
  };

  struct Aliens final {
    std::vector<Instance_Data> instance_data;
    std::vector<Alien> data;
  };

  struct Movement_Spec final {
    v3 dir{ 0.f, 0.f, 0.f };
    f32 dirScale{ 80.f };
    f32 drag{ .8f };
    bool unit{ true };
  };

  struct Lives final {
    std::vector<Player_Lives_Instance_Data> instance_data;
    u32 current_lives{ 4 };
  };

  struct Player final {
    m4 model{ identity() };
    Movement_Spec ms;
    Lives lives;
    v3 pos{ 0.f, 0.f, 0.f };
    v3 vel{ 0.f, 0.f, 0.f };
    v2 size{ 0.f, 0.f };
    u32 row{ 0 }, col{ 0 }, end_row{ 0 }, end_col{ 0 };
    i32 shooting_cd{ 20 };
    i32 currcd{ 0 };
    bool shooting{ false };
    bool destroyed{ false };
  };

  struct Invaders final {
    Aliens aliens;
    Player player;
    Missiles player_missiles;
    Missiles alien_missiles;
    Explosions explosions;
    Grid grid;
    Text_Renderer text_renderer;
    std::unordered_map<Key, bool> keys;
    std::unordered_map<Key, bool> prev_keys;
    u32 current_level{ 0 };
    u32 max_levels{ 4 };
    f32 scene_width_px{ 0.f };
    f32 scene_height_px{ 0.f };
    f32 scene_width_m{ 0.f };
    f32 scene_height_m{ 0.f };
    Game_State state{ Game_State::PLAYING };
    bool is_running{ true };
  };

};
