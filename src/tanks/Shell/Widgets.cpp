#include "Widgets.h"

#include "World.h"
#include "GuiManager.h"
#include "TextureManager.h"
#include "DrawingContext.h"
#include <sstream>
#include <iomanip>

FpsCounter::FpsCounter(UIWindow *parent, float x, float y, AlignTextKind align)
  : Text(parent)
  , _nSprites(0)
  , _nLights(0)
  , _nBatches(0)
{
	SetTimeStep(true);
	Move(x, y);
	SetAlign(align);
}

void FpsCounter::OnVisibleChange(bool visible, bool inherited)
{
	SetTimeStep(visible);
}

void FpsCounter::OnTimeStep(float dt)
{
	_dts.push_back(dt);
	if( _dts.size() > 200 ) _dts.pop_front();

	if( GetVisibleCombined() )
	{
		float avr = 0;
		float min = _dts.front();
		float max = _dts.front();

		for( std::list<float>::iterator it = _dts.begin(); it != _dts.end(); ++it )
		{
			avr += *it;
			if( *it > max ) max = *it;
			if( *it < min ) min = *it;
		}
		avr /= (float) _dts.size();

		std::ostringstream s;
		s << std::setfill('0');
		s << "fps:" << std::setw(3) << int(1.0f / max + 0.5f) << '-' << std::setw(3) << int(1.0f / avr + 0.5f) << '-' << std::setw(3) << int(1.0f / min + 0.5f);
//		if (GameContextBase *gc = _appState.GetGameContext())
//		{
//			s << std::setfill(' ');
//			s << "; obj:" << gc->GetWorld().GetList(LIST_objects).size() << '\n';
//			s << std::setw(4) << gc->GetWorld().GetList(LIST_timestep).size() << "timestep";
//
//#ifndef NDEBUG
//			s << " " << std::setw(4) << gc->GetWorld()._garbage.size() << "garbage";
//#endif
//		}


		SetText(s.str());
	}

	_nSprites = 0;
	_nLights  = 0;
	_nBatches = 0;
}

///////////////////////////////////////////////////////////////////////////////

Oscilloscope::Oscilloscope(UI::UIWindow *parent, float x, float y)
  : UIWindow(parent)
  , _barTexture(GetManager().GetTextureManager().FindSprite("ui/bar"))
  , _titleFont(GetManager().GetTextureManager().FindSprite("font_small"))
  , _rangeMin(-0.1f)
  , _rangeMax(0.1f)
  , _gridStepX(1)
  , _gridStepY(1)
  , _scale(3)
{
	Move(x, y);
	SetTexture("ui/list", true);
	SetDrawBorder(true);
	SetClipChildren(true);
}

void Oscilloscope::Push(float value)
{
	_data.push_back(value);
	size_t size = (size_t) (GetWidth() / _scale);
	if( _data.size() > size )
	{
		_data.erase(_data.begin(), _data.begin() + (_data.size() - size));
	}

	AutoGrid();
	AutoRange();
}

void Oscilloscope::SetRange(float rmin, float rmax)
{
	_rangeMin = rmin;
	_rangeMax = rmax;
}

void Oscilloscope::SetTitle(const std::string &title)
{
	_title = title;
}

void Oscilloscope::SetGridStep(float stepX, float stepY)
{
	_gridStepY = stepY;
	_gridStepX = stepX;
}

void Oscilloscope::AutoGrid()
{
	assert(!_data.empty());

	float valMin = _data[0];
	float valMax = _data[0];
	for( size_t i = 1; i < _data.size(); ++i )
	{
		valMin = std::min(valMin, _data[i]);
		valMax = std::max(valMax, _data[i]);
	}

	float range = valMax - valMin;
	if( range != 0 )
	{
		float cheight = GetManager().GetTextureManager().GetCharHeight(_titleFont);
		float count = floor((GetHeight() - cheight) / cheight / 2);
		float dy = range / count;
		if( dy < 1 )
		{
			float p = pow(10.0f, -floor(log10(dy)));
			_gridStepY = ceil(dy * p) / p;
		}
		else if ( dy > 1 )
		{
			float p = pow(10.0f, floor(log10(dy)));
			_gridStepY = ceil(dy / p) * p;
		}
		else
		{
			_gridStepY = 1;
		}
	}
	else
	{
		_gridStepY = 0;
	}
}

void Oscilloscope::AutoRange()
{
	assert(!_data.empty());

	float valMin = _data[0];
	float valMax = _data[0];
	for( size_t i = 1; i < _data.size(); ++i )
	{
		valMin = std::min(valMin, _data[i]);
		valMax = std::max(valMax, _data[i]);
	}

	if( _gridStepY != 0 )
	{
		_rangeMax = ceil(valMax / _gridStepY) * _gridStepY;
		_rangeMin = floor(valMin / _gridStepY) * _gridStepY;
	}
	else
	{
		_rangeMax = valMax;
		_rangeMin = valMin;
	}

	if( _rangeMin == _rangeMax )
	{
		_rangeMin = 0;
		_rangeMax = valMax * 1.5f;
	}
}

void Oscilloscope::Draw(DrawingContext &dc) const
{
	UIWindow::Draw(dc);

	float labelOffset = GetManager().GetTextureManager().GetCharHeight(_titleFont) / 2;

	float scale = (GetHeight() - labelOffset * 2) / (_rangeMin - _rangeMax);
	float center = labelOffset - _rangeMax * scale;
	float dx = GetWidth() - (float) _data.size() * _scale;

	// data
	for( size_t i = 0; i < _data.size(); ++i )
	{
		dc.DrawSprite(_barTexture, 0, 0x44444444, (float) i * _scale + dx, center, 2, _data[i] * scale, Vector2(1,0));
	}

	// grid
	if( _gridStepY != 0 )
	{
		int start = int(_rangeMin / _gridStepY);
		int stop = int(_rangeMax / _gridStepY);
		for( int i = start; i <= stop; ++i )
		{
			float y = (float) i * _gridStepY;
			dc.DrawSprite(_barTexture, 0, 0x44444444, 0, labelOffset - (_rangeMax - y) * scale, GetWidth(), -1, Vector2(1,0));
			std::ostringstream buf;
			buf << y;
			float textWidth = float(6 * buf.str().size()); // FIXME: calc true char width
			dc.DrawBitmapText(GetWidth() - textWidth, labelOffset - (_rangeMax - y) * scale - labelOffset, _titleFont, 0x77777777, buf.str());
		}
	}
	else
	{
		dc.DrawSprite(_barTexture, 0, 0x44444444, 0, labelOffset - _rangeMax * scale, GetWidth(), -1, Vector2(1,0));
	}

	dc.DrawBitmapText(0, labelOffset - labelOffset, _titleFont, 0x77777777, _title);
}

