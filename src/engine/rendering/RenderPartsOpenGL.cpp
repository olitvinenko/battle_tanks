#include "RenderPartsOpenGL.h"

#include "GlTexture.h"

#include "glm/gtc/type_ptr.hpp" // glm::value_ptr

#include <stdio.h>
#include <stdarg.h>

#if WIN32
//#include <wincon.h>
//
//// Определение прототипа функции
//typedef GLboolean (*GL_ISSHADER) (GLuint);
//typedef void(*GL_GETSHADERIV) (GLuint, GLenum, GLint*);
//
//typedef GLboolean(*GL_GETISPROGRAM) (GLuint);
//typedef GLboolean(*GL_glGetProgramiv) (GLuint, GLenum, GLint*);
//
//typedef void(*GL_glGetShaderInfoLog)(GLuint, GLsizei, GLsizei*, char*);
//typedef void(*GL_glGetProgramInfoLog)(GLuint, GLsizei, GLsizei*, char*);
//
//typedef GLuint(*GL_glCreateShader)(GLenum);
//
//typedef void(*GL_glShaderSource)(GLuint, GLsizei, const char* const*, const GLint*);
//typedef void(*GL_glCompileShader)(GLuint);
//
//typedef void(*GL_glDeleteShader)(GLuint);
//
//template<typename TRet, typename ... Args>
//auto GetFuncAddress(const char* addressName) ->TRet(*)(Args...)
//{
//    using function_ptr_t = TRet(*)(Args...);
//    auto func = reinterpret_cast<function_ptr_t>(wglGetProcAddress(addressName));
//    if (!func)
//    {
//	    //TODO::
//    }
//    return func;
//}
//
//namespace
//{
//#define GL_INFO_LOG_LENGTH 0x8B84
//#define GL_COMPILE_STATUS 0x8B81
//
//    auto glIsShader = GetFuncAddress<GLboolean, GLuint>("glIsShader");
//
//    GL_GETSHADERIV glGetShaderiv = (GL_GETSHADERIV)wglGetProcAddress("glGetShaderiv");
//
//    GL_GETISPROGRAM glIsProgram = (GL_GETISPROGRAM)wglGetProcAddress("glIsProgram");
//    GL_glGetProgramiv glGetProgramiv = (GL_glGetProgramiv)wglGetProcAddress("glGetProgramiv");
//
//    GL_glGetShaderInfoLog glGetShaderInfoLog = (GL_glGetShaderInfoLog)wglGetProcAddress("glGetShaderInfoLog");
//    GL_glGetProgramInfoLog glGetProgramInfoLog = (GL_glGetProgramInfoLog)wglGetProcAddress("glGetProgramInfoLog");
//
//    GL_glCreateShader glCreateShader = (GL_glCreateShader)wglGetProcAddress("glCreateShader");
//
//    GL_glShaderSource glShaderSource = (GL_glShaderSource)wglGetProcAddress("glShaderSource");
//    GL_glCompileShader glCompileShader = (GL_glCompileShader)wglGetProcAddress("glCompileShader");
//
//    GL_glDeleteShader glDeleteShader = (GL_glDeleteShader)wglGetProcAddress("glDeleteShader");
//}

#endif



static void sCheckGLError()
{
    GLenum errCode = glGetError();
    if (errCode != GL_NO_ERROR)
    {
        fprintf(stderr, "OpenGL error = %d\n", errCode);
        assert(false);
    }
}

static void sPrintLog(GLuint object)
{
    GLint log_length = 0;
    if (glIsShader(object))
        glGetShaderiv(object, GL_INFO_LOG_LENGTH, &log_length);
    else if (glIsProgram(object))
        glGetProgramiv(object, GL_INFO_LOG_LENGTH, &log_length);
    else
    {
        fprintf(stderr, "printlog: Not a shader or a program\n");
        return;
    }

    char* log = (char*)malloc(log_length);

    if (glIsShader(object))
        glGetShaderInfoLog(object, log_length, NULL, log);
    else if (glIsProgram(object))
        glGetProgramInfoLog(object, log_length, NULL, log);

    fprintf(stderr, "%s", log);
    free(log);
}

