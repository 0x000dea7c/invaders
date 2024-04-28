#pragma once

namespace Game {

    struct AABB final {
        v2 min;
        v2 max;
    };

    struct Ray final {
        v2 origin;
        v2 dir;
    };

    bool aabb_aabb_test(const AABB a, const AABB b, const v2 vel, const i32 steps);
};
