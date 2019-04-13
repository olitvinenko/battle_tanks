#include "Edit.h"
#include "GuiManager.h"
#include "Keys.h"
#include "Base/IInput.h"
#include "Base/IClipboard.h"

#include <algorithm>
#include <cstring>
#include <sstream>
#include "Rendering/TextureManager.h"
#include "Rendering/DrawingContext.h"

namespace UI
{

Edit* Edit::Create(UIWindow *parent, float x, float y, float width)
{
	Edit *res = new Edit(parent);
	res->Move(x, y);
	res->Resize(width, res->GetHeight());
	return res;
}

///////////////////////////////////////////////////////////////////////////////

Edit::Edit(UIWindow *parent)
  : UIWindow(parent)
  , _selStart(-1)
  , _selEnd(-1)
  , _offset(0)
  , _time(0)
  , _font(GetManager().GetTextureManager().FindSprite("font_small"))
  , _cursor(GetManager().GetTextureManager().FindSprite("ui/editcursor"))
  , _selection(GetManager().GetTextureManager().FindSprite("ui/editsel"))
{
	SetTexture("ui/edit", true);
	SetDrawBorder(true);
	SetClipChildren(true);
	SetSel(0, 0);
	Resize(GetWidth(), GetManager().GetTextureManager().GetCharHeight(_font) + 2);
}

int Edit::GetTextLength() const
{
	return (int) GetText().length();
}

void Edit::SetInt(int value)
{
	std::ostringstream tmp;
	tmp << value;
	SetText(tmp.str());
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
	SetText(tmp.str());
}

float Edit::GetFloat() const
{
    std::istringstream tmp(GetText());
	float result = 0;
	tmp >> result;
	return result;
}

void Edit::SetSel(int begin, int end)
{
	assert(begin >= -1 && end >= -1);

	_selStart = begin <= GetTextLength() ? begin : -1;
	_selEnd   = end <= GetTextLength() ? end : -1;
	_time     = 0;

	float w = GetManager().GetTextureManager().GetFrameWidth(_font, 0) - 1;
	float cpos = GetSelEnd() * w;
	if( cpos - (float) (_offset * w) > GetWidth() - 10 || cpos - (float) (_offset * w) < 10 )
	{
		_offset = size_t(std::max<float>(0, cpos - GetWidth() * 0.5f) / w);
	}
}

int Edit::GetSelStart() const
{
	return static_cast<unsigned>(_selStart) <= static_cast<unsigned>(GetTextLength()) ? _selStart : GetTextLength();
}

int Edit::GetSelEnd() const
{
	return static_cast<unsigned>(_selEnd) <= static_cast<unsigned>(GetTextLength()) ? _selEnd : GetTextLength();
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

void Edit::Draw(DrawingContext &dc) const
{
	UIWindow::Draw(dc);

	float w = GetManager().GetTextureManager().GetFrameWidth(_font, 0) - 1;

	// selection
	if( GetSelLength() && GetTimeStep() )
	{
		math::RectFloat rt;
		rt.left = 1 + (GetSelMin() - static_cast<float>(_offset)) * w;
		rt.top = 0;
		rt.right = rt.left + w * GetSelLength() - 1;
		rt.bottom = rt.top + GetHeight();
		dc.DrawSprite(&rt, _selection, 0xffffffff, 0);
	}

	// text
	Color c = GetEnabledCombined() ? 0xffffffff : 0xaaaaaaaa;
	if( _offset < GetSelMin() )
	{
		dc.DrawBitmapText(0, 1, _font, c, GetText().substr(_offset, GetSelMin() - _offset));
	}
	dc.DrawBitmapText((GetSelMin() - _offset) * w, 1, _font, 0xffff0000, GetText().substr(GetSelMin(), GetSelLength()));
	dc.DrawBitmapText((GetSelMax() - _offset) * w, 1, _font, c, GetText().substr(GetSelMax()));

	// cursor
	if( this == GetManager().GetFocusWnd() && fmodf(_time, 1.0f) < 0.5f )
	{
		math::RectFloat rt;
		rt.left = (GetSelEnd() - (float) _offset) * w;
		rt.top = 0;
		rt.right = rt.left + GetManager().GetTextureManager().GetFrameWidth(_cursor, 0);
		rt.bottom = rt.top + GetHeight();
		dc.DrawSprite(&rt, _cursor, 0xffffffff, 0);
	}
}

bool Edit::OnChar(int c)
{
	if( isprint(static_cast<unsigned char>(c)) && '\t' != c )
	{
		int start = GetSelMin();
		SetText(GetText().substr(0, start) + static_cast<std::string::value_type>(c) + GetText().substr(GetSelMax()));
		SetSel(start + 1, start + 1);
		return true;
	}
	return false;
}

bool Edit::OnKeyPressed(Key key)
{
    bool shift = GetManager().GetInput().GetKey(Key::LeftShift) ||
        GetManager().GetInput().GetKey(Key::RightShift);
    bool control = GetManager().GetInput().GetKey(Key::LeftCtrl) ||
        GetManager().GetInput().GetKey(Key::RightCtrl);
	int tmp;
	switch(key)
	{
	case Key::Insert:
        if( shift )
		{
			Paste();
			return true;
		}
		else if( control )
		{
			Copy();
			return true;
		}
		break;
	case Key::V:
		if( control )
		{
			Paste();
			return true;
		}
		break;
	case Key::C:
		if( control )
		{
			Copy();
			return true;
		}
		break;
	case Key::X:
		if( 0 != GetSelLength() && control )
		{
			Copy();
			SetText(GetText().substr(0, GetSelMin()) + GetText().substr(GetSelMax()));
			SetSel(GetSelMin(), GetSelMin());
			return true;
		}
		break;
	case Key::Delete:
		if( 0 == GetSelLength() && GetSelEnd() < GetTextLength() )
		{
			SetText(GetText().substr(0, GetSelStart())
				+ GetText().substr(GetSelEnd() + 1, GetTextLength() - GetSelEnd() - 1));
		}
		else
		{
			if( shift )
			{
				Copy();
			}
			SetText(GetText().substr(0, GetSelMin()) + GetText().substr(GetSelMax()));
		}
		SetSel(GetSelMin(), GetSelMin());
		return true;
	case Key::Backspace:
		tmp = std::max(0, 0 == GetSelLength() ? GetSelStart() - 1 : GetSelMin());
		if( 0 == GetSelLength() && GetSelStart() > 0 )
		{
			SetText(GetText().substr(0, GetSelStart() - 1)
				+ GetText().substr(GetSelEnd(), GetTextLength() - GetSelEnd()));
		}
		else
		{
			SetText(GetText().substr(0, GetSelMin()) + GetText().substr(GetSelMax()));
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

bool Edit::OnPointerDown(float x, float y, int button, PointerType pointerType, unsigned int pointerID)
{
	if( 1 == button && !GetManager().HasCapturedPointers(this) )
	{
		GetManager().SetCapture(pointerID, this);
		float w = GetManager().GetTextureManager().GetFrameWidth(_font, 0) - 1;
		int sel = std::min(GetTextLength(), std::max(0, int(x / w)) + (int) _offset);
		SetSel(sel, sel);
	}
    return true;
}

bool Edit::OnPointerMove(float x, float y, PointerType pointerType, unsigned int pointerID)
{
	if( GetManager().GetCapture(pointerID) == this )
	{
		float w = GetManager().GetTextureManager().GetFrameWidth(_font, 0) - 1;
		int sel = std::min(GetTextLength(), std::max(0, int(x / w)) + (int) _offset);
		SetSel(GetSelStart(), sel);
	}
    return true;
}

bool Edit::OnPointerUp(float x, float y, int button, PointerType pointerType, unsigned int pointerID)
{
	if( 1 == button && GetManager().GetCapture(pointerID) == this )
	{
		GetManager().SetCapture(pointerID, nullptr);
	}
    return true;
}

bool Edit::OnFocus(bool focus)
{
	_time = 0;
	SetTimeStep(focus);
	return true;
}

void Edit::OnEnabledChange(bool enable, bool inherited)
{
	if( !enable )
	{
		SetSel(0, 0);
		SetFrame(1);
	}
	else
	{
		SetFrame(0);
	}
}

void Edit::OnTextChange()
{
	SetSel(_selStart, _selEnd);
	if( eventChange )
		eventChange();
}

void Edit::OnTimeStep(float dt)
{
	_time += dt;
}

void Edit::Paste()
{
    if( const char *data = GetManager().GetClipboard().GetText().c_str() )
    {
        std::ostringstream buf;
        buf << GetText().substr(0, GetSelMin());
        buf << data;
        buf << GetText().substr(GetSelMax(), GetText().length() - GetSelMax());
        SetText(buf.str());
        SetSel(GetSelMin() + std::strlen(data), GetSelMin() + std::strlen(data));
    }
}

void Edit::Copy() const
{
	std::string str = GetText().substr(GetSelMin(), GetSelLength());
	if( !str.empty() )
	{
        GetManager().GetClipboard().SetText(std::move(str));
	}
}

///////////////////////////////////////////////////////////////////////////////
} // end of namespace UI

// end of file

