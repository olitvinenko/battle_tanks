#pragma once

#include <cmath>
#include <cassert>
#include <cstdlib>

#define PI		 3.141593f
#define PI2		 6.283185f
#define PI4		 0.785398f
#define EPSILONE 1e-7

namespace math
{
	class Vector2
	{
	public:
		float x;
		float y;

		Vector2()
			: x(0)
			, y(0)
		{ }

		Vector2(float _x, float _y)
			: x(_x)
			, y(_y)
		{ }

		explicit Vector2(float angle)
			: x(cosf(angle))
			, y(sinf(angle))
		{ }

		Vector2& operator -= (const Vector2 &v)
		{
			x -= v.x;
			y -= v.y;
			return *this;
		}

		Vector2& operator += (const Vector2 &v)
		{
			x += v.x;
			y += v.y;
			return *this;
		}

		Vector2 operator + (const Vector2 &v) const
		{
			return Vector2(x + v.x, y + v.y);
		}

		Vector2 operator - (const Vector2 &v) const
		{
			return Vector2(x - v.x, y - v.y);
		}

		Vector2 operator - () const
		{
			return Vector2(-x, -y);
		}

		Vector2 operator * (float a) const
		{
			return Vector2(x * a, y * a);
		}

		Vector2 operator / (float a) const
		{
			return Vector2(x / a, y / a);
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

		float operator * (const Vector2 &v) const
		{
			return x * v.x + y * v.y;  // dot product
		}

		friend Vector2 operator * (float a, const Vector2 &v)
		{
			return Vector2(v.x * a, v.y * a);
		}

		bool operator == (const Vector2 &v) const
		{
			return v.x == x && v.y == y;
		}

		bool operator != (const Vector2 &v) const
		{
			return v.x != x || v.y != y;
		}

		float sqr() const
		{
			return x * x + y * y;
		}

		float len() const
		{
			return sqrt(x * x + y * y);
		}

		bool isZero() const
		{
			return x == 0 && y == 0;
		}

		float angle() const // angle to the X axis
		{
			float a = atan2(y, x);
			return (a < 0) ? (a + PI2) : a;
		}

		Vector2& normalize()
		{
			float len = sqrt(x * x + y * y);
			if (len < EPSILONE)
			{
				zero();
			}
			else
			{
				x /= len;
				y /= len;
			}
			return *this;
		}

		Vector2 normalizeCopy() const
		{
			Vector2 result = *this;
			return result.normalize();
		}

		const Vector2& set(float _x, float _y)
		{
			x = _x;
			y = _y;
			return *this;
		}

		void zero()
		{
			x = 0;
			y = 0;
		}
	};

	inline Vector2 Vec2dAddDirection(const Vector2 &a, const Vector2 &b)
	{
		assert(std::abs(a.sqr() - 1) < 1e-5);
		assert(std::abs(b.sqr() - 1) < 1e-5);
		return Vector2(a.x*b.x - a.y*b.y, a.y*b.x + a.x*b.y);
	}

	inline Vector2 Vec2dSubDirection(const Vector2 &a, const Vector2 &b)
	{
		assert(std::abs(a.sqr() - 1) < 1e-5);
		assert(std::abs(b.sqr() - 1) < 1e-5);
		return Vector2(a.x*b.x + a.y*b.y, a.y*b.x - a.x*b.y);
	}

	inline float Vec2dCross(const Vector2 &a, const Vector2 &b)
	{
		return a.x*b.y - a.y*b.x;
	}

	inline float Vec2dDot(const Vector2 &a, const Vector2 &b)
	{
		return a.x*b.x + a.y*b.y;
	}

	// generates a pseudo random number in range [0, max)
	inline float frand(float max)
	{
		return static_cast<float>(rand()) / RAND_MAX * max;
	}

	// generates a pseudo random vector of the specified length
	inline Vector2 vrand(float len)
	{
		return Vector2(frand(PI2)) * len;
	}

} // namespace math

using namespace math;