#include "invaders_physics.h"

namespace Game {

    // assume: ray only moves in positive y direction, direction cannot be 0
    // bool ray_aabb_test(const Ray a, const AABB b)
    // {
    //     // check if ray's origin is "inside" aabb
    //     if(!(a.origin.x >= b.min.x && a.origin.x <= b.max.x)) {
    //         return false;
    //     }

    //     const f32 t_min = ((b.min.y - a.origin.y) / a.dir.y);

    //     // hack of mine
    //     const f32 p = a.origin.y + t_min * a.dir.y;

    //     // this has to happen within this time frame
    //     return a.origin.y >= p;
    // }
    //         (a.origin.y + a.dir.y >= sweptB.min.y && a.origin.y <= sweptB.max.y);

    // assume: ray only moves in positive y direction, direction cannot be 0
    bool aabb_aabb_test(const AABB& a, const AABB& b)
    {
        // Check for overlap in the X dimension
        if (a.max.x < b.min.x || a.min.x > b.max.x) return false;

        // Check for overlap in the Y dimension
        if (a.max.y < b.min.y || a.min.y > b.max.y) return false;

        // If necessary, also check the Z dimension for 3D cases:
        // if (a.max.z < b.min.z || a.min.z > b.max.z) return false;

        // If no separating axis is found, the AABBs must be intersecting
        return true;
    }

};
