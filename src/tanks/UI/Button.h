// Button.h

#pragma once

#include "UIWindow.h"

#include <functional>

///////////////////////////////////////////////////////////////////////////////

namespace UI
{

class ButtonBase : public UIWindow
{
public:
	enum State
	{
		stateNormal,
		stateHottrack,
		statePushed,
		stateDisabled,
	};

	ButtonBase(UIWindow *parent);

	std::function<void(void)> eventClick;
	std::function<void(float, float)> eventMouseDown;
	std::function<void(float, float)> eventMouseUp;
	std::function<void(float, float)> eventMouseMove;

	State GetState() const { return _state; }

protected:
    bool OnPointerMove(float x, float y, PointerType pointerType, unsigned int pointerID) override;
    bool OnPointerDown(float x, float y, int button, PointerType pointerType, unsigned int pointerID) override;
	bool OnPointerUp  (float x, float y, int button, PointerType pointerType, unsigned int pointerID) override;
	bool OnMouseLeave() override;
    bool OnTap(float x, float y) override;

	void OnEnabledChange(bool enable, bool inherited) override;
	virtual void OnChangeState(State state);

private:
	virtual void OnClick();

	State _state;
	void SetState(State s);
};

///////////////////////////////////////////////////////////////////////////////

class Button : public ButtonBase
{
public:
	static Button* Create(UIWindow *parent, const std::string &text, float x, float y, float w=-1, float h=-1);

	void SetIcon(const char *spriteName);

protected:
	Button(UIWindow *parent);
	void SetFont(const char *fontName);

	// ButtonBase
	void OnChangeState(State state) override;

	// Window
	void Draw(DrawingContext &dc) const override;

private:
	size_t _font;
	size_t _icon;
};

///////////////////////////////////////////////////////////////////////////////

class TextButton : public ButtonBase
{
public:
	static TextButton* Create(UIWindow *parent, float x, float y, const std::string &text, const char *font);

	void SetFont(const char *fontName);

	void SetDrawShadow(bool drawShadow);
	bool GetDrawShadow() const;

protected:
	TextButton(UIWindow *parent);

	void AlignSizeToContent();

    void OnTextChange() override;
	void Draw(DrawingContext &dc) const override;


private:
	size_t _fontTexture;
	bool   _drawShadow;
};

///////////////////////////////////////////////////////////////////////////////

class ImageButton : public ButtonBase
{
public:
	static ImageButton* Create(UIWindow *parent, float x, float y, const char *texture);

protected:
	ImageButton(UIWindow *parent);
	virtual void OnChangeState(State state);
};

///////////////////////////////////////////////////////////////////////////////

class CheckBox : public ButtonBase
{
public:
	static CheckBox* Create(UIWindow *parent, float x, float y, const std::string &text);

	void SetCheck(bool checked);
	bool GetCheck() const { return _isChecked; }

	void SetDrawShadow(bool drawShadow);
	bool GetDrawShadow() const;

protected:
	CheckBox(UIWindow *parent);

	void AlignSizeToContent();

	void OnClick() override;
	void OnTextChange() override;
	void OnChangeState(State state) override;

	// Window
	void Draw(DrawingContext &dc) const override;

private:
	size_t _fontTexture;
	size_t _boxTexture;
	bool   _drawShadow;
	bool   _isChecked;
};

} // namespace UI
