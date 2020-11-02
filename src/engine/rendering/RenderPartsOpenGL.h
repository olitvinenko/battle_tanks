#pragma once

#include "base/IRenderParts.h"

#include "OpenGL.h"

#include "Color.h"
#include "Point.h"
#include "Line.h"
#include "Vertex.h"
#include "ColoredVertex.h"
#include "math/Vector2.h"


class RenderPointsOpenGL : public IRenderPoints
{
public:
    RenderPointsOpenGL(bool colorNormalized);
    ~RenderPointsOpenGL();
    
    void Draw(Point point, const glm::mat4x4& modelView, const glm::mat4x4& projection) override;
    void Flush(const glm::mat4x4& modelView, const glm::mat4x4& projection) override;
private:
    enum { maxVertices = 512 };
    Vector2 m_vertices[maxVertices];
    Color m_colors[maxVertices];
    float32 m_sizes[maxVertices];
    
    int32 m_count = 0;
    
    GLuint m_vaoId = 0;
    GLuint m_vboIds[3] { };
    GLuint m_programId = 0;
    
    GLboolean m_colorNormalized = GL_FALSE;
    
    GLint m_projectionUniform = 0;
    GLint m_modelViewUniform = 0;
    
    GLint m_vertexAttribute = 0;
    GLint m_colorAttribute = 0;
    GLint m_sizeAttribute = 0;
};

//------------------------------------------------------------------------------------------------

class RenderLinesOpenGL : public IRenderLines
{
public:
    RenderLinesOpenGL(bool colorNormalized);
    ~RenderLinesOpenGL();
    
    void Draw(const Line *lines, std::size_t count, const glm::mat4x4& modelView, const glm::mat4x4& projection) override;
    void Flush(const glm::mat4x4& modelView, const glm::mat4x4& projection) override;
private:
    enum { maxVertices = 2 * 512 };
    Vector2 m_vertices[maxVertices];
    Color m_colors[maxVertices];
    
    int32 m_count = 0;
    
    GLuint m_vaoId = 0;
    GLuint m_vboIds[2] { };
    GLuint m_programId = 0;

    GLboolean m_colorNormalized = GL_FALSE;
    
    GLint m_projectionUniform = 0;
    GLint m_modelViewUniform = 0;
    
    GLint m_vertexAttribute = 0;
    GLint m_colorAttribute = 0;
};

//------------------------------------------------------------------------------------------------

class RenderSolidTrianglesOpenGL : public IRenderSolidTriangles
{
public:
    RenderSolidTrianglesOpenGL(bool colorNormalized);
    ~RenderSolidTrianglesOpenGL();
    
    void Vertex(const Vector2& v, Color color, const glm::mat4x4& modelView, const glm::mat4x4& projection) override;
    void Flush(const glm::mat4x4& modelView, const glm::mat4x4& projection) override;
private:
    
    enum { e_maxVertices = 3 * 512 };
    Vector2 m_vertices[e_maxVertices];
    Color m_colors[e_maxVertices];

    int32 m_vertexCount = 0;

    GLuint m_vaoId = 0;
    GLuint m_vboIds[2] { };
    GLuint m_programId = 0;
    
    GLboolean m_colorNormalized = GL_FALSE;
    
    GLint m_projectionUniform = 0;
    GLint m_modelViewUniform = 0;
    
    GLint m_vertexAttribute = 0;
    GLint m_colorAttribute = 0;
};

//------------------------------------------------------------------------------------------------

class RenderTexturedTrianglesOpenGL : public IRenderTexturedTriangles
{
public:
    RenderTexturedTrianglesOpenGL();
    ~RenderTexturedTrianglesOpenGL();
    
    void SetMode(RenderMode mode) override;
    
    Vertex* GetFanVertices(std::size_t nEdges);
    Vertex* GetVertices(GlTexture texture, const glm::mat4x4& modelView, const glm::mat4x4& projection) override;
    void Flush(const glm::mat4x4& modelView, const glm::mat4x4& projection) override;
private:
    enum { e_maxVertices = 3 * 512 };
    
    GLenum m_blendSFactor = GL_ONE;
    GLenum m_blendDFactor = GL_ZERO;

    Vertex m_vertices[e_maxVertices];
    GLuint m_indices[e_maxVertices * 3];

    int32 m_vertexCount = 0;
    int32 m_indexCount = 0;

    GLuint m_vaoId = 0;
    GLuint m_vboIds[2] { };
    GLuint m_programId = 0;
        
    GLint m_projectionUniform = 0;
    GLint m_modelViewUniform = 0;
    
    GLint m_vertexAttribute = 0;
    GLint m_colorAttribute = 0;
    GLint m_uvAttribute = 0;
    
    GLuint m_texture = 0;
};

//------------------------------------------------------------------------------------------------

class RenderFanOpenGL : public IRenderFan
{
public:
    RenderFanOpenGL();
    ~RenderFanOpenGL();
    
    Vertex* GetVertices(std::size_t nEdges, const glm::mat4x4& modelView, const glm::mat4x4& projection) override;
    void Flush(const glm::mat4x4& modelView, const glm::mat4x4& projection) override;
private:
    enum { e_maxVertices = 3 * 512 };

    Vertex m_vertices[e_maxVertices];
    GLuint m_indices[e_maxVertices * 3];

    int32 m_vertexCount = 0;
    int32 m_indexCount = 0;

    GLuint m_vaoId = 0;
    GLuint m_vboIds[2] { };
    GLuint m_programId = 0;
        
    GLint m_projectionUniform = 0;
    GLint m_modelViewUniform = 0;
    
    GLint m_vertexAttribute = 0;
    GLint m_colorAttribute = 0;
};
