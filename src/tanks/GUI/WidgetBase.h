#pragma once

#include "Rendering/IDrawable.h"
#include "Rendering/Color.h"

#include <list>

enum class StretchMode
{
	Stretch,
	Fill,
};

class DrawingContext;
class TextureManager;

class WidgetBase
{
public:
	WidgetBase(WidgetBase* parent, const TextureManager& textures);
	virtual ~WidgetBase();

	// rendering
	virtual void Draw(DrawingContext& dc, float interpolation) const;

	// size & position
	void Move(float x, float y);
	float GetX() const { return m_x; }
	float GetY() const { return m_y; }

	void Resize(float width, float height);
	float GetWidth()  const { return m_width; }
	float GetHeight() const { return m_height; }

	// logic
	void Update(float deltaTime);

	bool Contains(const WidgetBase* other) const;

	// Appearance
	void SetBackColor(Color color)   { _backColor = color; }
	Color GetBackColor() const       { return _backColor;  }

	void SetBorderColor(Color color) { _borderColor = color; }
	Color GetBorderColor() const     { return _borderColor;  }

	void SetDrawBorder(bool enable)        { _drawBorder = enable; }
	bool GetDrawBorder() const             { return _drawBorder;   }

	void SetDrawBackground(bool enable)    { _drawBackground = enable; }
	bool GetDrawBackground() const         { return _drawBackground;   }

	void SetTexture(const char *tex, bool fitSize);
	void SetTextureStretchMode(StretchMode stretchMode);
	float GetTextureWidth()  const;
	float GetTextureHeight() const;

	void SetVisible(bool show);
	bool GetVisible() const { return _isVisible; }
	bool GetVisibleCombined() const;   // also includes inherited parent visibility

	void SetTopMost(bool topmost);
	bool GetTopMost() const { return _isTopMost; }

	void SetClipChildren(bool clip)  { _clipChildren = clip; }
	bool GetClipChildren() const     { return _clipChildren; }

	void BringToFront();
	void BringToBack();

	const std::string& GetText() const;
	void SetText(const std::string &text);

protected:
	
	//
	// size & position handlers
	//
	virtual void OnMove(float x, float y) { }
	virtual void OnSize(float width, float height) { }


	//
	// other
	//
	virtual void OnTextChange() { }

	std::list<WidgetBase*> m_children;

private:

	//
	// size and position
	//

	float m_x;
	float m_y;
	float m_width;
	float m_height;

	//
	// attributes
	//

	std::string     _text;

	Color  _backColor;
	Color  _borderColor;
	size_t       _texture;
	StretchMode  _textureStretchMode;
	unsigned int _frame;

	bool _isVisible : 1;
	bool _isEnabled : 1;
	bool _isTopMost : 1;
	bool _isTimeStep : 1;
	bool _drawBorder : 1;
	bool _drawBackground : 1;
	bool _clipChildren : 1;


	WidgetBase* m_parent;

	const TextureManager& m_textureManager;
};