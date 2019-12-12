#pragma once
#include "Window.h"

#include "rendering/DrawingContext.h"
#include "rendering/Color.h"

namespace UI
{
template<class T> struct DataSource;

class Text : public Window
{
public:
	Text(LayoutManager &manager, TextureManager &texman);

	void SetAlign(AlignTextKind align);
	void SetFont(TextureManager &texman, const char *fontName);
	void SetFontColor(std::shared_ptr<DataSource<Color>> color);

	void SetText(std::shared_ptr<DataSource<const std::string&>> text);

	// Window
	void Draw(const StateContext &sc, const LayoutContext &lc, const InputContext &ic, DrawingContext &dc, TextureManager &texman) const override;
	Vector2 GetContentSize(TextureManager &texman, const StateContext &sc, float scale) const override;

private:
	AlignTextKind  _align;
	size_t         _fontTexture;
	std::shared_ptr<DataSource<Color>> _fontColor;
	std::shared_ptr<DataSource<const std::string&>> _text;
};

} // namespace UI
