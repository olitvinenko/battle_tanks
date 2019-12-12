#include "Edit.h"
#include "Clipboard.h"
#include "InputContext.h"
#include "LayoutContext.h"
#include "GuiManager.h"
#include "Keys.h"
#include "UIInput.h"
#include "rendering/Color.h"
#include "rendering/TextureManager.h"
#include "rendering/DrawingContext.h"

#include <algorithm>
#include <cstring>
#include <sstream>

using namespace UI;

Edit::Edit(LayoutManager &manager, TextureManager &texman)
  : Rectangle(manager)
  , _selStart(-1)
  , _selEnd(-1)
  , _offset(0)
  , _font(texman.FindSprite("font_small"))
  , _cursor(texman.FindSprite("ui/editcursor"))
  , _selection(texman.FindSprite("ui/editsel"))
{
	SetTexture(texman, "ui/edit", true);
	SetDrawBorder(true);
	SetClipChildren(true);
	SetSel(0, 0);
}

int Edit::GetTextLength() const
{
	return (int) GetText().length();
}

void Edit::SetInt(int value)
{
	std::ostringstream tmp;
	tmp << value;
	SetText(GetManager().GetTextureManager(), tmp.str());
}

int Edit::GetInt() const
{
	std::istringstream tmp(GetText());
	int result = 0;
	tmp >> result;
	return result;
}

void Edit::SetFloat(float value)
{
	std::ostringstream tmp;
	tmp << value;
	SetText(GetManager().GetTextureManager(), tmp.str());
}

float Edit::GetFloat() const
{
	std::istringstream tmp(GetText());
	float result = 0;
	tmp >> result;
	return result;
}

void Edit::SetSel(int begin, int end, LayoutContext *optionalLC)
{
	assert(begin >= -1 && end >= -1);

	_selStart = begin <= GetTextLength() ? begin : -1;
	_selEnd   = end <= GetTextLength() ? end : -1;
	_lastCursortime = GetManager().GetTime();

	if (optionalLC)
	{
		float pxWidth = optionalLC->GetPixelSize().x;
		float w = std::floor(GetManager().GetTextureManager().GetFrameWidth(_font, 0) * optionalLC->GetScale()) - 1;
		float cpos = GetSelEnd() * w;
		const float pxScrollThreshold = 10 * optionalLC->GetScale();
		if (cpos - (float)(_offset * w) > pxWidth - pxScrollThreshold || cpos - (float)(_offset * w) < pxScrollThreshold)
		{
			_offset = size_t(std::max<float>(0, cpos - pxWidth / 2) / w);
		}
	}
}

int Edit::GetSelStart() const
{
	return (unsigned) _selStart <= (unsigned) GetTextLength() ? _selStart : GetTextLength();
}

int Edit::GetSelEnd() const
{
	return (unsigned) _selEnd <= (unsigned) GetTextLength() ? _selEnd : GetTextLength();
}

int Edit::GetSelLength() const
{
	return abs(GetSelStart() - GetSelEnd());
}

int Edit::GetSelMin() const
{
	return std::min(GetSelStart(), GetSelEnd());
}

int Edit::GetSelMax() const
{
	return std::max(GetSelStart(), GetSelEnd());
}

