// Text.h

#pragma once

#include "UIWindow.h"
#include "Rendering/DrawingContext.h"

namespace UI
{

// static text
class Text : public UIWindow
{
public:
	static Text* Create(UIWindow *parent, float x, float y, const std::string &text, AlignTextKind align);

	void SetDrawShadow(bool drawShadow);
	bool GetDrawShadow() const;

	void SetAlign(AlignTextKind align);
	void SetFont(const char *fontName);
	void SetFontColor(Color color);

	float GetCharWidth();
	float GetCharHeight();

	void Draw(DrawingContext &dc) const override;
	void OnTextChange() override;

protected:
	Text(UIWindow *parent);

private:
	size_t         _lineCount;
	size_t         _maxline;
	AlignTextKind  _align;
	size_t         _fontTexture;
	Color    _fontColor;

	bool           _drawShadow;
};

} // namespace UI
