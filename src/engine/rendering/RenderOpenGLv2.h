#pragma once

#include "base/IRender.h"
#include "Vertex.h"

#if defined (__APPLE__)
#define GL_SILENCE_DEPRECATION
#endif

#include <GLFW/glfw3.h>

#include "RenderPartsOpenGL.h"
#include <memory>

class IImage;
struct Line;
struct GlTexture;
struct ColoredVertex;

class RenderOpenGLv2 : public IRender
{
public:
	RenderOpenGLv2();
	~RenderOpenGLv2();

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
    void DrawPoints(const ColoredVertex* point, std::size_t count, float pointSize) override;
	void DrawLines(const Line* lines, size_t count) override;

private:
    glm::mat4 m_projectionMatrix;
    glm::mat4 m_modelViewMatrix;
    
    std::unique_ptr<RenderPointsOpenGL> m_renderPoints;
    std::unique_ptr<RenderLinesOpenGL> m_renderLines;
    std::unique_ptr<RenderSolidTrianglesOpenGL> m_renderSolidTriangles;
    std::unique_ptr<RenderTexturedTrianglesOpenGL> m_renderTexturedTriangles;
    std::unique_ptr<RenderFanOpenGL> m_renderFan;
    
	int m_windowWidth;
	int m_windowHeight;
	RectInt m_rtViewport;

	float m_ambient;
	RenderMode m_mode;
};

// end of file
