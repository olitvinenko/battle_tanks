#include "DrawingContext.h"
#include "TextureManager.h"
#include <algorithm>

DrawingContext::DrawingContext(const TextureManager &tm, unsigned int width, unsigned int height)
	: _tm(tm)
{
	_transformStack.push(Vector2(0, 0));
	_viewport.left = 0;
	_viewport.top = 0;
	_viewport.right = width;
	_viewport.bottom = height;
	_tm.GetRender().OnResizeWnd(width, height);
}

void DrawingContext::PushClippingRect(math::RectInt rect)
{
	rect.left += static_cast<int>(_transformStack.top().x);
	rect.top += static_cast<int>(_transformStack.top().y);
	rect.right += static_cast<int>(_transformStack.top().x);
	rect.bottom += static_cast<int>(_transformStack.top().y);

	if (_clipStack.empty())
	{
		_clipStack.push(rect);
		_tm.GetRender().SetScissor(&rect);
	}
	else
	{
		math::RectInt tmp = _clipStack.top();
		tmp.left = std::min(std::max(tmp.left, rect.left), rect.right);
		tmp.top = std::min(std::max(tmp.top, rect.top), rect.bottom);
		tmp.right = std::max(std::min(tmp.right, rect.right), rect.left);
		tmp.bottom = std::max(std::min(tmp.bottom, rect.bottom), rect.top);
		assert(tmp.right >= tmp.left && tmp.bottom >= tmp.top);
		_clipStack.push(tmp);
		_tm.GetRender().SetScissor(&tmp);
	}
}

void DrawingContext::PopClippingRect()
{
	assert(!_clipStack.empty());
	_clipStack.pop();
	if (_clipStack.empty())
	{
		_tm.GetRender().SetScissor(&_viewport);
	}
	else
	{
		_tm.GetRender().SetScissor(&_clipStack.top());
	}
}

void DrawingContext::PushTransform(Vector2 offset)
{
	assert(!_transformStack.empty());
	_transformStack.push(_transformStack.top() + offset);
}

void DrawingContext::PopTransform()
{
	assert(_transformStack.size() > 1);
	_transformStack.pop();
}

void DrawingContext::DrawSprite(const math::RectFloat *dst, size_t sprite, SpriteColor color, unsigned int frame)
{
	DrawSprite(sprite, frame, color, dst->left, dst->top, dst->right - dst->left, dst->bottom - dst->top, Vector2(1, 0));
}

