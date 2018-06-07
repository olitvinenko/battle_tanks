#include "GLESProgram.h"
#include "RenderOpenGL.h"
#include <OpenGLES/ES2/gl.h>
#include <cstring>

#define VERTEX_ARRAY_SIZE   1024
#define  INDEX_ARRAY_SIZE   2048

static const char s_vertexShader[] =
R"(
    attribute vec2 vPos;
    attribute vec2 vTexCoord;
    attribute vec4 vColor;
    varying lowp vec2 texCoord;
    varying lowp vec4 color;
    uniform mat4 viewProj;
    void main()
    {
        gl_Position = viewProj * vec4(vPos, 0, 1);
        texCoord = vTexCoord;
        color = vColor;
    }
)";

static const char s_fragmentShader[] =
R"(
    varying lowp vec2 texCoord;
    varying lowp vec4 color;
    uniform sampler2D s_tex;
    void main()
    {
        gl_FragColor = texture2D(s_tex, texCoord) * color;
    }
)";


#define ATTRIB_POSITION     0
#define ATTRIB_TEXCOORD     1
#define ATTRIB_COLOR        2

static const AttribLocationBinding s_bindings[] =
{
    {ATTRIB_POSITION, "vPos"},
    {ATTRIB_COLOR, "vColor"},
    {ATTRIB_TEXCOORD, "vTexCoord"},
    {0, nullptr},
};


//-----------------------------------------------------------------------------

std::unique_ptr<IRender> RenderCreateOpenGL()
{
	return std::unique_ptr<IRender>(nullptr);
}

