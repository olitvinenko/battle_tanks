#pragma once

#include <iostream>
#include <fstream>

#include <cmath>
#include <cassert>
#include <cstdlib>
#include <limits>
#include "utils.h"

#define PI		 3.141593f
#define PI2		 6.283185f
#define PI4		 0.785398f
#define EPSILONE 1e-7

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

	friend Vector2 operator*(float lhs, const Vector2 &rhs)
	{
		Vector2 result(rhs);
		result *= lhs;
		return result;
	}

	float operator * (const Vector2 &v) const
		{
			return x * v.x + y * v.y;  // dot product
		}

	bool operator == (const Vector2 &v) const
		{
			return isEqual(x, v.x) && isEqual(y, v.y);
		}

	bool operator != (const Vector2 &v) const
		{
			return v.x != x || v.y != y;
		}

	float sqr() const
		{
			return x * x + y * y;
		}

	float length() const
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

	void normalize()
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
		}

	Vector2 normalizeCopy() const
		{
			Vector2 result = *this;
			result.normalize();
			return result;
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

	void truncate(float max)
	{
		if (length() > max)
		{
			normalize();
			*this *= max;
		}
	}

	void reflect(const Vector2& norm)
	{
		*this += 2.0f * Dot(norm) * norm.GetReverse();
	}

	Vector2 GetReverse() const
		{
			return Vector2(-this->x, -this->y);
		}

	float Dot(const Vector2 &v2) const
		{
			return x*v2.x + y*v2.y;
		}

	void Reflect(const Vector2& norm)
	{
		*this += 2.0f * Dot(norm) * norm.GetReverse();
	}

	//  calculates the euclidean distance between two vectors
	float Distance(const Vector2 &v2)const
	{
		float ySeparation = v2.y - y;
		float xSeparation = v2.x - x;

		return sqrt(ySeparation*ySeparation + xSeparation*xSeparation);
	}

	//  calculates the euclidean distance squared between two vectors 
	float DistanceSq(const Vector2 &v2)const
	{
		float ySeparation = v2.y - y;
		float xSeparation = v2.x - x;

		return ySeparation*ySeparation + xSeparation*xSeparation;
	}

	float LengthSq() const
		{
			return (x * x + y * y);
		}

	//  returns positive if v2 is clockwise of this vector,
	//  minus if anticlockwise (Y axis pointing down, X axis to right)
	enum { clockwise = 1, anticlockwise = -1 };
	int Sign(const Vector2& v2)const
		{
			if (y*v2.x > x*v2.y)
			{
				return anticlockwise;
			}
			else
			{
				return clockwise;
			}
		}

	Vector2 Perp() const
		{
			return Vector2(-y, x);
		}

	friend std::ostream& operator<<(std::ostream& os, const Vector2& rhs)
	{
		os << " " << rhs.x << " " << rhs.y;
		return os;
	}

	friend std::ifstream& operator >> (std::ifstream& is, Vector2& lhs)
	{
		is >> lhs.x >> lhs.y;
		return is;
	}

	friend std::ofstream& operator << (std::ofstream& is, Vector2& lhs)
	{
		is << lhs.x << " " << lhs.y;
		return is;
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
		return a.Dot(b);
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

	inline float Vec2DDistance(const Vector2 &v1, const Vector2 &v2)
	{
		float ySeparation = v2.y - v1.y;
		float xSeparation = v2.x - v1.x;

		return sqrt(ySeparation*ySeparation + xSeparation*xSeparation);
	}

	inline float Vec2DDistanceSq(const Vector2 &v1, const Vector2 &v2)	{

		float ySeparation = v2.y - v1.y;
		float xSeparation = v2.x - v1.x;

		return ySeparation*ySeparation + xSeparation*xSeparation;
	}

	inline float Vec2DLength(const Vector2& v)
	{
		return sqrt(v.x*v.x + v.y*v.y);
	}

	inline float Vec2DLengthSq(const Vector2& v)
	{
		return (v.x*v.x + v.y*v.y);
	}

	inline Vector2 Vec2DNormalize(const Vector2 &v)
	{
		Vector2 vec = v;

		float vector_length = vec.length();

		if (vector_length > std::numeric_limits<float>::epsilon())
		{
			vec.x /= vector_length;
			vec.y /= vector_length;
		}

		return vec;
	}


	//treats a window as a toroid
	inline void WrapAround(Vector2 &pos, int MaxX, int MaxY)
	{
		if (pos.x > MaxX) { pos.x = 0.0; }

		if (pos.x < 0) { pos.x = (float)MaxX; }

		if (pos.y < 0) { pos.y = (float)MaxY; }

		if (pos.y > MaxY) { pos.y = 0.0; }
	}

	//returns true if the point p is not inside the region defined by top_left
	//and bot_rgt
	inline bool NotInsideRegion(Vector2 p,
		Vector2 top_left,
		Vector2 bot_rgt)
	{
		return (p.x < top_left.x) || (p.x > bot_rgt.x) ||
			(p.y < top_left.y) || (p.y > bot_rgt.y);
	}

	inline bool InsideRegion(Vector2 p,
		Vector2 top_left,
		Vector2 bot_rgt)
	{
		return !((p.x < top_left.x) || (p.x > bot_rgt.x) ||
			(p.y < top_left.y) || (p.y > bot_rgt.y));
	}

	inline bool InsideRegion(Vector2 p, int left, int top, int right, int bottom)
	{
		return !((p.x < left) || (p.x > right) || (p.y < top) || (p.y > bottom));
	}

	//  returns true if the target position is in the field of view of the entity
	//  positioned at posFirst facing in facingFirst
	inline bool isSecondInFOVOfFirst(Vector2 posFirst,
		Vector2 facingFirst,
		Vector2 posSecond,
		float    fov)
	{
		Vector2 direction = posSecond.operator-(posFirst);
		Vector2 toTarget = Vec2DNormalize(direction);

		return facingFirst.Dot(toTarget) >= cos(fov / 2.0);
	}