static GLuint sCreateShaderFromString(const char* source, GLenum type)
{
    GLuint res = glCreateShader(type);
    const char* sources[] = { source };
    glShaderSource(res, 1, sources, NULL);
    glCompileShader(res);
    GLint compile_ok = GL_FALSE;
    glGetShaderiv(res, GL_COMPILE_STATUS, &compile_ok);
    if (compile_ok == GL_FALSE)
    {
        fprintf(stderr, "Error compiling shader of type %d!\n", type);
        sPrintLog(res);
        glDeleteShader(res);
        return 0;
    }

    return res;
}

static GLuint sCreateShaderProgram(const char* vs, const char* fs)
{
    GLuint vsId = sCreateShaderFromString(vs, GL_VERTEX_SHADER);
    GLuint fsId = sCreateShaderFromString(fs, GL_FRAGMENT_SHADER);
    assert(vsId != 0 && fsId != 0);

    GLuint programId = glCreateProgram();
    glAttachShader(programId, vsId);
    glAttachShader(programId, fsId);
    glBindFragDataLocation(programId, 0, "color");
    glLinkProgram(programId);

    glDeleteShader(vsId);
    glDeleteShader(fsId);

    GLint status = GL_FALSE;
    glGetProgramiv(programId, GL_LINK_STATUS, &status);
    assert(status != GL_FALSE);
    
    return programId;
}

#define BUFFER_OFFSET(x)  ((const void*) (x))

template <typename T1, typename T2>
inline constexpr std::size_t OffsetOf(T1 T2::*member)
{
    return (std::size_t)&reinterpret_cast<char const&>((((T2*)0)->*member));
}

//------------------------------------------------------------------------------------------------

RenderPointsOpenGL::RenderPointsOpenGL(bool colorNormalized)
    : m_colorNormalized(colorNormalized)
{
    const char* vs = R"-(
        #version 330
    
        uniform mat4 projectionMatrix;
        uniform mat4 modelViewMatrix;
    
        layout(location = 0) in vec2 v_position;
        layout(location = 1) in vec4 v_color;
        layout(location = 2) in float v_size;
    
        out vec4 f_color;
    
        void main(void)
        {
            f_color = v_color;
            gl_Position = projectionMatrix * modelViewMatrix * vec4(v_position, 0.0f, 1.0f);
            gl_PointSize = v_size;
        }
    )-";
    
    const char* fs = R"-(
        #version 330
    
        in vec4 f_color;
        out vec4 color;
    
        void main(void)
        {
            color = f_color;
        }
    )-";
    
    m_programId = sCreateShaderProgram(vs, fs);
    m_projectionUniform = glGetUniformLocation(m_programId, "projectionMatrix");
    m_modelViewUniform = glGetUniformLocation(m_programId, "modelViewMatrix");
    m_vertexAttribute = 0;
    m_colorAttribute = 1;
    m_sizeAttribute = 2;
    
    // Generate
    glGenVertexArrays(1, &m_vaoId);
    glGenBuffers(3, m_vboIds);
    
    glBindVertexArray(m_vaoId);
    glEnableVertexAttribArray(m_vertexAttribute);
    glEnableVertexAttribArray(m_colorAttribute);
    glEnableVertexAttribArray(m_sizeAttribute);
    
    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glVertexAttribPointer(m_vertexAttribute, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
    glVertexAttribPointer(m_colorAttribute, 4, GL_UNSIGNED_BYTE, m_colorNormalized, 0, BUFFER_OFFSET(0));
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors, GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[2]);
    glVertexAttribPointer(m_sizeAttribute, 1, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_sizes), m_sizes, GL_DYNAMIC_DRAW);

    sCheckGLError();
    
    // Cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
    
    m_count = 0;
}

RenderPointsOpenGL::~RenderPointsOpenGL()
{
    if (m_vaoId)
    {
        //glDeleteVertexArrays(1, &m_vaoId);
        //glDeleteBuffers(2, m_vboIds);
        m_vaoId = 0;
    }
    
    if (m_programId)
    {
        //glDeleteProgram(m_programId);
        m_programId = 0;
    }
}

void RenderPointsOpenGL::Draw(Point point, const glm::mat4x4& modelView, const glm::mat4x4& projection)
{
    if (m_count == maxVertices)
        Flush(modelView, projection);
    
    m_vertices[m_count] = point.position;
    m_colors[m_count] = point.color;
    m_sizes[m_count] = point.size;
    
    ++m_count;
}

