#pragma once

#include "invaders_types.h"

#include <initializer_list>
#include <algorithm>
#include <math.h>

namespace Game {

    struct v2 final {
        f32 x{0.f};
        f32 y{0.f};
    };

    struct v3 final {
        f32 x{ 0.f };
        f32 y{ 0.f };
        f32 z{ 0.f };
    };

    struct v4 final {
        f32 x{ 0.f };
        f32 y{ 0.f };
        f32 z{ 0.f };
        f32 w{ 0.f };
    };

    // column major mode accessing
    struct m4 final {
        f32 elements[16];

        m4(std::initializer_list<f32> a)
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

        f32& operator()(i32 row, i32 col)
        {
            return elements[col * 4 + row];
        }

        const f32& operator()(i32 row, i32 col) const
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
    struct Orthographic_args {
        f32 left;
        f32 right;
        f32 bottom;
        f32 top;
        f32 zNear;
        f32 zFar;
    };

    inline m4 orthographic(const Orthographic_args a)
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

        const f32 sq{ sqrtf(res.x * res.x + res.y * res.y) };

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

        const f32 sq{ sqrtf(res.x * res.x + res.y * res.y + res.z * res.z) };

        res.x = res.x / sq;
        res.y = res.y / sq;
        res.z = res.z / sq;

        return res;
    }

    inline v2 scale(const v2& v, const f32 n)
    {
        v2 res {.x = v.x, .y = v.y };

        res.x *= n;
        res.y *= n;

        return res;
    }

    inline v3 scale(const v3& v, const f32 n)
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

    inline f32 square(const f32 v)
    {
        return v * v;
    }
};
