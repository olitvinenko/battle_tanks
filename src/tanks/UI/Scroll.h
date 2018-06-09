// Scroll.h

#pragma once

#include "UIWindow.h"

namespace UI
{

class ImageButton;

class ScrollBarBase : public UIWindow
{
public:
	void SetShowButtons(bool showButtons);
	bool GetShowButtons() const;

	virtual void SetSize(float size) = 0;
	virtual float GetSize() const = 0;

	virtual void SetPos(float pos);
	float GetPos() const;

	void  SetDocumentSize(float limit);
	float GetDocumentSize() const;

	void  SetLineSize(float ls);
	float GetLineSize() const;

	void  SetPageSize(float ps);
	float GetPageSize() const;

	void SetElementTextures(const char *slider, const char *upleft, const char *downright);

	std::function<void(float)> eventScroll;

protected:
	ScrollBarBase(UIWindow *parent);

	virtual void OnEnabledChange(bool enable, bool inherited);
	virtual float Select(float x, float y) const = 0;
	float GetScrollPaneLength() const;

	float _tmpBoxPos;
	ImageButton *_btnBox;
	ImageButton *_btnUpLeft;
	ImageButton *_btnDownRight;

private:
	virtual void OnSize(float width, float height);

	void OnBoxMouseDown(float x, float y);
	void OnBoxMouseUp(float x, float y);
	void OnBoxMouseMove(float x, float y);

	void OnUpLeft();
	void OnDownRight();

	void OnLimitsChanged();

	float _pos;
	float _lineSize;
	float _pageSize;
	float _documentSize;

	bool _showButtons;
};

///////////////////////////////////////////////////////////////////////////////

class ScrollBarVertical : public ScrollBarBase
{
public:
	static ScrollBarVertical* Create(UIWindow *parent, float x, float y, float height);

	virtual void SetSize(float size);
	virtual float GetSize() const;

	virtual void SetPos(float pos);

protected:
	ScrollBarVertical(UIWindow *parent);
	virtual float Select(float x, float y) const { return y; }
};

///////////////////////////////////////////////////////////////////////////////

class ScrollBarHorizontal : public ScrollBarBase
{
public:
	static ScrollBarHorizontal* Create(UIWindow *parent, float x, float y, float width);

	virtual void SetSize(float size);
	virtual float GetSize() const;

	virtual void SetPos(float pos);

protected:
	ScrollBarHorizontal(UIWindow *parent);
	virtual float Select(float x, float y) const { return x; }
};


///////////////////////////////////////////////////////////////////////////////
} // end of namespace UI

// end of file