void RenderPointsOpenGL::Flush(const glm::mat4x4& modelView, const glm::mat4x4& projection)
{
    if (m_count == 0)
        return;
    
    glUseProgram(m_programId);
    
    glUniformMatrix4fv(m_modelViewUniform, 1, GL_FALSE, glm::value_ptr(modelView));
    glUniformMatrix4fv(m_projectionUniform, 1, GL_FALSE, glm::value_ptr(projection));
    
    glBindVertexArray(m_vaoId);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(Vector2), m_vertices);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(Color), m_colors);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[2]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(float32), m_sizes);
    
    glEnable(GL_PROGRAM_POINT_SIZE);
    glDrawArrays(GL_POINTS, 0, m_count);
    glDisable(GL_PROGRAM_POINT_SIZE);
    
    sCheckGLError();
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
    
    m_count = 0;
}

//------------------------------------------------------------------------------------------------

RenderLinesOpenGL::RenderLinesOpenGL(bool colorNormalized)
    : m_colorNormalized(colorNormalized)
{
    const char* vs = R"-(
        #version 330
    
        uniform mat4 projectionMatrix;
        uniform mat4 modelViewMatrix;
    
        layout(location = 0) in vec2 v_position;
        layout(location = 1) in vec4 v_color;
    
        out vec4 f_color;
    
        void main(void)
        {
            f_color = v_color;
            gl_Position = projectionMatrix * modelViewMatrix * vec4(v_position, 0.0f, 1.0f);
        }
    )-";
    
    const char* fs = R"-(
        #version 330
    
        in vec4 f_color;
        out vec4 color;
        void main(void)
        {
            color = f_color;
        }
    )-";
    
    m_programId = sCreateShaderProgram(vs, fs);
    m_projectionUniform = glGetUniformLocation(m_programId, "projectionMatrix");
    m_modelViewUniform = glGetUniformLocation(m_programId, "modelViewMatrix");
    m_vertexAttribute = 0;
    m_colorAttribute = 1;
    
    // Generate
    glGenVertexArrays(1, &m_vaoId);
    glGenBuffers(2, m_vboIds);
    
    glBindVertexArray(m_vaoId);
    glEnableVertexAttribArray(m_vertexAttribute);
    glEnableVertexAttribArray(m_colorAttribute);
    
    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glVertexAttribPointer(m_vertexAttribute, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);
    
    // Color buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
    glVertexAttribPointer(m_colorAttribute, 4, GL_UNSIGNED_BYTE, m_colorNormalized, 0, BUFFER_OFFSET(0));
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors, GL_DYNAMIC_DRAW);
    
    sCheckGLError();
    
    // Cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
    
    m_count = 0;
}

RenderLinesOpenGL::~RenderLinesOpenGL()
{
    if (m_vaoId)
    {
        glDeleteVertexArrays(1, &m_vaoId);
        glDeleteBuffers(2, m_vboIds);
        m_vaoId = 0;
    }
    
    if (m_programId)
    {
        glDeleteProgram(m_programId);
        m_programId = 0;
    }
}

void RenderLinesOpenGL::Draw(const Line *lines, std::size_t count, const glm::mat4x4& modelView, const glm::mat4x4& projection)
{
    if (m_count == maxVertices || m_count + count > maxVertices)
        Flush(modelView, projection);
    
    const Line *it = lines;
    const Line *end = lines + count;
    
    for (; it != end; ++it)
    {
        m_vertices[m_count] = it->begin;
        m_colors[m_count] = it->color;
        
        ++m_count;
        
        m_vertices[m_count] = it->end;
        m_colors[m_count] = it->color;
        
        ++m_count;
    }
}

void RenderLinesOpenGL::Flush(const glm::mat4x4& modelView, const glm::mat4x4& projection)
{
    if (m_count == 0)
        return;
    
    glUseProgram(m_programId);
    
    glUniformMatrix4fv(m_modelViewUniform, 1, GL_FALSE, glm::value_ptr(modelView));
    glUniformMatrix4fv(m_projectionUniform, 1, GL_FALSE, glm::value_ptr(projection));
    
    glBindVertexArray(m_vaoId);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(Vector2), m_vertices);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_count * sizeof(Color), m_colors);
    
    glDrawArrays(GL_LINES, 0, m_count);
    
    sCheckGLError();
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
    
    m_count = 0;
}

