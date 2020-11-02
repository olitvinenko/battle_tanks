#pragma once

#include "math/Rect.h"

class IImage;
struct GlTexture;
struct Line;
struct Vertex;
struct Point;
struct Color;
struct ColoredVertex;

enum RenderMode
{
    UNDEFINED = -1,
	LIGHT = 0,
	WORLD = 1,
	INTERFACE = 2,
};

struct IRender
{
    virtual ~IRender() = default;

    virtual bool Init() = 0;
    virtual void OnResizeWnd(unsigned int width, unsigned int height) = 0;

    virtual void SetScissor(const RectInt *rect) = 0;
    virtual void SetViewport(const RectInt *rect) = 0;
    virtual void Camera(const RectInt *vp, float x, float y, float scale) = 0;

    virtual void SetAmbient(float ambient) = 0;
    virtual void SetMode (const RenderMode mode) = 0;
    
    virtual void Begin() = 0;
    virtual void End() = 0;

    // texture management
    virtual bool TexCreate(GlTexture &tex, const IImage &img, bool magFilter) = 0;
    virtual void TexFree(GlTexture tex) = 0;

    // high level primitive drawing
    virtual Vertex* DrawQuad(GlTexture tex) = 0;
    virtual Vertex* DrawFan(unsigned int nEdges) = 0;

    virtual void DrawTriangles(const ColoredVertex* vertices, std::size_t count) = 0;
    virtual void DrawPoints(const ColoredVertex* points, std::size_t count, float pointSize) = 0;
    virtual void DrawLines(const Line *lines, size_t count) = 0;
};