void DrawingContext::DrawBorder(const math::RectFloat &dst, size_t sprite, SpriteColor color, unsigned int frame)
{
	const LogicalTexture &lt = _tm.GetSpriteInfo(sprite);

	math::RectFloat uvFrame = lt.uvFrames[frame];
	float uvFrameWidth = WIDTH(uvFrame);
	float uvFrameHeight = HEIGHT(uvFrame);

	const float uvBorderWidth = lt.pxBorderSize * uvFrameWidth / lt.pxFrameWidth;
	const float uvBorderHeight = lt.pxBorderSize * uvFrameHeight / lt.pxFrameHeight;

	const float left = dst.left + _transformStack.top().x;
	const float top = dst.top + _transformStack.top().y;
	const float right = dst.right + _transformStack.top().x;
	const float bottom = dst.bottom + _transformStack.top().y;

	MyVertex *v;
	IRender &render = _tm.GetRender();

	// left edge
	v = render.DrawQuad(lt.dev_texture);
	v[0].color = color;
	v[0].u = uvFrame.left - uvBorderWidth;
	v[0].v = uvFrame.top;
	v[0].x = left;
	v[0].y = top + lt.pxBorderSize;
	v[1].color = color;
	v[1].u = uvFrame.left;
	v[1].v = uvFrame.top;
	v[1].x = left + lt.pxBorderSize;
	v[1].y = top + lt.pxBorderSize;
	v[2].color = color;
	v[2].u = uvFrame.left;
	v[2].v = uvFrame.bottom;
	v[2].x = left + lt.pxBorderSize;
	v[2].y = bottom - lt.pxBorderSize;
	v[3].color = color;
	v[3].u = uvFrame.left - uvBorderWidth;
	v[3].v = uvFrame.bottom;
	v[3].x = left;
	v[3].y = bottom - lt.pxBorderSize;

	// right edge
	v = render.DrawQuad(lt.dev_texture);
	v[0].color = color;
	v[0].u = uvFrame.right;
	v[0].v = uvFrame.top;
	v[0].x = right - lt.pxBorderSize;
	v[0].y = top + lt.pxBorderSize;
	v[1].color = color;
	v[1].u = uvFrame.right + uvBorderWidth;
	v[1].v = uvFrame.top;
	v[1].x = right;
	v[1].y = top + lt.pxBorderSize;
	v[2].color = color;
	v[2].u = uvFrame.right + uvBorderWidth;
	v[2].v = uvFrame.bottom;
	v[2].x = right;
	v[2].y = bottom - lt.pxBorderSize;
	v[3].color = color;
	v[3].u = uvFrame.right;
	v[3].v = uvFrame.bottom;
	v[3].x = right - lt.pxBorderSize;
	v[3].y = bottom - lt.pxBorderSize;

	// top edge
	v = render.DrawQuad(lt.dev_texture);
	v[0].color = color;
	v[0].u = uvFrame.left;
	v[0].v = uvFrame.top - uvBorderHeight;
	v[0].x = left + lt.pxBorderSize;
	v[0].y = top;
	v[1].color = color;
	v[1].u = uvFrame.right;
	v[1].v = uvFrame.top - uvBorderHeight;
	v[1].x = right - lt.pxBorderSize;
	v[1].y = top;
	v[2].color = color;
	v[2].u = uvFrame.right;
	v[2].v = uvFrame.top;
	v[2].x = right - lt.pxBorderSize;
	v[2].y = top + lt.pxBorderSize;
	v[3].color = color;
	v[3].u = uvFrame.left;
	v[3].v = uvFrame.top;
	v[3].x = left + lt.pxBorderSize;
	v[3].y = top + lt.pxBorderSize;

	// bottom edge
	v = render.DrawQuad(lt.dev_texture);
	v[0].color = color;
	v[0].u = uvFrame.left;
	v[0].v = uvFrame.bottom;
	v[0].x = left + lt.pxBorderSize;
	v[0].y = bottom - lt.pxBorderSize;
	v[1].color = color;
	v[1].u = uvFrame.right;
	v[1].v = uvFrame.bottom;
	v[1].x = right - lt.pxBorderSize;
	v[1].y = bottom - lt.pxBorderSize;
	v[2].color = color;
	v[2].u = uvFrame.right;
	v[2].v = uvFrame.bottom + uvBorderHeight;
	v[2].x = right - lt.pxBorderSize;
	v[2].y = bottom;
	v[3].color = color;
	v[3].u = uvFrame.left;
	v[3].v = uvFrame.bottom + uvBorderHeight;
	v[3].x = left + lt.pxBorderSize;
	v[3].y = bottom;

	// left top corner
	v = render.DrawQuad(lt.dev_texture);
	v[0].color = color;
	v[0].u = uvFrame.left - uvBorderWidth;
	v[0].v = uvFrame.top - uvBorderHeight;
	v[0].x = left;
	v[0].y = top;
	v[1].color = color;
	v[1].u = uvFrame.left;
	v[1].v = uvFrame.top - uvBorderHeight;
	v[1].x = left + lt.pxBorderSize;
	v[1].y = top;
	v[2].color = color;
	v[2].u = uvFrame.left;
	v[2].v = uvFrame.top;
	v[2].x = left + lt.pxBorderSize;
	v[2].y = top + lt.pxBorderSize;
	v[3].color = color;
	v[3].u = uvFrame.left - uvBorderWidth;
	v[3].v = uvFrame.top;
	v[3].x = left;
	v[3].y = top + lt.pxBorderSize;

	// right top corner
	v = render.DrawQuad(lt.dev_texture);
	v[0].color = color;
	v[0].u = uvFrame.right;
	v[0].v = uvFrame.top - uvBorderHeight;
	v[0].x = right - lt.pxBorderSize;
	v[0].y = top;
	v[1].color = color;
	v[1].u = uvFrame.right + uvBorderWidth;
	v[1].v = uvFrame.top - uvBorderHeight;
	v[1].x = right;
	v[1].y = top;
	v[2].color = color;
	v[2].u = uvFrame.right + uvBorderWidth;
	v[2].v = uvFrame.top;
	v[2].x = right;
	v[2].y = top + lt.pxBorderSize;
	v[3].color = color;
	v[3].u = uvFrame.right;
	v[3].v = uvFrame.top;
	v[3].x = right - lt.pxBorderSize;
	v[3].y = top + lt.pxBorderSize;

	// right bottom corner
	v = render.DrawQuad(lt.dev_texture);
	v[0].color = color;
	v[0].u = uvFrame.right;
	v[0].v = uvFrame.bottom;
	v[0].x = right - lt.pxBorderSize;
	v[0].y = bottom - lt.pxBorderSize;
	v[1].color = color;
	v[1].u = uvFrame.right + uvBorderWidth;
	v[1].v = uvFrame.bottom;
	v[1].x = right;
	v[1].y = bottom - lt.pxBorderSize;
	v[2].color = color;
	v[2].u = uvFrame.right + uvBorderWidth;
	v[2].v = uvFrame.bottom + uvBorderHeight;
	v[2].x = right;
	v[2].y = bottom;
	v[3].color = color;
	v[3].u = uvFrame.right;
	v[3].v = uvFrame.bottom + uvBorderHeight;
	v[3].x = right - lt.pxBorderSize;
	v[3].y = bottom;

	// left bottom corner
	v = render.DrawQuad(lt.dev_texture);
	v[0].color = color;
	v[0].u = uvFrame.left - uvBorderWidth;
	v[0].v = uvFrame.bottom;
	v[0].x = left;
	v[0].y = bottom - lt.pxBorderSize;
	v[1].color = color;
	v[1].u = uvFrame.left;
	v[1].v = uvFrame.bottom;
	v[1].x = left + lt.pxBorderSize;
	v[1].y = bottom - lt.pxBorderSize;
	v[2].color = color;
	v[2].u = uvFrame.left;
	v[2].v = uvFrame.bottom + uvBorderHeight;
	v[2].x = left + lt.pxBorderSize;
	v[2].y = bottom;
	v[3].color = color;
	v[3].u = uvFrame.left - uvBorderWidth;
	v[3].v = uvFrame.bottom + uvBorderHeight;
	v[3].x = left;
	v[3].y = bottom;
}