//------------------------------------------------------------------------------------------------

RenderSolidTrianglesOpenGL::RenderSolidTrianglesOpenGL(bool colorNormalized)
    : m_colorNormalized(colorNormalized)
{
    const char* vs = R"-(
        #version 330
    
        uniform mat4 projectionMatrix;
        uniform mat4 modelViewMatrix;
    
        layout(location = 0) in vec2 v_position;
        layout(location = 1) in vec4 v_color;
    
        out vec4 f_color;
    
        void main(void)
        {
            f_color = v_color;
            gl_Position = projectionMatrix * modelViewMatrix * vec4(v_position, 0.0f, 1.0f);
        }
      )-";

    const char* fs = R"-(
        #version 330
        in vec4 f_color;
        out vec4 color;
        void main(void)
        {
            color = f_color;
        }
      )-";

    m_programId = sCreateShaderProgram(vs, fs);
    m_projectionUniform = glGetUniformLocation(m_programId, "projectionMatrix");
    m_modelViewUniform = glGetUniformLocation(m_programId, "modelViewMatrix");
    m_vertexAttribute = 0;
    m_colorAttribute = 1;
    
    glUseProgram(m_programId);

    // Generate
    glGenVertexArrays(1, &m_vaoId);
    glGenBuffers(2, m_vboIds);

    glBindVertexArray(m_vaoId);
    glEnableVertexAttribArray(m_vertexAttribute);
    glEnableVertexAttribArray(m_colorAttribute);

    // Vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glVertexAttribPointer(m_vertexAttribute, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);

    // Color buffer
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
    glVertexAttribPointer(m_colorAttribute, 4, GL_UNSIGNED_BYTE, m_colorNormalized, 0, BUFFER_OFFSET(0));
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_colors), m_colors, GL_DYNAMIC_DRAW);
    
    sCheckGLError();

    // Cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
    
    m_vertexCount = 0;
}

RenderSolidTrianglesOpenGL::~RenderSolidTrianglesOpenGL()
{
    if (m_vaoId)
    {
        glDeleteVertexArrays(1, &m_vaoId);
        glDeleteBuffers(2, m_vboIds);
        m_vaoId = 0;
    }

    if (m_programId)
    {
        glDeleteProgram(m_programId);
        m_programId = 0;
    }
}

void RenderSolidTrianglesOpenGL::Vertex(const Vector2& v, Color color, const glm::mat4x4& modelView, const glm::mat4x4& projection)
{
    if (m_vertexCount == e_maxVertices)
        Flush(modelView, projection);

    m_vertices[m_vertexCount] = v;
    m_colors[m_vertexCount] = color;
    ++m_vertexCount;
}

void RenderSolidTrianglesOpenGL::Flush(const glm::mat4x4& modelView, const glm::mat4x4& projection)
{
    if (m_vertexCount == 0)
        return;
    
    glUseProgram(m_programId);
    
    glUniformMatrix4fv(m_modelViewUniform, 1, GL_FALSE, glm::value_ptr(modelView));
    glUniformMatrix4fv(m_projectionUniform, 1, GL_FALSE, glm::value_ptr(projection));
    
    glBindVertexArray(m_vaoId);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertexCount * sizeof(Vector2), m_vertices);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[1]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertexCount * sizeof(Color), m_colors);
    
    glEnable(GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDrawArrays(GL_TRIANGLES, 0, m_vertexCount);
    glDisable(GL_BLEND);
    
    sCheckGLError();
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glUseProgram(0);
    
    m_vertexCount = 0;
}

//------------------------------------------------------------------------------------------------

