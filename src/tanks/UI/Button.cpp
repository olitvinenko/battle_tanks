// Button.cpp

#include "Button.h"
#include "GuiManager.h"
#include "TextureManager.h"
#include "DrawingContext.h"
#include <algorithm>

namespace UI
{

ButtonBase::ButtonBase(UIWindow *parent)
  : UIWindow(parent)
  , _state(stateNormal)
{
}

void ButtonBase::SetState(State s)
{
	if( _state != s )
	{
		_state = s;
		OnChangeState(s);
	}
}

bool ButtonBase::OnPointerMove(float x, float y, PointerType pointerType, unsigned int pointerID)
{
	if( GetManager().HasCapturedPointers(this) )
	{
		if (GetManager().GetCapture(pointerID) == this)
		{
			bool push = x < GetWidth() && y < GetHeight() && x > 0 && y > 0;
			SetState(push ? statePushed : stateNormal);
		}
	}
	else
	{
		SetState(stateHottrack);
	}
	if( eventMouseMove )
		eventMouseMove(x, y);
	return true;
}

bool ButtonBase::OnPointerDown(float x, float y, int button, PointerType pointerType, unsigned int pointerID)
{
	if( !GetManager().HasCapturedPointers(this) && 1 == button ) // primary button only
	{
		GetManager().SetCapture(pointerID, this);
		SetState(statePushed);
		if( eventMouseDown )
			eventMouseDown(x, y);
	}
	return true;
}

bool ButtonBase::OnPointerUp(float x, float y, int button, PointerType pointerType, unsigned int pointerID)
{
	if( GetManager().GetCapture(pointerID) == this && 1 == button )
	{
		GetManager().SetCapture(pointerID, nullptr);
		bool click = (GetState() == statePushed);
		WindowWeakPtr wwp(this);
		if( eventMouseUp )
			eventMouseUp(x, y);          // handler may destroy this object
		if( click && wwp.Get() )
		{
			OnClick();                   // handler may destroy this object
			if( eventClick && wwp.Get() )
				eventClick();            // handler may destroy this object
		}
		if( click && wwp.Get() && GetEnabledCombined() )  // handler may disable this button
			SetState(stateHottrack);
	}
	return true;
}

bool ButtonBase::OnMouseLeave()
{
	SetState(stateNormal);
	return true;
}

bool ButtonBase::OnTap(float x, float y)
{
    if( !GetManager().HasCapturedPointers(this))
    {
        WindowWeakPtr wwp(this);
        OnClick();                   // handler may destroy this object
        if( eventClick && wwp.Get() )
            eventClick();            // handler may destroy this object        
    }
    return true;
}

void ButtonBase::OnClick()
{
}

void ButtonBase::OnChangeState(State state)
{
}

void ButtonBase::OnEnabledChange(bool enable, bool inherited)
{
	if( enable )
	{
		SetState(stateNormal);
	}
	else
	{
		SetState(stateDisabled);
	}
}


///////////////////////////////////////////////////////////////////////////////
// button class implementation

Button* Button::Create(UIWindow *parent, const std::string &text, float x, float y, float w, float h)
{
	Button *res = new Button(parent);
	res->Move(x, y);
	res->SetText(text);
	if( w >= 0 && h >= 0 )
	{
		res->Resize(w, h);
	}

	return res;
}

Button::Button(UIWindow *parent)
  : ButtonBase(parent)
  , _font((size_t)-1)
  , _icon((size_t)-1)
{
	SetTexture("ui/button", true);
	SetFont("font_default");
	OnChangeState(stateNormal);
}

void Button::SetFont(const char *fontName)
{
	_font = GetManager().GetTextureManager().FindSprite(fontName);
}

void Button::SetIcon(const char *spriteName)
{
	_icon = spriteName ? GetManager().GetTextureManager().FindSprite(spriteName) : (size_t)-1;
}

void Button::OnChangeState(State state)
{
	SetFrame(state);
}

void Button::Draw(DrawingContext &dc) const
{
	ButtonBase::Draw(dc);

	SpriteColor c = 0;

	switch( GetState() )
	{
	case statePushed:
		c = 0xffffffff;
		break;
	case stateHottrack:
		c = 0xffffffff;
		break;
	case stateNormal:
		c = 0xffffffff;
		break;
	case stateDisabled:
		c = 0xbbbbbbbb;
		break;
	default:
		assert(false);
	}

	if (_icon != -1)
	{
		float iconHeight = GetManager().GetTextureManager().GetFrameHeight(_icon, 0);
		float textHeight = GetManager().GetTextureManager().GetFrameHeight(_font, 0);

		float x = GetWidth() / 2;
		float y = (GetHeight() - iconHeight - textHeight) / 2 + iconHeight;

		dc.DrawSprite(_icon, 0, c, x, y - iconHeight/2, math::Vector2(1, 0));
		dc.DrawBitmapText(x, y, _font, c, GetText(), alignTextCT);
	}
	else
	{
		float x = GetWidth() / 2;
		float y = GetHeight() / 2;
		dc.DrawBitmapText(x, y, _font, c, GetText(), alignTextCC);
	}
}


///////////////////////////////////////////////////////////////////////////////
// text button class implementation

TextButton* TextButton::Create(UIWindow *parent, float x, float y, const std::string &text, const char *font)
{
	TextButton *res = new TextButton(parent);
	res->Move(x, y);
	res->SetText(text);
	res->SetFont(font);
	return res;
}

TextButton::TextButton(UIWindow *parent)
  : ButtonBase(parent)
  , _fontTexture((size_t) -1)
  , _drawShadow(true)
{
	SetTexture(nullptr, false);
	OnChangeState(stateNormal);
}

void TextButton::AlignSizeToContent()
{
	if( -1 != _fontTexture )
	{
		float w = GetManager().GetTextureManager().GetFrameWidth(_fontTexture, 0);
		float h = GetManager().GetTextureManager().GetFrameHeight(_fontTexture, 0);
		Resize((w - 1) * (float) GetText().length(), h + 1);
	}
}

void TextButton::SetDrawShadow(bool drawShadow)
{
	_drawShadow = drawShadow;
}

bool TextButton::GetDrawShadow() const
{
	return _drawShadow;
}

void TextButton::SetFont(const char *fontName)
{
	_fontTexture = GetManager().GetTextureManager().FindSprite(fontName);
	AlignSizeToContent();
}

void TextButton::OnTextChange()
{
	AlignSizeToContent();
}

void TextButton::Draw(DrawingContext &dc) const
{
	ButtonBase::Draw(dc);

	// grep 'enum State'
	SpriteColor colors[] =
	{
		SpriteColor(0xffffffff), // normal
		SpriteColor(0xffccccff), // hottrack
		SpriteColor(0xffccccff), // pushed
		SpriteColor(0xAAAAAAAA), // disabled
	};
	if( _drawShadow && stateDisabled != GetState() )
	{
		dc.DrawBitmapText(1, 1, _fontTexture, 0xff000000, GetText());
	}
	dc.DrawBitmapText(0, 0, _fontTexture, colors[GetState()], GetText());
}

///////////////////////////////////////////////////////////////////////////////
// ImageButton class implementation

ImageButton* ImageButton::Create(UIWindow *parent, float x, float y, const char *texture)
{
	ImageButton *res = new ImageButton(parent);
	res->Move(x, y);
	res->SetTexture(texture, true);
	return res;
}

ImageButton::ImageButton(UIWindow *parent)
  : ButtonBase(parent)
{
}

void ImageButton::OnChangeState(State state)
{
	SetFrame(state);
}

///////////////////////////////////////////////////////////////////////////////
// CheckBox class implementation

CheckBox* CheckBox::Create(UIWindow *parent, float x, float y, const std::string &text)
{
	CheckBox *res = new CheckBox(parent);
	res->Move(x, y);
	res->SetText(text);
	return res;
}

CheckBox::CheckBox(UIWindow *parent)
  : ButtonBase(parent)
  , _fontTexture(GetManager().GetTextureManager().FindSprite("font_small"))
  , _boxTexture(GetManager().GetTextureManager().FindSprite("ui/checkbox"))
  , _drawShadow(true)
  , _isChecked(false)
{
	SetTexture(nullptr, false);
	AlignSizeToContent();
}

void CheckBox::AlignSizeToContent()
{
	const TextureManager &tm = GetManager().GetTextureManager();
	float th = tm.GetFrameHeight(_fontTexture, 0);
	float tw = tm.GetFrameWidth(_fontTexture, 0);
	float bh = tm.GetFrameHeight(_boxTexture, GetFrame());
	float bw = tm.GetFrameWidth(_boxTexture, GetFrame());
	Resize(bw + (tw - 1) * (float) GetText().length(), std::max(th + 1, bh));
}

void CheckBox::SetCheck(bool checked)
{
	_isChecked = checked;
	SetFrame(_isChecked ? GetState()+4 : GetState());
}

void CheckBox::OnClick()
{
	SetCheck(!GetCheck());
}

void CheckBox::OnTextChange()
{
	AlignSizeToContent();
}

void CheckBox::OnChangeState(State state)
{
	SetFrame(_isChecked ? state+4 : state);
}

void CheckBox::Draw(DrawingContext &dc) const
{
	ButtonBase::Draw(dc);

	float bh = GetManager().GetTextureManager().GetFrameHeight(_boxTexture, GetFrame());
	float bw = GetManager().GetTextureManager().GetFrameWidth(_boxTexture, GetFrame());
	float th = GetManager().GetTextureManager().GetFrameHeight(_fontTexture, 0);

	math::RectFloat box = {0, (GetHeight() - bh) / 2, bw, (GetHeight() - bh) / 2 + bh};
	dc.DrawSprite(&box, _boxTexture, GetBackColor(), GetFrame());

	// grep 'enum State'
	SpriteColor colors[] =
	{
		SpriteColor(0xffffffff), // Normal
		SpriteColor(0xffffffff), // Hottrack
		SpriteColor(0xffffffff), // Pushed
		SpriteColor(0xffffffff), // Disabled
	};
	if( _drawShadow && stateDisabled != GetState() )
	{
		dc.DrawBitmapText(bw + 1, (GetHeight() - th) / 2 + 1, _fontTexture, 0xff000000, GetText());
	}
	dc.DrawBitmapText(bw, (GetHeight() - th) / 2, _fontTexture, colors[GetState()], GetText());
}

} // namespace UI

