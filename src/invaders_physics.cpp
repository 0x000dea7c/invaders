#include "invaders_physics.h"

namespace Phys {
  using namespace Math;

  // assume: vel only moves in positive y direction and it's passed as the movement
  // per frame
  bool aabb_aabb_test(const AABB a, const AABB b, const v2 vel, const int steps)
  {
    auto swept = a;
    const float deltaVel = vel.y / steps;
    // don't know, really... it just doesn't work correctly sometimes, still experiencing tunneling...
    for(int i = 0; i < steps; ++i) {
      if(swept.min.x <= b.max.x && swept.max.x >= b.min.x &&
         swept.min.y <= b.max.y && swept.max.y >= b.min.y) {
        return true;
      }
      swept.min.y += deltaVel;
      swept.max.y += deltaVel;
    }
    return false;
  }

};