RenderTexturedTrianglesOpenGL::RenderTexturedTrianglesOpenGL()
{
    const char* vs = R"-(
        #version 330 core
    
        uniform mat4 projectionMatrix;
        uniform mat4 modelViewMatrix;
    
        layout (location = 0) in vec2 v_position;
        layout (location = 1) in vec4 v_color;
        layout (location = 2) in vec2 v_texCoord;

        out vec4 f_color;
        out vec2 f_texCoord;

        void main()
        {
            gl_Position = projectionMatrix * modelViewMatrix * vec4(v_position, 0.0f, 1.0f);
            f_color = v_color;
            f_texCoord = v_texCoord;
        }
    )-";
        
    const char* fs = R"-(
        #version 330 core
    
        in vec4 f_color;
        in vec2 f_texCoord;

        out vec4 color;

        uniform sampler2D Texture;

        void main()
        {
           color = texture(Texture, f_texCoord) * f_color;
        }
    )-";

    m_programId = sCreateShaderProgram(vs, fs);
    m_projectionUniform = glGetUniformLocation(m_programId, "projectionMatrix");
    m_modelViewUniform = glGetUniformLocation(m_programId, "modelViewMatrix");
    m_vertexAttribute = 0;
    m_colorAttribute = 1;
    m_uvAttribute = 2;
        
    glUseProgram(m_programId);

    // Generate
    glGenVertexArrays(1, &m_vaoId);
    glGenBuffers(2, m_vboIds);

    glBindVertexArray(m_vaoId);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices), m_indices, GL_DYNAMIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(m_vertexAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)OffsetOf(&Vertex::x));
    glEnableVertexAttribArray(m_vertexAttribute);
    // Color attribute
    glVertexAttribPointer(m_colorAttribute, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (GLvoid*)OffsetOf(&Vertex::color));
    glEnableVertexAttribArray(m_colorAttribute);
    // TexCoord attribute
    glVertexAttribPointer(m_uvAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)OffsetOf(&Vertex::u));
    glEnableVertexAttribArray(m_uvAttribute);

    sCheckGLError();

    // Cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    sCheckGLError();
    
    m_vertexCount = 0;
    m_indexCount = 0;
}

RenderTexturedTrianglesOpenGL::~RenderTexturedTrianglesOpenGL()
{
    if (m_vaoId)
    {
        glDeleteVertexArrays(1, &m_vaoId);
        glDeleteBuffers(2, m_vboIds);
        m_vaoId = 0;
    }

    if (m_programId)
    {
        glDeleteProgram(m_programId);
        m_programId = 0;
    }
}

void RenderTexturedTrianglesOpenGL::SetMode(RenderMode mode)
{
    switch (mode)
    {
    case LIGHT:
        break;

    case WORLD:
        m_blendSFactor = GL_DST_ALPHA;
        m_blendDFactor = GL_ONE_MINUS_SRC_ALPHA;
        break;

    case INTERFACE:
        m_blendSFactor = GL_ONE;
        m_blendDFactor = GL_ONE_MINUS_SRC_ALPHA;
        break;
    default:
        assert(false);
    }
}

Vertex* RenderTexturedTrianglesOpenGL::GetVertices(GlTexture texture, const glm::mat4x4& modelView, const glm::mat4x4& projection)
{
    GLuint& index = reinterpret_cast<GLuint&>(texture.index);
    if (m_texture != index)
    {
        Flush(modelView, projection);
        m_texture = index;
        
        //TODO:: do I need this
        glActiveTexture(GL_TEXTURE0);
        sCheckGLError();

        glBindTexture(GL_TEXTURE_2D, m_texture);
    }
    
    if (m_vertexCount > e_maxVertices - 4)
        Flush(modelView, projection);

    if (m_vertexCount == e_maxVertices)
        Flush(modelView, projection);
    
    Vertex* result = &m_vertices[m_vertexCount];
    
    m_indices[m_indexCount] = m_vertexCount;
    m_indices[m_indexCount + 1] = m_vertexCount + 1;
    m_indices[m_indexCount + 2] = m_vertexCount + 2;
    m_indices[m_indexCount + 3] = m_vertexCount;
    m_indices[m_indexCount + 4] = m_vertexCount + 2;
    m_indices[m_indexCount + 5] = m_vertexCount + 3;

    m_indexCount += 6;
    m_vertexCount += 4;
    
    return result;
}

void RenderTexturedTrianglesOpenGL::Flush(const glm::mat4x4& modelView, const glm::mat4x4& projection)
{
    sCheckGLError();
    
    if (m_vertexCount == 0)
        return;
        
    glUseProgram(m_programId);

    //TODO:: need this?
    glActiveTexture(GL_TEXTURE0);
    
    glUniformMatrix4fv(m_projectionUniform, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(m_modelViewUniform, 1, GL_FALSE, glm::value_ptr(modelView));
    
    glEnable(GL_BLEND);
    glBlendFunc(m_blendSFactor, m_blendDFactor);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_FALSE);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertexCount * sizeof (Vertex), m_vertices);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_indexCount * sizeof(GLuint), m_indices);
    
    glBindVertexArray(m_vaoId);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    glDisable(GL_BLEND);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    
    m_vertexCount = 0;
    m_indexCount = 0;
    
    sCheckGLError();
}

