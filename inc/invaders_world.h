#pragma once

#include <unordered_map>
#include <set>

namespace Game {

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

};
