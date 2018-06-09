// Text.cpp

#include "Text.h"
#include "GuiManager.h"
#include "TextureManager.h"
#include "DrawingContext.h"

namespace UI
{

Text* Text::Create(UIWindow *parent, float x, float y, const std::string &text, AlignTextKind align)
{
	Text *t = new Text(parent);
	t->Move(x, y);
	t->SetText(text);
	t->SetAlign(align);
	return t;
}

///////////////////////////////////////////////////////////////////////////////
// Text class implementation

Text::Text(UIWindow *parent)
  : UIWindow(parent)
  , _lineCount(1)
  , _maxline(0)
  , _align(alignTextLT)
  , _fontTexture(0)
  , _fontColor(0xffffffff)
  , _drawShadow(true)
{
	SetFont("font_small");
	SetTexture(nullptr, false);
}

void Text::SetDrawShadow(bool drawShadow)
{
	_drawShadow = drawShadow;
}

bool Text::GetDrawShadow() const
{
	return _drawShadow;
}

void Text::SetAlign(AlignTextKind align)
{
	_align = align;
}

void Text::SetFont(const char *fontName)
{
	_fontTexture = GetManager().GetTextureManager().FindSprite(fontName);
	float w = GetManager().GetTextureManager().GetFrameWidth(_fontTexture, 0);
	float h = GetManager().GetTextureManager().GetFrameHeight(_fontTexture, 0);
	Resize((w - 1) * (float) _maxline, h * (float) _lineCount);
}

void Text::SetFontColor(SpriteColor color)
{
	_fontColor = color;
}

float Text::GetCharWidth()
{
	return GetManager().GetTextureManager().GetFrameWidth(_fontTexture, 0) - 1;
}

float Text::GetCharHeight()
{
	return GetManager().GetTextureManager().GetFrameHeight(_fontTexture, 0);
}

void Text::Draw(DrawingContext &dc) const
{
	UIWindow::Draw(dc);

	if( _drawShadow )
	{
		dc.DrawBitmapText(1, 1, _fontTexture, 0xff000000, GetText(), _align);
	}
	dc.DrawBitmapText(0, 0, _fontTexture, _fontColor, GetText(), _align);
}

void Text::OnTextChange()
{
	// update lines
	_lineCount = 1;
	_maxline = 0;
	size_t count = 0;
	for( size_t n = 0; n != GetText().size(); ++n )
	{
		if( '\n' == GetText()[n] )
		{
			if( _maxline < count )
				_maxline = count;
			++_lineCount;
			count = 0;
		}
		++count;
	}
	if( 1 == _lineCount )
	{
		_maxline = GetText().size();
	}
	float w = GetManager().GetTextureManager().GetFrameWidth(_fontTexture, 0);
	float h = GetManager().GetTextureManager().GetFrameHeight(_fontTexture, 0);
	Resize((w - 1) * (float) _maxline, h * (float) _lineCount);
}

///////////////////////////////////////////////////////////////////////////////
} // end of namespace UI

// end of file