//------------------------------------------------------------------------------------------------

RenderFanOpenGL::RenderFanOpenGL()
{
    const char* vs = R"-(
        #version 330 core
    
        uniform mat4 projectionMatrix;
        uniform mat4 modelViewMatrix;
    
        layout (location = 0) in vec2 v_position;
        layout (location = 1) in vec4 v_color;

        out vec4 f_color;

        void main()
        {
            gl_Position = projectionMatrix * modelViewMatrix * vec4(v_position, 0.0f, 1.0f);
            f_color = v_color;
        }
    )-";
        
    const char* fs = R"-(
        #version 330 core
    
        in vec4 f_color;
        out vec4 color;

        void main()
        {
           color =  f_color;
        }
    )-";

    m_programId = sCreateShaderProgram(vs, fs);
    m_projectionUniform = glGetUniformLocation(m_programId, "projectionMatrix");
    m_modelViewUniform = glGetUniformLocation(m_programId, "modelViewMatrix");
    m_vertexAttribute = 0;
    m_colorAttribute = 1;
        
    glUseProgram(m_programId);

    // Generate
    glGenVertexArrays(1, &m_vaoId);
    glGenBuffers(2, m_vboIds);

    glBindVertexArray(m_vaoId);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(m_indices), m_indices, GL_DYNAMIC_DRAW);
    
    // Position attribute
    glVertexAttribPointer(m_vertexAttribute, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)OffsetOf(&Vertex::x));
    glEnableVertexAttribArray(m_vertexAttribute);
    // Color attribute
    glVertexAttribPointer(m_colorAttribute, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex), (GLvoid*)OffsetOf(&Vertex::color));
    glEnableVertexAttribArray(m_colorAttribute);

    // Cleanup
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    
    sCheckGLError();
    
    m_vertexCount = 0;
    m_indexCount = 0;
}

RenderFanOpenGL::~RenderFanOpenGL()
{
    if (m_vaoId)
    {
        glDeleteVertexArrays(1, &m_vaoId);
        glDeleteBuffers(2, m_vboIds);
        m_vaoId = 0;
    }

    if (m_programId)
    {
        glDeleteProgram(m_programId);
        m_programId = 0;
    }
}

Vertex* RenderFanOpenGL::GetVertices(std::size_t nEdges, const glm::mat4x4& modelView, const glm::mat4x4& projection)
{
    assert(nEdges * 3 < e_maxVertices * 3);

    if (m_vertexCount + nEdges + 1 > e_maxVertices ||
        m_indexCount + nEdges * 3 > e_maxVertices * 3)
    {
        Flush(modelView, projection);
    }

    Vertex *result = &m_vertices[m_vertexCount];

    for (unsigned int i = 0; i < nEdges; ++i)
    {
        m_indices[m_indexCount + i * 3] = m_vertexCount;
        m_indices[m_indexCount + i * 3 + 1] = m_vertexCount + i + 1;
        m_indices[m_indexCount + i * 3 + 2] = m_vertexCount + i + 2;
    }
    m_indices[m_indexCount + nEdges * 3 - 1] = m_vertexCount + 1;

    m_indexCount += nEdges * 3;
    m_vertexCount += nEdges + 1;

    return result;
}

void RenderFanOpenGL::Flush(const glm::mat4x4& modelView, const glm::mat4x4& projection)
{
    sCheckGLError();
    
    if (m_vertexCount == 0)
        return;
            
    glUseProgram(m_programId);
    
    glUniformMatrix4fv(m_projectionUniform, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(m_modelViewUniform, 1, GL_FALSE, glm::value_ptr(modelView));
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_vboIds[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, m_vertexCount * sizeof (Vertex), m_vertices);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vboIds[1]);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, m_indexCount * sizeof(GLuint), m_indices);
    
    glBindVertexArray(m_vaoId);
    glDrawElements(GL_TRIANGLES, m_indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
    
    glDisable(GL_BLEND);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    
    m_vertexCount = 0;
    m_indexCount = 0;
    
    sCheckGLError();
}

//------------------------------------------------------------------------------------------------