void DrawingContext::DrawBitmapText(float sx, float sy, size_t tex, SpriteColor color, const std::string &str, AlignTextKind align)
{
	// grep enum enumAlignText LT CT RT LC CC RC LB CB RB
	static const float dx[] = { 0, 1, 2, 0, 1, 2, 0, 1, 2 };
	static const float dy[] = { 0, 0, 0, 1, 1, 1, 2, 2, 2 };

	std::vector<size_t> lines;
	size_t maxline = 0;
	if (align)
	{
		size_t count = 0;
		for (const std::string::value_type *tmp = str.c_str(); *tmp; )
		{
			++count;
			++tmp;
			if ('\n' == *tmp || '\0' == *tmp)
			{
				if (maxline < count)
					maxline = count;
				lines.push_back(count);
				count = 0;
			}
		}
	}

	sx += _transformStack.top().x;
	sy += _transformStack.top().y;

	const LogicalTexture &lt = _tm.GetSpriteInfo(tex);
	IRender &render = _tm.GetRender();

	size_t count = 0;
	size_t line = 0;

	float x0 = sx - floorf(dx[align] * (lt.pxFrameWidth - 1) * (float)maxline / 2);
	float y0 = sy - floorf(dy[align] * lt.pxFrameHeight * (float)lines.size() / 2);

	for (const std::string::value_type *tmp = str.c_str(); *tmp; ++tmp)
	{
		if ('\n' == *tmp)
		{
			++line;
			count = 0;
		}

		if ((unsigned char)*tmp < 32)
		{
			continue;
		}

		const math::RectFloat &rt = lt.uvFrames[(unsigned char)*tmp - 32];
		float x = x0 + (float)((count++) * (lt.pxFrameWidth - 1));
		float y = y0 + (float)(line * lt.pxFrameHeight);

		MyVertex *v = render.DrawQuad(lt.dev_texture);

		v[0].color = color;
		v[0].u = rt.left;
		v[0].v = rt.top;
		v[0].x = x;
		v[0].y = y;

		v[1].color = color;
		v[1].u = rt.left + WIDTH(rt);
		v[1].v = rt.top;
		v[1].x = x + lt.pxFrameWidth;
		v[1].y = y;

		v[2].color = color;
		v[2].u = rt.left + WIDTH(rt);
		v[2].v = rt.bottom;
		v[2].x = x + lt.pxFrameWidth;
		v[2].y = y + lt.pxFrameHeight;

		v[3].color = color;
		v[3].u = rt.left;
		v[3].v = rt.bottom;
		v[3].x = x;
		v[3].y = y + lt.pxFrameHeight;
	}
}

