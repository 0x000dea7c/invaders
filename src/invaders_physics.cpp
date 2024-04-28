#include "invaders_physics.h"

namespace Game {

    // assume: vel only moves in positive y direction and it's passed as the movement
    // per frame
    bool aabb_aabb_test(const AABB a, const AABB b, const v2 vel, const i32 steps)
    {
        AABB swept = a;

        const f32 delta_vel = vel.y / steps;

        // don't know, really... it just doesn't work correctly sometimes, still experiencing tunneling...
        for(i32 i = 0; i < steps; ++i) {
            if(swept.min.x <= b.max.x && swept.max.x >= b.min.x &&
               swept.min.y <= b.max.y && swept.max.y >= b.min.y) {
                return true;
            }
            swept.min.y += delta_vel;
            swept.max.y += delta_vel;
        }

        return false;
    }

};
