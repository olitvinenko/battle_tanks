#pragma once

#include "IRender.h"
#include <glm/mat4x4.hpp> // glm::mat4

struct IRenderPart
{
    virtual void Begin() { }
    virtual void SetMode(RenderMode mode) { }
    
    virtual void Flush(const glm::mat4x4& modelView, const glm::mat4x4& projection) = 0;
};

struct Point;
struct IRenderPoints : public IRenderPart
{
    virtual ~IRenderPoints() = default;
    
    virtual void Draw(Point point, const glm::mat4x4& modelView, const glm::mat4x4& projection) = 0;
};

struct Line;
struct IRenderLines : public IRenderPart
{
    virtual ~IRenderLines() = default;
    
    virtual void Draw(const Line *lines, std::size_t count, const glm::mat4x4& modelView, const glm::mat4x4& projection) = 0;
};

class Vector2;
struct Color;
struct IRenderSolidTriangles : public IRenderPart
{
    virtual ~IRenderSolidTriangles() = default;
    
    virtual void Vertex(const Vector2& v, Color color, const glm::mat4x4& modelView, const glm::mat4x4& projection) = 0;
};

struct GlTexture;
struct Vertex;
struct IRenderTexturedTriangles : public IRenderPart
{
    virtual ~IRenderTexturedTriangles() = default;
    
    virtual Vertex* GetVertices(GlTexture texture, const glm::mat4x4& modelView, const glm::mat4x4& projection) = 0;
};


struct FanVertex;
struct IRenderFan : public IRenderPart
{
    virtual ~IRenderFan() = default;
    
    virtual Vertex* GetVertices(std::size_t nEdges, const glm::mat4x4& modelView, const glm::mat4x4& projection) = 0;
};
