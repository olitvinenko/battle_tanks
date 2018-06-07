#pragma once

#include "Vector2.h"

namespace math
{
	template<typename T>
	class Rect
	{
	public:
		T left;
		T top;
		T right;
		T bottom;

		Rect operator *(float m) const
		{
			return Rect{ left * m, top * m, right * m, bottom * m };
		}
	};

	typedef Rect<int>		RectInt;
	typedef Rect<float>		RectFloat;

	inline float WIDTH(const RectFloat &rect) { return rect.right - rect.left; }
	inline float HEIGHT(const RectFloat &rect) { return rect.bottom - rect.top; }
	inline Vector2 Size(const RectFloat &rect) { return { WIDTH(rect), HEIGHT(rect) }; }
	inline Vector2 Offset(const RectFloat &rect) { return{ rect.left, rect.top }; }
	inline Vector2 Center(const RectFloat &rect) { return Vector2{ rect.left + rect.right, rect.top + rect.bottom} / 2; }

	inline int WIDTH(const RectInt &rect) { return rect.right - rect.left; }
	inline int HEIGHT(const RectInt &rect) { return rect.bottom - rect.top; }

	inline bool PtInFRect(const RectFloat &rect, const Vector2 &pt)
	{
		return rect.left <= pt.x && pt.x < rect.right &&
			rect.top <= pt.y && pt.y < rect.bottom;
	}

	inline bool PtInRect(const RectInt &rect, int x, int y)
	{
		return rect.left <= x && x < rect.right &&
			rect.top <= y && y < rect.bottom;
	}

	inline void RectToFRect(RectFloat *lpfrt, const RectInt *lprt)
	{
		lpfrt->left = static_cast<float>(lprt->left);
		lpfrt->top = static_cast<float>(lprt->top);
		lpfrt->right = static_cast<float>(lprt->right);
		lpfrt->bottom = static_cast<float>(lprt->bottom);
	}

	inline void FRectToRect(RectInt *lprt, const RectFloat *lpfrt)
	{
		lprt->left = static_cast<int>(lpfrt->left);
		lprt->top = static_cast<int>(lpfrt->top);
		lprt->right = static_cast<int>(lpfrt->right);
		lprt->bottom = static_cast<int>(lpfrt->bottom);
	}

	inline void OffsetFRect(RectFloat *lpfrt, float x, float y)
	{
		lpfrt->left += x;
		lpfrt->top += y;
		lpfrt->right += x;
		lpfrt->bottom += y;
	}

	inline void OffsetFRect(RectFloat *lpfrt, const Vector2 &x)
	{
		lpfrt->left += x.x;
		lpfrt->top += x.y;
		lpfrt->right += x.x;
		lpfrt->bottom += x.y;
	}

} // namespace math
