#include "WidgetBase.h"
#include <algorithm>
#include "Rect.h"
#include "Rendering/TextureManager.h"
#include "Rendering/DrawingContext.h"

WidgetBase::WidgetBase(WidgetBase* parent, const TextureManager& textures)
	: m_parent(parent)
	, m_textureManager(textures)
	, m_x(0)
	, m_y(0)
	, m_width(0)
	, m_height(0)
	, _backColor(0xffffffff)
	, _borderColor(0xffffffff)
	, _texture(-1)
	, _textureStretchMode(StretchMode::Stretch)
	, _frame(0)
	, _isVisible(true)
	, _isEnabled(true)
	, _isTopMost(false)
	, _isTimeStep(false)
	, _drawBorder(true)
	, _drawBackground(true)
	, _clipChildren(false)
{
	if (m_parent)
		m_parent->m_children.push_back(this);
}

WidgetBase::~WidgetBase()
{
	std::for_each(m_children.begin(), m_children.end(), [](WidgetBase* child) { delete child; });
	m_children.clear();
}

void WidgetBase::SetTexture(const char *tex, bool fitSize)
{
	if (tex)
	{
		_texture = m_textureManager.FindSprite(tex);
		if (fitSize)
		{
			Resize(GetTextureWidth(), GetTextureHeight());
		}
	}
	else
	{
		_texture = (size_t)-1;
	}
}

void WidgetBase::SetTextureStretchMode(StretchMode stretchMode)
{
	_textureStretchMode = stretchMode;
}

float WidgetBase::GetTextureWidth() const
{
	return (-1 != _texture) ? m_textureManager.GetFrameWidth(_texture, _frame) : 1;
}

float WidgetBase::GetTextureHeight() const
{
	return (-1 != _texture) ? m_textureManager.GetFrameHeight(_texture, _frame) : 1;
}

void WidgetBase::Draw(DrawingContext& dc, float interpolation) const
{
	assert(_isVisible);

	math::RectFloat dst = { 0, 0, m_width, m_height };

	if (_texture != -1 )
	{
		if (_drawBackground)
		{
			float border = _drawBorder ? m_textureManager.GetBorderSize(_texture) : 0.f;
			math::RectFloat client = { dst.left + border, dst.top + border, dst.right - border, dst.bottom - border };
			if (_textureStretchMode == StretchMode::Stretch)
			{
				dc.DrawSprite(&client, _texture, _backColor, _frame);
			}
			else
			{
				math::RectInt clip;
				math::FRectToRect(&clip, &client);
				dc.PushClippingRect(clip);

				float frameWidth = m_textureManager.GetFrameWidth(_texture, _frame);
				float frameHeight = m_textureManager.GetFrameHeight(_texture, _frame);

				if (WIDTH(client) * frameHeight > HEIGHT(client) * frameWidth)
				{
					float newHeight = WIDTH(client) / frameWidth * frameHeight;
					client.top = (HEIGHT(client) - newHeight) / 2;
					client.bottom = client.top + newHeight;
				}
				else
				{
					float newWidth = HEIGHT(client) / frameHeight * frameWidth;
					client.left = (WIDTH(client) - newWidth) / 2;
					client.right = client.left + newWidth;
				}

				dc.DrawSprite(&client, _texture, _backColor, _frame);

				dc.PopClippingRect();
			}
		}
		if (_drawBorder)
		{
			dc.DrawBorder(dst, _texture, _borderColor, _frame);
		}
	}

	//TODO:: skip if not visible! clipping rect?
	for(std::list<WidgetBase*>::const_iterator it = m_children.begin(); it != m_children.end(); ++it)
	{
		dc.PushTransform(Vector2((*it)->GetX(), (*it)->GetY()));
		(*it)->Draw(dc, interpolation);
		dc.PopTransform();
	}
}

void WidgetBase::Update(float deltaTime)
{
	//TODO::
}

void WidgetBase::Move(float x, float y)
{
	if (m_x != x || m_y != y)
	{
		m_x = x;
		m_y = y;
		OnMove(x, y);
	}
}

void WidgetBase::Resize(float width, float height)
{
	if (m_width != width || m_height != height)
	{
		m_width = width;
		m_height = height;
		OnSize(width, height);
	}
}

bool WidgetBase::Contains(const WidgetBase *other) const
{
	while (other)
	{
		if (other == this)
		{
			return true;
		}
		other = other->m_parent;
	}
	return false;
}

const std::string& WidgetBase::GetText() const
{
	return _text;
}

void WidgetBase::SetText(const std::string &text)
{
	_text.assign(text);
	OnTextChange();
}