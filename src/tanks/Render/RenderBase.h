#pragma once
#include <cstdint>
#include "Vector2.h"
#include "Rect.h"

struct SpriteColor
{
	union {
		unsigned char rgba[4];
		uint32_t color;
		struct {
			unsigned char r, g, b, a;
		};
	};

	SpriteColor() {}
	SpriteColor(uint32_t c) : color(c) {}
	SpriteColor(uint8_t r_, uint8_t g_, uint8_t b_, uint8_t a_)
		: r(r_)
		, g(g_)
		, b(b_)
		, a(a_)
	{}
};

struct MyLine
{
	Vector2       begin;
	Vector2       end;
	SpriteColor color;
};

struct Texture
{
	union
	{
		unsigned int index;
		void          *ptr;
	};

	bool operator < (const Texture &r) const
	{
		return ptr < r.ptr;
	}
	bool operator == (const Texture &r) const
	{
		return ptr == r.ptr;
	}
};

enum RenderMode
{
	RM_LIGHT = 0,
	RM_WORLD = 1,
	RM_INTERFACE = 2,
	//-------------------
	RM_FORCE32BIT = 0xffffffff
};

struct MyVertex
{                        // offset  size
	float        x, y, z;  //   0      12
	SpriteColor  color;  //  12       4
	float        u, v;    //  16       8
};

///////////////////////////////////////////////////////////////////////////////

class Image
{
public:
	virtual ~Image() {}
	virtual const void* GetData() const = 0;
	virtual unsigned int GetBitsPerPixel() const = 0;
	virtual unsigned int GetWidth() const = 0;
	virtual unsigned int GetHeight() const = 0;
};

///////////////////////////////////////////////////////////////////////////////

struct IRender
{
	virtual ~IRender() {}
	virtual void OnResizeWnd(unsigned int width, unsigned int height) = 0;

	virtual void SetScissor(const math::RectInt *rect) = 0;
	virtual void SetViewport(const math::RectInt *rect) = 0;
	virtual void Camera(const math::RectInt *vp, float x, float y, float scale) = 0;

	virtual void SetAmbient(float ambient) = 0;
	virtual void SetMode(const RenderMode mode) = 0;
	virtual void Begin() = 0;
	virtual void End() = 0;

	//
	// texture management
	//
	virtual bool TexCreate(Texture &tex, const Image &img) = 0;
	virtual void TexFree(Texture tex) = 0;


	//
	// high level primitive drawing
	//
	virtual MyVertex* DrawQuad(Texture tex) = 0;
	virtual MyVertex* DrawFan(unsigned int nEdges) = 0;

	virtual void DrawLines(const MyLine *lines, size_t count) = 0;
};
