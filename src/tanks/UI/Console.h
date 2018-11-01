// Console.h

#pragma once

#include "UIWindow.h"

#include <deque>
#include <functional>
#include <string>
#include <vector>

namespace UI
{

class ConsoleBuffer;
class ScrollBarVertical;
class Edit;

///////////////////////////////////////////////////////////////////////////////

struct IConsoleHistory
{
	virtual void Enter(std::string str) = 0;
	virtual size_t GetItemCount() const = 0;
	virtual const std::string& GetItem(size_t index) const = 0;

	virtual ~IConsoleHistory() = default;
};

///////////////////////////////////////////////////////////////////////////////

class ConsoleHistoryDefault : public IConsoleHistory
{
public:
	ConsoleHistoryDefault(size_t maxSize);

    void Enter(std::string str) override;
    size_t GetItemCount() const override;
    const std::string& GetItem(size_t index) const override;

private:
	size_t _maxSize;
	std::deque<std::string> _buf;
};

///////////////////////////////////////////////////////////////////////////////

class Console : public UIWindow
{
public:
	Console(UIWindow *parent);
	static Console* Create(UIWindow *parent, float x, float y, float w, float h, ConsoleBuffer *buf);

	float GetInputHeight() const;

	void SetColors(const Color *colors, size_t count);
	void SetHistory(IConsoleHistory *history);
	void SetBuffer(ConsoleBuffer *buf);
	void SetEcho(bool echo);
	std::function<void(const std::string &)> eventOnSendCommand;
	std::function<bool(const std::string &, int &, std::string &)> eventOnRequestCompleteCommand;

protected:
	bool OnChar(int c) override;
	bool OnKeyPressed(Key key) override;
	bool OnMouseWheel(float x, float y, float z) override;
	bool OnPointerDown(float x, float y, int button, PointerType pointerType, unsigned int pointerID) override;
	bool OnPointerUp(float x, float y, int button, PointerType pointerType, unsigned int pointerID) override;
	bool OnPointerMove(float x, float y, PointerType pointerType, unsigned int pointerID) override;

	void OnTimeStep(float dt) override;
	void Draw(DrawingContext &dc) const override;
	void OnSize(float width, float height) override;
	bool OnFocus(bool focus) override;

private:
	void OnScroll(float pos);

private:
	ScrollBarVertical *_scroll;
	Edit  *_input;
	size_t _cmdIndex;
	size_t _font;

	ConsoleBuffer *_buf;
	IConsoleHistory *_history;
	std::vector<Color> _colors;

	bool _echo;
	bool _autoScroll;
};

} // namespace UI