void DrawingContext::DrawSprite(size_t tex, unsigned int frame, SpriteColor color, float x, float y, Vector2 dir)
{
	assert(frame < _tm.GetFrameCount(tex));
	const LogicalTexture &lt = _tm.GetSpriteInfo(tex);
	const math::RectFloat &rt = lt.uvFrames[frame];
	IRender &render = _tm.GetRender();

	MyVertex *v = render.DrawQuad(lt.dev_texture);

	x += _transformStack.top().x;
	y += _transformStack.top().y;

	float width = lt.pxFrameWidth;
	float height = lt.pxFrameHeight;

	float px = lt.uvPivot.x * width;
	float py = lt.uvPivot.y * height;

	v[0].color = color;
	v[0].u = rt.left;
	v[0].v = rt.top;
	v[0].x = x - px * dir.x + py * dir.y;
	v[0].y = y - px * dir.y - py * dir.x;

	v[1].color = color;
	v[1].u = rt.right;
	v[1].v = rt.top;
	v[1].x = x + (width - px) * dir.x + py * dir.y;
	v[1].y = y + (width - px) * dir.y - py * dir.x;

	v[2].color = color;
	v[2].u = rt.right;
	v[2].v = rt.bottom;
	v[2].x = x + (width - px) * dir.x - (height - py) * dir.y;
	v[2].y = y + (width - px) * dir.y + (height - py) * dir.x;

	v[3].color = color;
	v[3].u = rt.left;
	v[3].v = rt.bottom;
	v[3].x = x - px * dir.x - (height - py) * dir.y;
	v[3].y = y - px * dir.y + (height - py) * dir.x;
}

void DrawingContext::DrawSprite(size_t tex, unsigned int frame, SpriteColor color, float x, float y, float width, float height, Vector2 dir)
{
	const LogicalTexture &lt = _tm.GetSpriteInfo(tex);
	const math::RectFloat &rt = lt.uvFrames[frame];

	MyVertex *v = _tm.GetRender().DrawQuad(lt.dev_texture);

	x += _transformStack.top().x;
	y += _transformStack.top().y;


	float px = lt.uvPivot.x * width;
	float py = lt.uvPivot.y * height;

	v[0].color = color;
	v[0].u = rt.left;
	v[0].v = rt.top;
	v[0].x = x - px * dir.x + py * dir.y;
	v[0].y = y - px * dir.y - py * dir.x;

	v[1].color = color;
	v[1].u = rt.right;
	v[1].v = rt.top;
	v[1].x = x + (width - px) * dir.x + py * dir.y;
	v[1].y = y + (width - px) * dir.y - py * dir.x;

	v[2].color = color;
	v[2].u = rt.right;
	v[2].v = rt.bottom;
	v[2].x = x + (width - px) * dir.x - (height - py) * dir.y;
	v[2].y = y + (width - px) * dir.y + (height - py) * dir.x;

	v[3].color = color;
	v[3].u = rt.left;
	v[3].v = rt.bottom;
	v[3].x = x - px * dir.x - (height - py) * dir.y;
	v[3].y = y - px * dir.y + (height - py) * dir.x;
}