void Edit::Draw(const StateContext &sc, const LayoutContext &lc, const InputContext &ic, DrawingContext &dc, TextureManager &texman) const
{
	const_cast<Edit*>(this)->SetFrame(lc.GetEnabledCombined() ? 0 : 1);

	Rectangle::Draw(sc, lc, ic, dc, texman);

    float pxCharWidth = std::floor((texman.GetFrameWidth(_font, 0) - 1) * lc.GetScale());

	// selection
	if( GetSelLength() && ic.GetFocused() )
	{
		RectFloat rt;
		rt.left = 1 + (GetSelMin() - (float) _offset) * pxCharWidth;
		rt.top = 0;
		rt.right = rt.left + pxCharWidth * GetSelLength() - 1;
		rt.bottom = rt.top + lc.GetPixelSize().y;
		dc.DrawSprite(rt, _selection, 0xffffffff, 0);
	}

	// text
	Color c = lc.GetEnabledCombined() ? 0xffffffff : 0xaaaaaaaa;
	if( _offset < GetSelMin() )
	{
		dc.DrawBitmapText(Vector2{ 0, 1 }, lc.GetScale(), _font, c, GetText().substr(_offset, GetSelMin() - _offset));
	}
	dc.DrawBitmapText(Vector2{ (GetSelMin() - _offset) * pxCharWidth, 1 }, lc.GetScale(), _font, 0xffff0000, GetText().substr(GetSelMin(), GetSelLength()));
	dc.DrawBitmapText(Vector2{ (GetSelMax() - _offset) * pxCharWidth, 1 }, lc.GetScale(), _font, c, GetText().substr(GetSelMax()));

	float time = GetManager().GetTime() - _lastCursortime;

	// cursor
	if( ic.GetFocused() && fmodf(time, 1.0f) < 0.5f )
	{
		RectFloat rt = MakeRectWH(
            Vector2{(GetSelEnd() - (float) _offset) * pxCharWidth, 0},
            Vector2{std::floor(texman.GetFrameWidth(_cursor, 0) * lc.GetScale()), lc.GetPixelSize().y});
		dc.DrawSprite(rt, _cursor, 0xffffffff, 0);
	}
}

bool Edit::OnChar(int c)
{
	if( isprint((unsigned char) c) && '\t' != c )
	{
		int start = GetSelMin();
		SetText(GetManager().GetTextureManager(), GetText().substr(0, start) + (std::string::value_type) c + GetText().substr(GetSelMax()));
		SetSel(start + 1, start + 1);
		return true;
	}
	return false;
}

bool Edit::OnKeyPressed(InputContext &ic, Key key)
{
	TextureManager &texman = GetManager().GetTextureManager();
	bool shift = ic.GetInput().IsKeyPressed(Key::LeftShift) ||
		ic.GetInput().IsKeyPressed(Key::RightShift);
	bool control = ic.GetInput().IsKeyPressed(Key::LeftCtrl) ||
		ic.GetInput().IsKeyPressed(Key::RightCtrl);
	int tmp;
	switch(key)
	{
	case Key::Insert:
		if( shift )
		{
			Paste(texman, ic);
			return true;
		}
		else if( control )
		{
			Copy(ic);
			return true;
		}
		break;
	case Key::V:
		if( control )
		{
			Paste(texman, ic);
			return true;
		}
		break;
	case Key::C:
		if( control )
		{
			Copy(ic);
			return true;
		}
		break;
	case Key::X:
		if( 0 != GetSelLength() && control )
		{
			Copy(ic);
			SetText(texman, GetText().substr(0, GetSelMin()) + GetText().substr(GetSelMax()));
			SetSel(GetSelMin(), GetSelMin());
			return true;
		}
		break;
	case Key::Delete:
		if( 0 == GetSelLength() && GetSelEnd() < GetTextLength() )
		{
			SetText(texman, GetText().substr(0, GetSelStart())
				+ GetText().substr(GetSelEnd() + 1, GetTextLength() - GetSelEnd() - 1));
		}
		else
		{
			if( shift )
			{
				Copy(ic);
			}
			SetText(texman, GetText().substr(0, GetSelMin()) + GetText().substr(GetSelMax()));
		}
		SetSel(GetSelMin(), GetSelMin());
		return true;
	case Key::Backspace:
		tmp = std::max(0, 0 == GetSelLength() ? GetSelStart() - 1 : GetSelMin());
		if( 0 == GetSelLength() && GetSelStart() > 0 )
		{
			SetText(texman, GetText().substr(0, GetSelStart() - 1)
				+ GetText().substr(GetSelEnd(), GetTextLength() - GetSelEnd()));
		}
		else
		{
			SetText(texman, GetText().substr(0, GetSelMin()) + GetText().substr(GetSelMax()));
		}
		SetSel(tmp, tmp);
		return true;
	case Key::Left:
		if( shift )
		{
			tmp = std::max(0, GetSelEnd() - 1);
			SetSel(GetSelStart(), tmp);
		}
		else
		{
			tmp = std::max(0, GetSelMin() - 1);
			SetSel(tmp, tmp);
		}
		return true;
	case Key::Right:
		if( shift )
		{
			tmp = std::min(GetTextLength(), GetSelEnd() + 1);
			SetSel(GetSelStart(), tmp);
		}
		else
		{
			tmp = std::min(GetTextLength(), GetSelMax() + 1);
			SetSel(tmp, tmp);
		}
		return true;
	case Key::Home:
		if( shift )
		{
			SetSel(GetSelStart(), 0);
		}
		else
		{
			SetSel(0, 0);
		}
		return true;
	case Key::End:
		if( shift )
		{
			SetSel(GetSelStart(), -1);
		}
		else
		{
			SetSel(-1, -1);
		}
		return true;
	case Key::Space:
		return true;

	default:
		break;
	}
	return false;
}

