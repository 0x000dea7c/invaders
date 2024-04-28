#include "invaders.h"
#include "invaders_world.h"

#include <cstdlib>
#include <cmath>

#define ALIEN_YELLOW 0
#define ALIEN_BEIGE  1
#define ALIEN_GREEN  2
#define ALIEN_PINK   3
#define ALIEN_BLUE   4

#include <cassert>

namespace Game {

  static std::pair<f32, f32> get_alien_tex_dims(const i32 type)
  {
    // hardcoded values bc each tex dim is different, :(
    switch(type) {
    case ALIEN_YELLOW: {
      return {125.f, 108.f};
    } break;
    case ALIEN_BEIGE: {
      return {125.f, 122.f};
    } break;
    case ALIEN_GREEN: {
      return {125.f, 123.f};
    } break;
    case ALIEN_PINK: {
      return {125.f, 127.f};
    } break;
    case ALIEN_BLUE: {
      return {125.f, 144.f};
    } break;
    default: {
      assert(false && "get_alien_tex_dims: Invalid alien type!");
    } break;
    }
  }

  static std::pair<i32, i32> get_alien_atlas_idx(const i32 type)
  {
    switch(type) {
    case ALIEN_YELLOW: {
      return {0, 0};
    } break;
    case ALIEN_BEIGE: {
      return {1, 0};
    } break;
    case ALIEN_GREEN: {
      return {2, 0};
    } break;
    case ALIEN_PINK: {
      return {3, 0};
    } break;
    case ALIEN_BLUE: {
      return {4, 0};
    } break;
    default: {
      assert(false && "get_alien_atlas_idx: Invalid alien type!");
    } break;
    }
  }

  static i32 get_alien_fire_cd(const i32 type)
  {
    switch(type) {
    case ALIEN_YELLOW: {
      return (rand() % 500) + 100;
    } break;
    case ALIEN_BEIGE: {
      return (rand() % 500) + 100;
    } break;
    case ALIEN_GREEN: {
      return (rand() % 200) + 50;
    } break;
    case ALIEN_PINK: {
      return (rand() % 100) + 50;
    } break;
    case ALIEN_BLUE: {
      return (rand() % 100) + 50;
    } break;
    default: {
      assert(false && "get_alien_fire_cd: Invalid alien type!");
    } break;
    }
  }

  void load_levels(Invaders& g)
  {
    constexpr i32 first[4][4] {
      { ALIEN_BEIGE,  ALIEN_BEIGE,  ALIEN_BEIGE,  ALIEN_BEIGE },
      { ALIEN_BEIGE,  ALIEN_BEIGE,  ALIEN_BEIGE,  ALIEN_BEIGE },
      { ALIEN_YELLOW, ALIEN_YELLOW, ALIEN_YELLOW, ALIEN_YELLOW },
      { ALIEN_YELLOW, ALIEN_YELLOW, ALIEN_YELLOW, ALIEN_YELLOW }
    };

    constexpr i32 second[4][4] {
      { ALIEN_BLUE,   ALIEN_BLUE,   ALIEN_BLUE,   ALIEN_BLUE },
      { ALIEN_YELLOW, ALIEN_YELLOW, ALIEN_YELLOW, ALIEN_YELLOW },
      { ALIEN_YELLOW, ALIEN_BEIGE,  ALIEN_YELLOW, ALIEN_BEIGE },
      { ALIEN_BEIGE,  ALIEN_BEIGE,  ALIEN_BEIGE,  ALIEN_BEIGE },
    };

    constexpr i32 third[4][4] {
      { ALIEN_PINK,   ALIEN_PINK,   ALIEN_PINK,   ALIEN_PINK },
      { ALIEN_BLUE,   ALIEN_BLUE,   ALIEN_BLUE,   ALIEN_BLUE },
      { ALIEN_BEIGE,  ALIEN_BEIGE,  ALIEN_BEIGE,  ALIEN_BEIGE },
      { ALIEN_YELLOW, ALIEN_YELLOW, ALIEN_YELLOW, ALIEN_YELLOW },
    };

    constexpr i32 fourth[4][4] {
      { ALIEN_PINK,   ALIEN_PINK,   ALIEN_PINK,   ALIEN_PINK },
      { ALIEN_BLUE,   ALIEN_BLUE,   ALIEN_BLUE,   ALIEN_BLUE },
      { ALIEN_GREEN,  ALIEN_GREEN,  ALIEN_GREEN,  ALIEN_GREEN },
      { ALIEN_YELLOW, ALIEN_YELLOW, ALIEN_YELLOW, ALIEN_YELLOW },
    };

    const i32 (*levels[])[4][4] {
      &first,
      &second,
      &third,
      &fourth
    };

    static constexpr f32 spacingX{ 0.1f };
    static constexpr f32 spacingY{ 2.f };

    // yep, by hand, can't be bothered
    for(u32 i{ 0 }; i < 4; ++i) {
      for(u32 j{ 0 }; j < 4; ++j) {
        const auto alien_type = (*levels[g.current_level])[i][j];
        const auto alien_tex_dim = get_alien_tex_dims(alien_type);
        const auto alien_atlas_idx = get_alien_atlas_idx(alien_type);
        const auto pos_x = 8.f + spacingX * j * 30;
        const auto pos_y = 17.f - i * spacingY;
        const auto cd = get_alien_fire_cd(alien_type);

        g.aliens.data.emplace_back(Alien{
            .pos    = v3{ pos_x, pos_y, 0.f },
            .size   = v2{ .x = alien_tex_dim.first * 0.4f / Meters_per_pixel, .y = alien_tex_dim.second * 0.4f / Meters_per_pixel },
            .vel    = v2{ 2.f, 0.f },
            .width  = alien_tex_dim.first,
            .height = alien_tex_dim.second,
            .max_x  = pos_x + 5.f,
            .min_x  = pos_x - 5.f,
            .dir    = -1,
            .idx_x  = alien_atlas_idx.first,
            .idx_y  = alien_atlas_idx.second,
            .firecd = cd
          });

        g.aliens.instance_data.emplace_back(Instance_Data{});
      }
    }
  }

