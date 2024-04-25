#define ALIEN_YELLOW 0
#define ALIEN_BEIGE  1
#define ALIEN_GREEN  2
#define ALIEN_PINK   3
#define ALIEN_BLUE   4

#include <vector>

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

    static void load_levels(Invaders& g)
    {
        constexpr i32 first[4][4] {
            { ALIEN_BEIGE,  ALIEN_BEIGE,  ALIEN_BEIGE,  ALIEN_BEIGE },
            { ALIEN_BEIGE,  ALIEN_BEIGE,  ALIEN_BEIGE,  ALIEN_BEIGE },
            { ALIEN_YELLOW, ALIEN_YELLOW, ALIEN_YELLOW, ALIEN_YELLOW },
            { ALIEN_YELLOW, ALIEN_YELLOW, ALIEN_YELLOW, ALIEN_YELLOW }
        };

        constexpr i32 second[4][4] {
            { ALIEN_BEIGE,  ALIEN_BEIGE,  ALIEN_BEIGE,  ALIEN_BEIGE },
            { ALIEN_YELLOW, ALIEN_YELLOW, ALIEN_YELLOW, ALIEN_YELLOW },
            { ALIEN_PINK,   ALIEN_PINK,   ALIEN_PINK,   ALIEN_PINK },
            { ALIEN_BLUE,   ALIEN_BLUE,   ALIEN_BLUE,   ALIEN_BLUE }
        };

        const i32 (*levels[])[4][4] {
            &first,
            &second
        };

        static constexpr f32 spacingX{ 0.2f };
        static const f32 initialYOffset{ g.scene_height_m - 1.5f };
        static constexpr f32 spacingY{ 0.05f };

        // TEMP
        for(u32 i{ 0 }; i < 4; ++i) {
            for(u32 j{ 0 }; j < 4; ++j) {
                const auto alien_type = (*levels[g.current_level])[i][j];
                const auto alien_tex_dim = get_alien_tex_dims(alien_type);
                const auto alien_atlas_idx = get_alien_atlas_idx(alien_type);
                const f32 startX{ g.scene_width_m - (alien_tex_dim.first / Meters_per_pixel) * 4 };

                g.aliens.data.emplace_back(Alien{
                        // .pos    = v3{ .x = startX - (j * ((alien_tex_dim.first / Meters_per_pixel) + spacingX)),
                        //               .y = initialYOffset - i * ((alien_tex_dim.second / Meters_per_pixel) + spacingY) },
                        .pos    = v3{ 5.f + spacingX * j * 30, 17.f - i * spacingY * 30 },
                        .size   = v2 { .x = alien_tex_dim.first * 0.4f / Meters_per_pixel, .y = alien_tex_dim.second * 0.4f / Meters_per_pixel },
                        .width  = alien_tex_dim.first,
                        .height = alien_tex_dim.second,
                        .idx_x  = alien_atlas_idx.first,
                        .idx_y  = alien_atlas_idx.second
                    });

                g.aliens.instance_data.emplace_back(Instance_Data{});
            }
        }

        // g.aliens.data.emplace_back(
        //                            Alien{
        //                                .pos    = v3{ 7.f, 14.f, 0.f },
        //                                .size   = v2{ (125.f / Meters_per_pixel) * 0.4f, (150.f / Meters_per_pixel) * 0.4f },
        //                                .width  = 125.f,
        //                                .height = 150.f,
        //                                .idx_x  = 0,
        //                                .idx_y  = 0
        //                            });
        // g.aliens.instance_data.emplace_back(Instance_Data{});
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

    void get_nearby_alive_aliens(Grid& gr,
                                 const v2 pos,
                                 std::set<Entity_Grid_Data, Entity_Grid_Data_Comp>& out)
    {
        // NOTE: assuming all entities are aliens, if not, pass type. Also assuming that the entity
        // associated with POS isn't in the grid itself, i.e missiles

        // top left
        auto id = get_bucket_id(gr, v2{ pos.x - gr.cell_size.x, pos.y + gr.cell_size.y });
        for(const auto& e : gr.buckets[id]) {
            if(e.type == Entity_Type::ALIEN) {
                out.emplace(e);
            }
        }

        // top
        id = get_bucket_id(gr, v2{ pos.x, pos.y + gr.cell_size.y });
        for(const auto& e : gr.buckets[id]) {
            if(e.type == Entity_Type::ALIEN) {
                out.emplace(e);
            }
        }

        // top right
        id = get_bucket_id(gr, v2{ pos.x + gr.cell_size.x, pos.y + gr.cell_size.y });
        for(const auto& e : gr.buckets[id]) {
            if(e.type == Entity_Type::ALIEN) {
                out.emplace(e);
            }
        }

        // left
        id = get_bucket_id(gr, v2{ pos.x - gr.cell_size.x, pos.y });
        for(const auto& e : gr.buckets[id]) {
            if(e.type == Entity_Type::ALIEN) {
                out.emplace(e);
            }
        }

        // right
        id = get_bucket_id(gr, v2{ pos.x + gr.cell_size.x, pos.y });
        for(const auto& e : gr.buckets[id]) {
            if(e.type == Entity_Type::ALIEN) {
                out.emplace(e);
            }
        }

        // bottom left
        id = get_bucket_id(gr, v2{ pos.x - gr.cell_size.x, pos.y - gr.cell_size.y });
        for(const auto& e : gr.buckets[id]) {
            if(e.type == Entity_Type::ALIEN) {
                out.emplace(e);
            }
        }

        // bottom
        id = get_bucket_id(gr, v2{ pos.x, pos.y - gr.cell_size.y });
        for(const auto& e : gr.buckets[id]) {
            if(e.type == Entity_Type::ALIEN) {
                out.emplace(e);
            }
        }

        // bottom right
        id = get_bucket_id(gr, v2{ pos.x + gr.cell_size.x, pos.y - gr.cell_size.y });
        for(const auto& e : gr.buckets[id]) {
            if(e.type == Entity_Type::ALIEN) {
                out.emplace(e);
            }
        }
    }

    // void get_nearby(Grid& gr,
    //                 const v2 pos,
    //                 const void* curr_entity,
    //                 std::set<Entity_Grid_Data, Entity_Grid_Data_Comp>& out)
    // {
    //     // cba
    //     // top left
    //     auto id = get_bucket_id(gr, v2{ pos.x - gr.cell_size.x, pos.y + gr.cell_size.y });
    //     for(const auto& e : gr.buckets[id]) {
    //         if(e.data != curr_entity) {
    //             out.emplace(e);
    //         }
    //     }

    //     // top
    //     id = get_bucket_id(gr, v2{ pos.x, pos.y + gr.cell_size.y });
    //     for(const auto& e : gr.buckets[id]) {
    //         if(e.data != curr_entity) {
    //             out.emplace(e);
    //         }
    //     }

    //     // top right
    //     id = get_bucket_id(gr, v2{ pos.x + gr.cell_size.x, pos.y + gr.cell_size.y });
    //     for(const auto& e : gr.buckets[id]) {
    //         if(e.data != curr_entity) {
    //             out.emplace(e);
    //         }
    //     }

    //     // left
    //     id = get_bucket_id(gr, v2{ pos.x - gr.cell_size.x, pos.y });
    //     for(const auto& e : gr.buckets[id]) {
    //         if(e.data != curr_entity) {
    //             out.emplace(e);
    //         }
    //     }

    //     // right
    //     id = get_bucket_id(gr, v2{ pos.x + gr.cell_size.x, pos.y });
    //     for(const auto& e : gr.buckets[id]) {
    //         if(e.data != curr_entity) {
    //             out.emplace(e);
    //         }
    //     }

    //     // bottom left
    //     id = get_bucket_id(gr, v2{ pos.x - gr.cell_size.x, pos.y - gr.cell_size.y });
    //     for(const auto& e : gr.buckets[id]) {
    //         if(e.data != curr_entity) {
    //             out.emplace(e);
    //         }
    //     }

    //     // bottom
    //     id = get_bucket_id(gr, v2{ pos.x, pos.y - gr.cell_size.y });
    //     for(const auto& e : gr.buckets[id]) {
    //         if(e.data != curr_entity) {
    //             out.emplace(e);
    //         }
    //     }

    //     // bottom right
    //     id = get_bucket_id(gr, v2{ pos.x + gr.cell_size.x, pos.y - gr.cell_size.y });
    //     for(const auto& e : gr.buckets[id]) {
    //         if(e.data != curr_entity) {
    //             out.emplace(e);
    //         }
    //     }
    // }

    static void store_on_grid(Grid& gr,
                              v3 pos,
                              v2 size,
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
                const auto id = get_bucket_id(gr, v2{ i, j });
                gr.buckets[id].emplace(Entity_Grid_Data{ entity });
            }
        }
    }

    void update_grid(Grid& gr, const Aliens& a)
    {
        gr.buckets.clear();

        for(const auto& alien : a.data) {
            store_on_grid(gr,
                          scale(alien.pos, Meters_per_pixel),
                          scale(alien.size, Meters_per_pixel),
                          &alien);
        }
    }

};