void DrawingContext::DrawIndicator(size_t tex, float x, float y, float value) const
{
	const LogicalTexture &lt = _tm.GetSpriteInfo(tex);
	const math::RectFloat &rt = lt.uvFrames[0];
	IRender &render = _tm.GetRender();

	float px = lt.uvPivot.x * lt.pxFrameWidth;
	float py = lt.uvPivot.y * lt.pxFrameHeight;

	MyVertex *v = render.DrawQuad(lt.dev_texture);

	v[0].color = 0xffffffff;
	v[0].u = rt.left;
	v[0].v = rt.top;
	v[0].x = x - px;
	v[0].y = y - py;

	v[1].color = 0xffffffff;
	v[1].u = rt.left + WIDTH(rt) * value;
	v[1].v = rt.top;
	v[1].x = x - px + lt.pxFrameWidth * value;
	v[1].y = y - py;

	v[2].color = 0xffffffff;
	v[2].u = rt.left + WIDTH(rt) * value;
	v[2].v = rt.bottom;
	v[2].x = x - px + lt.pxFrameWidth * value;
	v[2].y = y - py + lt.pxFrameHeight;

	v[3].color = 0xffffffff;
	v[3].u = rt.left;
	v[3].v = rt.bottom;
	v[3].x = x - px;
	v[3].y = y - py + lt.pxFrameHeight;
}

void DrawingContext::DrawLine(size_t tex, SpriteColor color,
	float x0, float y0, float x1, float y1, float phase)
{
	const LogicalTexture &lt = _tm.GetSpriteInfo(tex);
	IRender &render = _tm.GetRender();

	MyVertex *v = render.DrawQuad(lt.dev_texture);

	float len = sqrtf((x1 - x0)*(x1 - x0) + (y1 - y0)*(y1 - y0));
	float phase1 = phase + len / lt.pxFrameWidth;
	float c = (x1 - x0) / len;
	float s = (y1 - y0) / len;
	float py = lt.pxFrameHeight / 2;

	v[0].color = color;
	v[0].u = phase;
	v[0].v = 0;
	v[0].x = x0 + py * s;
	v[0].y = y0 - py * c;

	v[1].color = color;
	v[1].u = phase1;
	v[1].v = 0;
	v[1].x = x0 + len * c + py * s;
	v[1].y = y0 + len * s - py * c;

	v[2].color = color;
	v[2].u = phase1;
	v[2].v = 1;
	v[2].x = x0 + len * c - py * s;
	v[2].y = y0 + len * s + py * c;

	v[3].color = color;
	v[3].u = phase;
	v[3].v = 1;
	v[3].x = x0 - py * s;
	v[3].y = y0 + py * c;
}

void DrawingContext::DrawBackground(size_t tex, float sizeX, float sizeY) const
{
	const LogicalTexture &lt = _tm.GetSpriteInfo(tex);
	IRender &render = _tm.GetRender();
	MyVertex *v = render.DrawQuad(lt.dev_texture);
	v[0].color = 0xffffffff;
	v[0].u = 0;
	v[0].v = 0;
	v[0].x = 0;
	v[0].y = 0;
	v[1].color = 0xffffffff;
	v[1].u = sizeX / lt.pxFrameWidth;
	v[1].v = 0;
	v[1].x = sizeX;
	v[1].y = 0;
	v[2].color = 0xffffffff;
	v[2].u = sizeX / lt.pxFrameWidth;
	v[2].v = sizeY / lt.pxFrameHeight;
	v[2].x = sizeX;
	v[2].y = sizeY;
	v[3].color = 0xffffffff;
	v[3].u = 0;
	v[3].v = sizeY / lt.pxFrameHeight;
	v[3].x = 0;
	v[3].y = sizeY;
}

static const int SINTABLE_SIZE = 32;
static const int SINTABLE_MASK = 0x1f;

static float sintable[SINTABLE_SIZE] = {
	0.000000f, 0.195090f, 0.382683f, 0.555570f,
	0.707106f, 0.831469f, 0.923879f, 0.980785f,
	1.000000f, 0.980785f, 0.923879f, 0.831469f,
	0.707106f, 0.555570f, 0.382683f, 0.195090f,
	-0.000000f,-0.195090f,-0.382683f,-0.555570f,
	-0.707106f,-0.831469f,-0.923879f,-0.980785f,
	-1.000000f,-0.980785f,-0.923879f,-0.831469f,
	-0.707106f,-0.555570f,-0.382683f,-0.195090f,
};

void DrawingContext::DrawPointLight(float intensity, float radius, Vector2 pos)
{
	SpriteColor color;
	color.color = 0x00000000;
	color.a = (unsigned char)std::max(0, std::min(255, int(255.0f * intensity)));

	IRender &render = _tm.GetRender();
	MyVertex *v = render.DrawFan(SINTABLE_SIZE >> 1);
	v[0].color = color;
	v[0].x = pos.x;
	v[0].y = pos.y;
	for (int i = 0; i < SINTABLE_SIZE >> 1; i++)
	{
		v[i + 1].x = pos.x + radius * sintable[(i << 1) + (SINTABLE_SIZE >> 2) & SINTABLE_MASK];
		v[i + 1].y = pos.y + radius * sintable[i << 1];
		v[i + 1].color.color = 0x00000000;
	}
}