  void init_grid(Grid& gr,
                 const i32 scene_width,
                 const i32 scene_height)
  {
    gr.cell_size = v2{ .x = 220.f, .y = 160.f };
    gr.rows = std::ceil(scene_width  / gr.cell_size.x);
    gr.cols = std::ceil(scene_height / gr.cell_size.y);
  }

  inline i32 get_bucket_id(Grid& gr, v2 pos)
  {
    return std::floor(pos.x / gr.cell_size.x) + std::floor(pos.y / gr.cell_size.y) * gr.cols;
  }

  void get_nearby_ent_type(Grid& gr,
                           const v2 pos,
                           Entity_Type type,
                           std::set<Entity_Grid_Data, Entity_Grid_Data_Comp>& out)
  {
    // NOTE: assuming all entities are aliens, if not, pass type. Also assuming that the entity
    // associated with POS isn't in the grid itself, i.e missiles

    // top left
    auto id = get_bucket_id(gr, v2{ pos.x - gr.cell_size.x, pos.y + gr.cell_size.y });
    for(const auto& e : gr.buckets[id]) {
      if(e.type == type) {
        out.emplace(e);
      }
    }

    // top
    id = get_bucket_id(gr, v2{ pos.x, pos.y + gr.cell_size.y });
    for(const auto& e : gr.buckets[id]) {
      if(e.type == type) {
        out.emplace(e);
      }
    }

    // top right
    id = get_bucket_id(gr, v2{ pos.x + gr.cell_size.x, pos.y + gr.cell_size.y });
    for(const auto& e : gr.buckets[id]) {
      if(e.type == type) {
        out.emplace(e);
      }
    }

    // left
    id = get_bucket_id(gr, v2{ pos.x - gr.cell_size.x, pos.y });
    for(const auto& e : gr.buckets[id]) {
      if(e.type == type) {
        out.emplace(e);
      }
    }

    // right
    id = get_bucket_id(gr, v2{ pos.x + gr.cell_size.x, pos.y });
    for(const auto& e : gr.buckets[id]) {
      if(e.type == type) {
        out.emplace(e);
      }
    }

    // bottom left
    id = get_bucket_id(gr, v2{ pos.x - gr.cell_size.x, pos.y - gr.cell_size.y });
    for(const auto& e : gr.buckets[id]) {
      if(e.type == type) {
        out.emplace(e);
      }
    }

    // bottom
    id = get_bucket_id(gr, v2{ pos.x, pos.y - gr.cell_size.y });
    for(const auto& e : gr.buckets[id]) {
      if(e.type == type) {
        out.emplace(e);
      }
    }

    // bottom right
    id = get_bucket_id(gr, v2{ pos.x + gr.cell_size.x, pos.y - gr.cell_size.y });
    for(const auto& e : gr.buckets[id]) {
      if(e.type == type) {
        out.emplace(e);
      }
    }
  }

  static void store_on_grid(Grid& gr,
                            v3 pos,
                            v2 size,
                            Entity_Type type,
                            const void* entity)
  {
    const v2 min{
      .x = pos.x,
      .y = pos.y
    };

    const v2 max{
      .x = pos.x + size.x,
      .y = pos.y + size.y
    };

    for(u32 i = std::floor(min.x); i <= std::ceil(max.x); ++i) {
      for(u32 j = std::floor(min.y); j <= std::ceil(max.y); ++j) {
        const auto id = get_bucket_id(gr, v2{ (f32)i, (f32)j });
        gr.buckets[id].emplace(Entity_Grid_Data{ entity, type });
      }
    }
  }

  void update_grid(Grid& gr, const Aliens& a, const Player& p)
  {
    gr.buckets.clear();

    for(const auto& alien : a.data) {
      store_on_grid(gr,
                    scale(alien.pos, Meters_per_pixel),
                    scale(alien.size, Meters_per_pixel),
                    Entity_Type::ALIEN,
                    &alien);
    }

    store_on_grid(gr,
                  scale(p.pos, Meters_per_pixel),
                  scale(p.size, Meters_per_pixel),
                  Entity_Type::PLAYER,
                  &p);
  }

};

