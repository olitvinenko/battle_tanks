#pragma once

#include <cassert>
#include <functional>
#include <string>
#include <list>
#include "Rendering/Color.h"

class DrawingContext;
enum class Key;

namespace UI
{
class LayoutManager;
class UIWindow;

enum class PointerType;
    
namespace detail
{
	struct Resident
	{
		unsigned int counter;
		UIWindow *ptr;
		Resident(UIWindow *p) : counter(0), ptr(p) {}
	};
}

enum class StretchMode
{
	Stretch,
	Fill,
};

class UIWindow
{
	friend class WindowWeakPtr;
	detail::Resident *_resident;

	friend class LayoutManager;
	LayoutManager &_manager;

	UIWindow* _parent;
	UIWindow* _firstChild;
	UIWindow* _lastChild;
	UIWindow* _prevSibling;
	UIWindow* _nextSibling;


	std::list<UIWindow*>::iterator _timeStepReg;


	//
	// size and position
	//

	float _x;
	float _y;
	float _width;
	float _height;


	//
	// attributes
	//

	std::string     _text;

	Color  _backColor;
	Color  _borderColor;
	size_t       _texture;
	StretchMode  _textureStretchMode;
	unsigned int _frame;


	bool _isVisible      : 1;
	bool _isEnabled      : 1;
	bool _isTopMost      : 1;
	bool _isTimeStep     : 1;
	bool _drawBorder     : 1;
	bool _drawBackground : 1;
	bool _clipChildren   : 1;

protected:
	unsigned int GetFrameCount() const;
	unsigned int GetFrame() const { return _frame; }
	void SetFrame(unsigned int n) { assert(-1 == _texture || n < GetFrameCount()); _frame = n; }

	void OnEnabledChangeInternal(bool enable, bool inherited);
	void OnVisibleChangeInternal(bool visible, bool inherited);

protected:
	explicit UIWindow(UIWindow *parent, LayoutManager *manager = nullptr);
	virtual ~UIWindow(); // delete via Destroy() only

public:
	static UIWindow* Create(UIWindow *parent);
	void Destroy();

	UIWindow* GetParent()      const { return _parent;      }
	UIWindow* GetPrevSibling() const { return _prevSibling; }
	UIWindow* GetNextSibling() const { return _nextSibling; }
	UIWindow* GetFirstChild()  const { return _firstChild;  }
	UIWindow* GetLastChild()   const { return _lastChild;   }
	LayoutManager& GetManager() const { return _manager;  }

	bool Contains(const UIWindow *other) const;


	//
	// Appearance
	//

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


	//
	// size & position
	//

	void Move(float x, float y);
	float GetX() const { return _x; }
	float GetY() const { return _y; }

	void Resize(float width, float height);
	float GetWidth()  const { return _width;  }
	float GetHeight() const { return _height; }


	//
	// Behavior
	//

	void SetEnabled(bool enable);
    bool GetEnabled() const { return _isEnabled; }
	bool GetEnabledCombined() const;

	void SetTimeStep(bool enable);
	bool GetTimeStep() const { return _isTimeStep; }


	//
	// Events
	//
	std::function<void(void)> eventLostFocus;


	//
	// rendering
	//

	virtual void Draw(DrawingContext &dc) const;

private:

	//
	// pointer handlers
	//

	virtual bool OnPointerDown(float x, float y, int button, PointerType pointerType, unsigned int pointerID);
	virtual bool OnPointerUp(float x, float y, int button, PointerType pointerType, unsigned int pointerID);
	virtual bool OnPointerMove(float x, float y, PointerType pointerType, unsigned int pointerID);
	virtual bool OnMouseWheel(float x, float y, float z);
	virtual bool OnMouseEnter(float x, float y);
	virtual bool OnMouseLeave();
	virtual bool OnTap(float x, float y);


	//
	// keyboard handlers
	//

	virtual bool OnChar(int c);
	virtual bool OnKeyPressed(Key key);


	//
	// size & position handlers
	//

	virtual void OnMove(float x, float y);
	virtual void OnSize(float width, float height);


	//
	// other
	//

	virtual void OnTextChange();
	virtual bool OnFocus(bool focus); // return true if the window took focus
	virtual void OnEnabledChange(bool enable, bool inherited);
	virtual void OnVisibleChange(bool visible, bool inherited);
	virtual void OnTimeStep(float dt);
};

} // namespace UI

