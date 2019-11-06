#pragma once
#include <Window.h>
#include <string>
#include <deque>

class ShellConfig;
class TextureManager;

namespace UI
{
	class ConsoleBuffer;
}

class MessageArea : public UI::Window
{
public:
	MessageArea(UI::LayoutManager &manager, TextureManager &texman, ShellConfig &conf, UI::ConsoleBuffer &logger);

	void WriteLine(const std::string &text);
	void Clear();

	// UI::Window
	void OnTimeStep(UI::LayoutManager &manager, float dt) override;
	void Draw(const UI::StateContext &sc, const UI::LayoutContext &lc, const UI::InputContext &ic, DrawingContext &dc, TextureManager &texman) const override;

private:
	struct Line
	{
		float time;
		std::string str;
	};
	typedef std::deque<Line> LineList;
	LineList _lines;
	size_t _fontTexture;
	ShellConfig &_conf;
	UI::ConsoleBuffer &_logger;
};
