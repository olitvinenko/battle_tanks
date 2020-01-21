#pragma once

#include "base/IRender.h"
#include "Vertex.h"

#if defined (__APPLE__)
#define GL_SILENCE_DEPRECATION
#endif

#include <GLFW/glfw3.h>

class IImage;
struct Line;
struct GlTexture;

class RenderOpenGL : public IRender
{
public:
	RenderOpenGL();
	~RenderOpenGL() override;

	void Flush();

	void OnResizeWnd(unsigned int width, unsigned int height) override;

	void SetViewport(const RectInt *rect) override;
	void SetScissor(const RectInt *rect) override;
	void Camera(const RectInt* vp, float x, float y, float scale) override;

	void Begin() override;
	void End() override;
	void SetMode(RenderMode mode) override;

	void SetAmbient(float ambient) override;

	bool TexCreate(GlTexture& tex, const IImage& img, bool magFilter) override;
	void TexFree(GlTexture tex) override;

	Vertex* DrawQuad(GlTexture tex) override;
	Vertex* DrawFan(unsigned int nEdges) override;

    void DrawTriangles(const ColoredVertex* vertices, std::size_t count) override;
    void DrawPoints(const ColoredVertex* points, std::size_t count, float pointSize) override;
	void DrawLines(const Line* lines, size_t count) override;

private:
	int m_windowWidth;
	int m_windowHeight;
	RectInt   m_rtViewport;

	GLuint m_curtex;
	float  m_ambient;

	static const int VERTEX_ARRAY_SIZE = 1024;
	static const int INDEX_ARRAY_SIZE = 2048;

	GLushort m_indexArray[INDEX_ARRAY_SIZE];
	Vertex m_vertexArray[VERTEX_ARRAY_SIZE];

	unsigned int m_vaSize;      // number of filled elements in _VertexArray
	unsigned int m_iaSize;      // number of filled elements in _IndexArray

	RenderMode  m_mode;
};

// end of file
