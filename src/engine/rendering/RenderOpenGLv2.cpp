#include "RenderOpenGLv2.h"
#include "GlTexture.h"
#include "base/IImage.h"
#include "Line.h"

#include "glm/ext/matrix_clip_space.hpp" // glm::perspective
#include "glm/ext/matrix_transform.hpp" // glm::translate, glm::rotate, glm::scale


RenderOpenGLv2::RenderOpenGLv2()
	: m_windowWidth(0)
	, m_windowHeight(0)
	, m_ambient(0)
{
    m_renderPoints = std::make_unique<RenderPointsOpenGL>(GL_TRUE);
    m_renderLines = std::make_unique<RenderLinesOpenGL>(GL_TRUE);
    m_renderSolidTriangles = std::make_unique<RenderSolidTrianglesOpenGL>(GL_TRUE);
    m_renderTexturedTriangles = std::make_unique<RenderTexturedTrianglesOpenGL>();
    m_renderFan = std::make_unique<RenderFanOpenGL>();
}

RenderOpenGLv2::~RenderOpenGLv2() = default;

void RenderOpenGLv2::OnResizeWnd(unsigned int width, unsigned int height)
{
	m_windowWidth = (int)width;
	m_windowHeight = (int)height;
	SetViewport(nullptr);
	SetScissor(nullptr);
}

void RenderOpenGLv2::SetScissor(const RectInt* rect)
{
	Flush();
	if (rect)
	{
		glScissor(rect->left, m_windowHeight - rect->bottom, rect->right - rect->left, rect->bottom - rect->top);
		glEnable(GL_SCISSOR_TEST);
	}
	else
	{
		glDisable(GL_SCISSOR_TEST);
	}
}

void RenderOpenGLv2::SetViewport(const RectInt *rect)
{
	Flush();

	if (rect)
	{
        m_projectionMatrix = glm::ortho<float>(0, (GLfloat)(rect->right - rect->left), (GLfloat)(rect->bottom - rect->top), 0, -1, 1);
        
		glViewport(
			rect->left,                       // X
			m_windowHeight - rect->bottom,     // Y
			rect->right - rect->left,         // width
			rect->bottom - rect->top          // height
		);
		m_rtViewport = *rect;
	}
	else
	{
        m_projectionMatrix = glm::ortho<float>(0, (GLfloat)m_windowWidth, (GLfloat)m_windowHeight, 0, -1, 1);
        
		glViewport(0, 0, m_windowWidth, m_windowHeight);
		m_rtViewport.left = 0;
		m_rtViewport.top = 0;
		m_rtViewport.right = m_windowWidth;
		m_rtViewport.bottom = m_windowHeight;
	}
}

void RenderOpenGLv2::Camera(const RectInt* vp, float x, float y, float scale)
{
	SetViewport(vp);
	SetScissor(vp);
    
    m_modelViewMatrix = glm::mat4(1.0f);
    
    if (vp)
        m_modelViewMatrix = glm::translate(m_modelViewMatrix, glm::vec3((float)(WIDTH(*vp) / 2) - x * scale, (float)(HEIGHT(*vp) / 2) - y * scale, 0));
    else
        m_modelViewMatrix = glm::translate(m_modelViewMatrix, glm::vec3(0, 0, 0));
    
    m_modelViewMatrix = glm::scale(m_modelViewMatrix, glm::vec3(scale, scale, 0));
}

void RenderOpenGLv2::Begin()
{
	glClearColor(0, 0, 0, m_ambient);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT);
}

void RenderOpenGLv2::End()
{
	Flush();
}

void RenderOpenGLv2::SetMode(RenderMode mode)
{
	Flush();
    
    if (mode == LIGHT)
    {
        glClearColor(0, 0, 0, m_ambient);
    }
    
    if (mode == INTERFACE)
    {
        SetViewport(nullptr);
        Camera(nullptr, 0, 0, 1);
    }
    
    m_renderTexturedTriangles->SetMode(mode);

    m_renderFan->SetMode(mode);
    m_renderPoints->SetMode(mode);
    m_renderLines->SetMode(mode);
    m_renderSolidTriangles->SetMode(mode);
}

bool RenderOpenGLv2::TexCreate(GlTexture &tex, const IImage &img, bool magFilter)
{
	glGenTextures(1, &tex.index);
	glBindTexture(GL_TEXTURE_2D, tex.index);

	glTexImage2D(
		GL_TEXTURE_2D,                      // target
		0,                                  // level
		GL_RGBA,                            // internalformat
		img.GetWidth(),                    // width
		img.GetHeight(),                   // height
		0,                                  // border
		(24 == img.GetBitsPerPixel()) ? GL_RGB : GL_RGBA, // format
		GL_UNSIGNED_BYTE,                   // type
		img.GetData()                      // pixels
	);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,  magFilter ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, /*GL_NEAREST*/ GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	return true;
}

void RenderOpenGLv2::TexFree(GlTexture tex)
{
	assert(glIsTexture(tex.index));
	glDeleteTextures(1, &tex.index);
}

void RenderOpenGLv2::Flush()
{
    m_renderFan->Flush(m_modelViewMatrix, m_projectionMatrix);
    m_renderTexturedTriangles->Flush(m_modelViewMatrix, m_projectionMatrix);
    m_renderPoints->Flush(m_modelViewMatrix, m_projectionMatrix);
    m_renderLines->Flush(m_modelViewMatrix, m_projectionMatrix);
    m_renderSolidTriangles->Flush(m_modelViewMatrix, m_projectionMatrix);
}

Vertex* RenderOpenGLv2::DrawQuad(GlTexture tex)
{
    return m_renderTexturedTriangles->GetVertices(tex, m_modelViewMatrix, m_projectionMatrix);
}

Vertex* RenderOpenGLv2::DrawFan(unsigned int nEdges)
{
    return m_renderFan->GetVertices(nEdges, m_modelViewMatrix, m_projectionMatrix);
}

void RenderOpenGLv2::DrawTriangles(const ColoredVertex* vertices, std::size_t count)
{
    for (std::size_t i = 1; i < count - 1; ++i)
    {
        m_renderSolidTriangles->Vertex(vertices[0].position, vertices[0].color, m_modelViewMatrix, m_projectionMatrix);
        m_renderSolidTriangles->Vertex(vertices[i].position, vertices[i].color, m_modelViewMatrix, m_projectionMatrix);
        m_renderSolidTriangles->Vertex(vertices[i+1].position, vertices[i+1].color, m_modelViewMatrix, m_projectionMatrix);
    }
}

void RenderOpenGLv2::DrawPoints(const ColoredVertex* points, std::size_t count, float pointSize)
{
    for (std::size_t i = 0; i < count; ++i)
    {
        Point p { points[i].position, points[i].color, pointSize };
        m_renderPoints->Draw(p, m_modelViewMatrix, m_projectionMatrix);
    }
}

void RenderOpenGLv2::DrawLines(const Line *lines, size_t count)
{
    m_renderLines->Draw(lines, count, m_modelViewMatrix, m_projectionMatrix);
}

void RenderOpenGLv2::SetAmbient(float ambient)
{
	m_ambient = ambient;
}