bool Edit::OnPointerDown(InputContext &ic, LayoutContext &lc, TextureManager &texman, Vector2 pointerPosition, int button, PointerType pointerType, unsigned int pointerID)
{
	if( pointerType == PointerType::Mouse && 1 == button && !ic.HasCapturedPointers(this) )
	{
        int sel = HitTest(texman, pointerPosition, lc.GetScale());
		SetSel(sel, sel);
		return true;
	}
	return false;
}

void Edit::OnPointerMove(InputContext &ic, LayoutContext &lc, TextureManager &texman, Vector2 pointerPosition, PointerType pointerType, unsigned int pointerID, bool captured)
{
	if( captured )
	{
        int sel = HitTest(texman, pointerPosition, lc.GetScale());
		SetSel(GetSelStart(), sel);
	}
}

void Edit::OnTap(InputContext &ic, LayoutContext &lc, TextureManager &texman, Vector2 pointerPosition)
{
    if (!ic.HasCapturedPointers(this))
    {
        int sel = HitTest(texman, pointerPosition, lc.GetScale());
        SetSel(sel, sel);
    }
}

KeyboardSink* Edit::GetKeyboardSink()
{
	// FIXME: gross hack
	_lastCursortime = GetManager().GetTime();
	return this;
}

const std::string& Edit::GetText() const
{
	return _text;
}

void Edit::SetText(TextureManager &texman, const std::string &text)
{
	_text.assign(text);
	OnTextChange(texman);
}

void Edit::OnTextChange(TextureManager &texman)
{
	SetSel(_selStart, _selEnd);
	if( eventChange )
		eventChange();
}

int Edit::HitTest(TextureManager &texman, Vector2 px, float scale) const
{
    float pxCharWidth = std::floor((texman.GetFrameWidth(_font, 0) - 1) * scale);
    return std::min(GetTextLength(), std::max(0, int(px.x / pxCharWidth)) + (int) _offset);
}

void Edit::Paste(TextureManager &texman, InputContext &ic)
{
	if( const char *data = ic.GetClipboard().GetClipboardText() )
	{
		std::ostringstream buf;
		buf << GetText().substr(0, GetSelMin());
		buf << data;
		buf << GetText().substr(GetSelMax(), GetText().length() - GetSelMax());
		SetText(texman, buf.str());
		SetSel(GetSelMin() + std::strlen(data), GetSelMin() + std::strlen(data));
	}
}

void Edit::Copy(InputContext &ic) const
{
	std::string str = GetText().substr(GetSelMin(), GetSelLength());
	if( !str.empty() )
	{
		ic.GetClipboard().SetClipboardText(std::move(str));
	}
}

Vector2 Edit::GetContentSize(TextureManager &texman, const StateContext &sc, float scale) const
{
	return Vector2{ 0, std::floor(texman.GetFrameHeight(_font, 0) * scale) + 2 };
}
