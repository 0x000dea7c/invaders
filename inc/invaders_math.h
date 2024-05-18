#pragma once

#include <initializer_list>
#include <algorithm>
#include <math.h>

namespace Math {

  class v2i final {
  public:
    int x{ 0 };
    int y{ 0 };
  };

  class v2 final {
  public:
    float x{ 0.f };
    float y{ 0.f };
  };

  class v3 final {
  public:
    float x{ 0.f };
    float y{ 0.f };
    float z{ 0.f };
  };

  class v4 final {
  public:
    float x{ 0.f };
    float y{ 0.f };
    float z{ 0.f };
    float w{ 0.f };
  };

  // column major mode accessing
  class m4 final {
  public:
    float elements[16];
    m4(std::initializer_list<float> a)
    {
      std::copy(a.begin(), a.end(), elements);
    }
    m4(m4& a)
    {
      std::copy(a.elements, a.elements + 16, elements);
    }
    m4(const m4& a)
    {
      std::copy(a.elements, a.elements + 16, elements);
    }
    float& operator()(int row, int col)
    {
      return elements[col * 4 + row];
    }
    const float& operator()(int row, int col) const
    {
      return elements[col * 4 + row];
    }
    m4& operator=(const m4& a)
    {
      if(this != &a) {
        std::copy(a.elements, a.elements + 16, elements);
      }
      return *this;
    }
  };

  inline m4 identity()
  {
    return m4{
      1.f, 0.f, 0.f, 0.f,
      0.f, 1.f, 0.f, 0.f,
      0.f, 0.f, 1.f, 0.f,
      0.f, 0.f, 0.f, 1.f
    };
  }

  //
  // see glOrtho
  //
  // left, right: coords for the left and right vertical clipping planes
  // bottom, top: coords for the bottom and top horizontal clipping planes
  // zNear, zFar: distances to the nearer and farther clipping planes, negative
  //              if the plane is behind the viewer
  //
  class Orthographic_args {
  public:
    float left;
    float right;
    float bottom;
    float top;
    float zNear;
    float zFar;
  };

  inline m4 orthographic(const Orthographic_args& a)
  {
    m4 m = identity();
    m(0, 0) =   2.f / (a.right - a.left);
    m(1, 1) =   2.f / (a.top - a.bottom);
    m(2, 2) = - 2.f / (a.zFar - a.zNear);
    m(0, 3) = - (a.right + a.left) / (a.right - a.left);
    m(1, 3) = - (a.top + a.bottom) / (a.top - a.bottom);
    m(2, 3) = - (a.zFar + a.zNear) / (a.zFar - a.zNear);
    return m;
  }

  inline m4 scale(const m4& m, const v3& v)
  {
    m4 res = m;
    res(0, 0) = res(0, 0) * v.x;
    res(1, 1) = res(1, 1) * v.y;
    res(2, 2) = res(2, 2) * v.z;
    return res;
  }

  inline m4 translate(const m4& m, const v3& v)
  {
    m4 res = m;
    res(0, 3) += v.x;
    res(1, 3) += v.y;
    res(2, 3) += v.z;
    return res;
  }

  inline m4 translate(const m4& m, const v2& v)
  {
    m4 res = m;
    res(0, 3) += v.x;
    res(1, 3) += v.y;
    return res;
  }

  inline v2 normalize(const v2& v)
  {
    if(v.x == 0 && v.y == 0) {
      return v2{ 0.f, 0.f };
    }
    v2 res{ .x = v.x, .y = v.y };
    const float sq{ sqrtf(res.x * res.x + res.y * res.y) };
    res.x = res.x / sq;
    res.y = res.y / sq;
    return res;
  }

  inline v3 normalize(const v3& v)
  {
    if(v.x == 0 && v.y == 0 && v.z == 0) {
      return v3{ 0.f, 0.f, 0.f };
    }
    v3 res{ .x = v.x, .y = v.y, .z = v.z };
    const float sq{ sqrtf(res.x * res.x + res.y * res.y + res.z * res.z) };
    res.x = res.x / sq;
    res.y = res.y / sq;
    res.z = res.z / sq;
    return res;
  }

  inline v2 scale(const v2& v, const float n)
  {
    v2 res {.x = v.x, .y = v.y };
    res.x *= n;
    res.y *= n;
    return res;
  }

  inline v3 scale(const v3& v, const float n)
  {
    v3 res { .x = v.x, .y = v.y, .z = v.z };
    res.x *= n;
    res.y *= n;
    res.z *= n;
    return res;
  }

  inline v2 add(const v2& vA, const v2& vB)
  {
    v2 res;
    res.x = vA.x + vB.x;
    res.y = vA.y + vB.y;
    return res;
  }

  inline v3 add(const v3& vA, const v3& vB)
  {
    v3 res;
    res.x = vA.x + vB.x;
    res.y = vA.y + vB.y;
    res.z = vA.z + vB.z;
    return res;
  }

  // assume vB's Z is 0
  inline v3 add(const v3& vA, const v2& vB)
  {
    v3 res;
    res.x = vA.x + vB.x;
    res.y = vA.y + vB.y;
    res.z = vA.z;
    return res;
  }

  inline float square(const float v)
  {
    return v * v;
  }
};
