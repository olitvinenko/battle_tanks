#pragma once
#include "UIWindow.h"

namespace UI
{

class Edit : public UIWindow
{
	int   _selStart;
	int   _selEnd;
	int   _offset;
	float _time;
	size_t _font;
	size_t _cursor;
	size_t _selection;

public:
	Edit(UIWindow *parent);
	static Edit* Create(UIWindow *parent, float x, float y, float width);

	int GetTextLength() const;

	void SetInt(int value);
	int  GetInt() const;

	void  SetFloat(float value);
	float GetFloat() const;

	void SetSel(int begin, int end); // -1 means end of string
	int GetSelStart() const;
	int GetSelEnd() const;
	int GetSelMin() const;
	int GetSelMax() const;
	int GetSelLength() const;

	void Paste();
	void Copy() const;

	std::function<void()> eventChange;

protected:
	void Draw(DrawingContext &dc) const override;
	bool OnChar(int c) override;
	bool OnKeyPressed(Key key) override;
	bool OnPointerDown(float x, float y, int button, PointerType pointerType, unsigned int pointerID) override;
	bool OnPointerUp(float x, float y, int button, PointerType pointerType, unsigned int pointerID) override;
	bool OnPointerMove(float x, float y, PointerType pointerType, unsigned int pointerID) override;
	bool OnFocus(bool focus) override;
	void OnEnabledChange(bool enable, bool inherited) override;
	void OnTextChange() override;
	void OnTimeStep(float dt) override;
};

} // namespace UI
