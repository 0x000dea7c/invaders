#pragma once

#include "invaders_math.h"

namespace Phys {

  class AABB final {
  public:
    Math::v2 min;
    Math::v2 max;
  };

  inline bool aabb_aabb_test(const AABB& a, const AABB& b)
  {
    return a.min.x <= b.max.x && a.max.x >= b.min.x && a.min.y <= b.max.y && a.max.y >= b.min.y;
  }

};
