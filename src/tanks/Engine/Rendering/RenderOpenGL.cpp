#include "RenderOpenGL.h"
#include "GlTexture.h"
#include "Images/IImage.h"
#include "Line.h"

RenderOpenGL::RenderOpenGL()
	: m_windowWidth(0)
	, m_windowHeight(0)
	, m_curtex(-1)
	, m_ambient(0)
	, m_vaSize(0)
	, m_iaSize(0)
	, m_mode()
{
	memset(m_indexArray, 0, sizeof(GLushort) * INDEX_ARRAY_SIZE);
	memset(m_vertexArray, 0, sizeof(Vertex) * VERTEX_ARRAY_SIZE);
}

RenderOpenGL::~RenderOpenGL() = default;

void RenderOpenGL::OnResizeWnd(unsigned int width, unsigned int height)
{
	m_windowWidth = (int)width;
	m_windowHeight = (int)height;
	SetViewport(nullptr);
	SetScissor(nullptr);
}

void RenderOpenGL::SetScissor(const math::RectInt *rect)
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

void RenderOpenGL::SetViewport(const math::RectInt *rect)
{
	Flush();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (rect)
	{
		glOrtho(0, (GLdouble)(rect->right - rect->left),
			(GLdouble)(rect->bottom - rect->top), 0, -1, 1);
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
		glOrtho(0, (GLdouble)m_windowWidth, (GLdouble)m_windowHeight, 0, -1, 1);
		glViewport(0, 0, m_windowWidth, m_windowHeight);
		m_rtViewport.left = 0;
		m_rtViewport.top = 0;
		m_rtViewport.right = m_windowWidth;
		m_rtViewport.bottom = m_windowHeight;
	}
}

void RenderOpenGL::Camera(const math::RectInt *vp, float x, float y, float scale)
{
	SetViewport(vp);
	SetScissor(vp);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (vp)
		glTranslatef((float)(WIDTH(*vp) / 2) - x * scale, (float)(HEIGHT(*vp) / 2) - y * scale, 0);
	else
		glTranslatef(0, 0, 0);
	glScalef(scale, scale, 1);
}

void RenderOpenGL::Begin()
{
	glEnable(GL_BLEND);

	glTexCoordPointer(2, GL_FLOAT, sizeof(Vertex), &m_vertexArray->u);
	glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(Vertex), &m_vertexArray->color);
	glVertexPointer(2, GL_FLOAT, sizeof(Vertex), &m_vertexArray->x);

	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_VERTEX_ARRAY);

	glClearColor(0, 0, 0, m_ambient);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT);
}

void RenderOpenGL::End()
{
	Flush();
}

void RenderOpenGL::SetMode(RenderMode mode)
{
	Flush();

	switch (mode)
	{
	case LIGHT:
		glClearColor(0, 0, 0, m_ambient);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_TEXTURE_2D);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_TRUE);
		break;

	case WORLD:
		glEnable(GL_TEXTURE_2D);
		glBlendFunc(GL_DST_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
		break;

	case INTERFACE:
		SetViewport(nullptr);
		Camera(nullptr, 0, 0, 1);
		glEnable(GL_TEXTURE_2D);
		glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
		break;

	default:
		assert(false);
	}

	m_mode = mode;
}

bool RenderOpenGL::TexCreate(GlTexture &tex, const IImage &img, bool magFilter)
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

void RenderOpenGL::TexFree(GlTexture tex)
{
	assert(glIsTexture(tex.index));
	glDeleteTextures(1, &tex.index);
}

void RenderOpenGL::Flush()
{
	if (m_iaSize)
	{
		glDrawElements(GL_TRIANGLES, m_iaSize, GL_UNSIGNED_SHORT, m_indexArray);
		m_vaSize = m_iaSize = 0;
	}
}

Vertex* RenderOpenGL::DrawQuad(GlTexture tex)
{
	GLuint& index = reinterpret_cast<GLuint&>(tex.index);
	if (m_curtex != index)
	{
		Flush();
		m_curtex = index;
		glBindTexture(GL_TEXTURE_2D, m_curtex);
	}
	if (m_vaSize > VERTEX_ARRAY_SIZE - 4 || m_iaSize > INDEX_ARRAY_SIZE - 6)
	{
		Flush();
	}

	Vertex *result = &m_vertexArray[m_vaSize];

	m_indexArray[m_iaSize] = m_vaSize;
	m_indexArray[m_iaSize + 1] = m_vaSize + 1;
	m_indexArray[m_iaSize + 2] = m_vaSize + 2;
	m_indexArray[m_iaSize + 3] = m_vaSize;
	m_indexArray[m_iaSize + 4] = m_vaSize + 2;
	m_indexArray[m_iaSize + 5] = m_vaSize + 3;

	m_iaSize += 6;
	m_vaSize += 4;

	return result;
}

Vertex* RenderOpenGL::DrawFan(unsigned int nEdges)
{
	assert(nEdges * 3 < INDEX_ARRAY_SIZE);

	if (m_vaSize + nEdges + 1 > VERTEX_ARRAY_SIZE ||
		m_iaSize + nEdges * 3 > INDEX_ARRAY_SIZE)
	{
		Flush();
	}

	Vertex *result = &m_vertexArray[m_vaSize];

	for (unsigned int i = 0; i < nEdges; ++i)
	{
		m_indexArray[m_iaSize + i * 3] = m_vaSize;
		m_indexArray[m_iaSize + i * 3 + 1] = m_vaSize + i + 1;
		m_indexArray[m_iaSize + i * 3 + 2] = m_vaSize + i + 2;
	}
	m_indexArray[m_iaSize + nEdges * 3 - 1] = m_vaSize + 1;

	m_iaSize += nEdges * 3;
	m_vaSize += nEdges + 1;

	return result;
}

void RenderOpenGL::DrawLines(const Line *lines, size_t count)
{
	Flush();

	glDisable(GL_TEXTURE_2D);

	glBegin(GL_LINES);
	const Line *it = lines, *end = lines + count;
	for (; it != end; ++it)
	{
		glColor4ub(it->color.rgba[3], it->color.rgba[2], it->color.rgba[1], it->color.rgba[0]);
		glVertex2fv(reinterpret_cast<const float *>(&it->begin));
		glVertex2fv(reinterpret_cast<const float *>(&it->end));
	}
	glEnd();

	SetMode(m_mode); // to enable texture
}

void RenderOpenGL::SetAmbient(float ambient)
{
	m_ambient = ambient;
}