void DrawingContext::DrawSpotLight(float intensity, float radius, Vector2 pos, Vector2 dir, float offset, float aspect)
{
	SpriteColor color;
	color.color = 0x00000000;
	color.a = (unsigned char)std::max(0, std::min(255, int(255.0f * intensity)));

	IRender &render = _tm.GetRender();
	MyVertex *v = render.DrawFan(SINTABLE_SIZE);
	v[0].color = color;
	v[0].x = pos.x;
	v[0].y = pos.y;
	for (int i = 0; i < SINTABLE_SIZE; i++)
	{
		float x = offset + radius * sintable[i + (SINTABLE_SIZE >> 2) & SINTABLE_MASK];
		float y = radius * sintable[i] * aspect;
		v[i + 1].x = pos.x + x*dir.x - y*dir.y;
		v[i + 1].y = pos.y + y*dir.x + x*dir.y;
		v[i + 1].color.color = 0x00000000;
	}
}

void DrawingContext::DrawDirectLight(float intensity, float radius, Vector2 pos, Vector2 dir, float length)
{
	SpriteColor color;
	color.color = 0x00000000;
	color.a = (unsigned char)std::max(0, std::min(255, int(255.0f * intensity)));

	IRender &render = _tm.GetRender();
	MyVertex *v = render.DrawFan((SINTABLE_SIZE >> 2) + 4);
	v[0].color = color;
	v[0].x = pos.x;
	v[0].y = pos.y;
	for (int i = 0; i <= SINTABLE_SIZE >> 2; i++)
	{
		float x = radius * sintable[i << 1];
		float y = radius * sintable[(i << 1) + (SINTABLE_SIZE >> 2) & SINTABLE_MASK];
		v[i + 1].x = pos.x - x*dir.x - y*dir.y;
		v[i + 1].y = pos.y - x*dir.y + y*dir.x;
		v[i + 1].color.color = 0x00000000;
	}

	v[(SINTABLE_SIZE >> 2) + 2].color.color = 0x00000000;
	v[(SINTABLE_SIZE >> 2) + 2].x = pos.x + length * dir.x + radius*dir.y;
	v[(SINTABLE_SIZE >> 2) + 2].y = pos.y + length * dir.y - radius*dir.x;

	v[(SINTABLE_SIZE >> 2) + 3].color = color;
	v[(SINTABLE_SIZE >> 2) + 3].x = pos.x + length * dir.x;
	v[(SINTABLE_SIZE >> 2) + 3].y = pos.y + length * dir.y;

	v[(SINTABLE_SIZE >> 2) + 4].color.color = 0x00000000;
	v[(SINTABLE_SIZE >> 2) + 4].x = pos.x + length * dir.x - radius*dir.y;
	v[(SINTABLE_SIZE >> 2) + 4].y = pos.y + length * dir.y + radius*dir.x;

	v = render.DrawFan((SINTABLE_SIZE >> 2) + 1);
	v[0].color = color;
	v[0].x = pos.x + length * dir.x;
	v[0].y = pos.y + length * dir.y;
	for (int i = 0; i <= SINTABLE_SIZE >> 2; i++)
	{
		float x = radius * sintable[i << 1] + length;
		float y = radius * sintable[(i << 1) + (SINTABLE_SIZE >> 2) & SINTABLE_MASK];
		v[i + 1].x = pos.x + x*dir.x - y*dir.y;
		v[i + 1].y = pos.y + x*dir.y + y*dir.x;
		v[i + 1].color.color = 0x00000000;
	}
}

void DrawingContext::Camera(const math::RectInt &viewport, float x, float y, float scale)
{
	_tm.GetRender().Camera(&viewport, x, y, scale);
}

void DrawingContext::SetAmbient(float ambient)
{
	_tm.GetRender().SetAmbient(ambient);
}

void DrawingContext::SetMode(const RenderMode mode)
{
	_tm.GetRender().SetMode(mode);
}
