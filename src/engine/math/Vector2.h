#pragma once

#include <algorithm>
#include <cmath>
#include <cassert>
#include <cstdlib>

#define PI    3.141593f
#define PI2   6.283185f
#define PI4   0.785398f

class Vector2
{
public:
    float x;
    float y;
    
    Vector2()
        : x(0.0f)
        , y(0.0f)
    {
    }
    
    Vector2(float _x, float _y)
        : x(_x)
        , y(_y)
    {
    }

    float operator[](unsigned int i) const
    {
        assert(i < 2);
        return (&x)[i];
    }

    Vector2& operator-=(const Vector2 &v)
    {
        x -= v.x;
        y -= v.y;
        return *this;
    }

    Vector2& operator+=(const Vector2 &v)
    {
        x += v.x;
        y += v.y;
        return *this;
    }

    Vector2 operator + (const Vector2 &v) const
    {
        return Vector2{ x + v.x, y + v.y };
    }

    Vector2 operator - (const Vector2 &v) const
    {
        return Vector2{ x - v.x, y - v.y };
    }

    Vector2 operator - () const
    {
        return Vector2{ -x, -y };
    }

    Vector2 operator * (float a) const
    {
        return Vector2{ x * a, y * a };
    }

    Vector2 operator * (const Vector2 &v) const
    {
        return Vector2{ x * v.x, y * v.y };
    }

    Vector2 operator / (float a) const
    {
        return Vector2{ x / a, y / a };
    }

    Vector2 operator / (const Vector2 &v) const
    {
        return Vector2{ x / v.x, y / v.y };
    }

    const Vector2& operator *= (float a)
    {
        x *= a;
        y *= a;
        return *this;
    }

    const Vector2& operator /= (float a)
    {
        x /= a;
        y /= a;
        return *this;
    }
    
    Vector2& operator = (const Vector2& v)
    {
        x = v.x;
        y = v.y;
        return *this;
    }

    friend Vector2 operator * (float a, const Vector2& v)
    {
        return Vector2{ v.x * a, v.y * a };
    }

    bool operator ==(const Vector2 &v) const
    {
        return v.x == x && v.y == y;
    }

    bool operator !=(const Vector2 &v) const
    {
        return v.x != x || v.y != y;
    }

    float sqr() const
    {
        return x*x + y*y;
    }

    float len() const
    {
        return sqrt(x*x + y*y);
    }

    bool IsZero() const
    {
        return x == 0 && y == 0;
    }

    float Angle() const // angle to the X axis
    {
        float a = atan2(y, x);
        return (a < 0) ? (a + PI2) : a;
    }

    Vector2& Normalize()
    {
        float len = sqrt(x*x + y*y);
        if( len < 1e-7 )
        {
            x = 0;
            y = 0;
        }
        else
        {
            x /= len;
            y /= len;
        }
        return *this;
    }
    
    float Distance(const Vector2 &v2)const
    {
        float ySeparation = v2.y - y;
        float xSeparation = v2.x - x;

        return sqrt(ySeparation * ySeparation + xSeparation * xSeparation);
    }

    Vector2 Norm() const
    {
        Vector2 result = *this;
        return result.Normalize();
    }
};

inline float Vec2DDistance(const Vector2 &v1, const Vector2 &v2)
{
    return v1.Distance(v2);
}

inline Vector2 Vec2dFloor(const Vector2 &vec)
{
    return{ std::floor(vec.x), std::floor(vec.y) };
}

inline Vector2 Vec2dFloor(float x, float y)
{
    return{ std::floor(x), std::floor(y) };
}

inline Vector2 Vec2dDirection(float angle)
{
    return{ cosf(angle), sinf(angle) };
}

inline Vector2 Vec2dAddDirection(const Vector2 &a, const Vector2 &b)
{
    assert(std::abs(a.sqr() - 1) < 1e-5);
    assert(std::abs(b.sqr() - 1) < 1e-5);
    return{ a.x*b.x - a.y*b.y, a.y*b.x + a.x*b.y };
}

inline Vector2 Vec2dSubDirection(const Vector2 &a, const Vector2 &b)
{
    assert(std::abs(a.sqr() - 1) < 1e-5);
    assert(std::abs(b.sqr() - 1) < 1e-5);
    return{ a.x*b.x + a.y*b.y, a.y*b.x - a.x*b.y };
}

inline float Vec2dCross(const Vector2 &a, const Vector2 &b)
{
    return a.x*b.y - a.y*b.x;
}

inline float Vec2dDot(const Vector2 &a, const Vector2 &b)
{
    return a.x*b.x + a.y*b.y;
}

inline Vector2 Vec2dMulX(Vector2 v, float m)
{
    return Vector2{ v.x * m, v.y };
}

inline Vector2 Vec2dMulY(Vector2 v, float m)
{
    return Vector2{ v.x, v.y * m };
}

// generates a pseudo random number in range [0, max)
inline float frand(float max)
{
    return (float) rand() / RAND_MAX * max;
}

// generates a pseudo random vector of the specified length
inline Vector2 vrand(float len)
{
    return Vec2dDirection(frand(PI2)) * len;
}
