#pragma once

#include <algorithm>
#include <cmath>
#include <cassert>
#include <cstdlib>

#include "Vector2.h"

template<typename T>
struct Rect
{
    T left;
    T top;
    T right;
    T bottom;

    Rect operator *(T m) const
    {
        return Rect{ left * m, top * m, right * m, bottom * m };
    }
};

typedef Rect<int>   RectInt;
typedef Rect<float> RectFloat;

inline RectFloat MakeRectRB(Vector2 lt, Vector2 rb) { return RectFloat{ lt.x, lt.y, rb.x, rb.y }; }
inline RectFloat MakeRectWH(Vector2 lt, Vector2 size) { return RectFloat{ lt.x, lt.y, lt.x + size.x, lt.y + size.y }; }
inline RectFloat MakeRectWH(Vector2 size) { return RectFloat{ 0, 0, size.x, size.y }; }
inline RectFloat MakeRectWH(float width, float height) { return RectFloat{ 0, 0, width, height }; }

inline float WIDTH(const RectFloat &rect) { return rect.right - rect.left; }
inline float HEIGHT(const RectFloat &rect) { return rect.bottom - rect.top; }
inline Vector2 Size(const RectFloat &rect) { return { WIDTH(rect), HEIGHT(rect) }; }
inline Vector2 Offset(const RectFloat &rect) { return{ rect.left, rect.top }; }
inline Vector2 Center(const RectFloat &rect) { return Vector2{ rect.left + rect.right, rect.top + rect.bottom} / 2; }

inline int WIDTH(const RectInt &rect) { return rect.right - rect.left; }
inline int HEIGHT(const RectInt &rect) { return rect.bottom - rect.top; }

inline Vector2 Vec2dConstrain(const Vector2 &vec, const RectFloat &rect)
{
    return Vector2{ std::max(rect.left, std::min(vec.x, rect.right)), std::max(rect.top, std::min(vec.y, rect.bottom)) };
}

inline bool PtInFRect(const RectFloat &rect, const Vector2 &pt)
{
    return rect.left <= pt.x && pt.x < rect.right &&
        rect.top <= pt.y && pt.y < rect.bottom;
}

inline bool PtOnFRect(const RectFloat &rect, const Vector2 &pt)
{
    return rect.left <= pt.x && pt.x <= rect.right &&
        rect.top <= pt.y && pt.y <= rect.bottom;
}

inline bool PtInRect(const RectInt &rect, int x, int y)
{
    return rect.left <= x && x < rect.right &&
        rect.top <= y && y < rect.bottom;
}

inline RectFloat RectToFRect(const RectInt &rect)
{
    return RectFloat{
        (float)rect.left,
        (float)rect.top,
        (float)rect.right,
        (float)rect.bottom
    };
}

inline RectInt FRectToRect(const RectFloat &rect)
{
    return RectInt{
        (int)rect.left,
        (int)rect.top,
        (int)rect.right,
        (int)rect.bottom
    };
}

inline RectFloat RectOffset(const RectFloat &rect, Vector2 offset)
{
    return RectFloat{rect.left + offset.x, rect.top + offset.y, rect.right + offset.x, rect.bottom + offset.y};
}
