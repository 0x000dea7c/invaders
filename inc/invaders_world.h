#pragma once

#include "invaders_types.h"
#include "invaders_math.h"

#include <unordered_map>
#include <set>

namespace Game {

    struct Invaders;
    struct Aliens;
    struct Player;

    enum class Entity_Type { ALIEN, PLAYER };

    struct Entity_Grid_Data final {
        const void* data;
        Entity_Type type;
    };

    struct Entity_Grid_Data_Comp final {
        inline bool operator()(const Entity_Grid_Data& a, const Entity_Grid_Data& b) const
        {
            if(a.data != b.data) {
                return a.data < b.data;
            }
            return a.type < b.type;
        }
    };

    struct Grid final {
        v2 cell_size{ 0.f, 0.f };
        u32 rows{ 0 };
        u32 cols{ 0 };
        std::unordered_map<i32, std::set<Entity_Grid_Data, Entity_Grid_Data_Comp>> buckets;
    };

    void load_levels(Invaders& g);
    void init_grid(Grid& gr, const i32 scene_width, const i32 scene_height);
    void get_nearby_ent_type(Grid& gr, const v2 pos, Entity_Type type, std::set<Entity_Grid_Data, Entity_Grid_Data_Comp>& out);
    void update_grid(Grid& gr, const Aliens& a, const Player& p);
};
