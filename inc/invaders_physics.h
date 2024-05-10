#pragma once

#include "invaders_math.h"

namespace Phys {

  struct AABB final {
    Math::v2 min;
    Math::v2 max;
  };

  bool aabb_aabb_test(const AABB a, const AABB b, const Math::v2 vel, const int steps);

};
