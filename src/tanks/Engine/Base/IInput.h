#pragma once
#include <list>

class Vector2;

enum class Key;
enum class MouseButton;

struct IInput
{
	virtual ~IInput() = default;

	virtual void Read() = 0;

	virtual bool GetKeyDown(Key key) const = 0;
	virtual bool GetKey(Key key) const = 0;
	virtual bool GetKeyUp(Key key) const = 0;

	virtual bool GetMouseButtonDown(MouseButton button) const = 0;
	virtual bool GetMouseButton(MouseButton button) const = 0;
	virtual bool GetMouseButtonUp(MouseButton button) const = 0;

	virtual const Vector2& GetMousePosition() const = 0;
	virtual const Vector2& GetMouseScrollOffset() const = 0;

	virtual const std::list<char>& GetCharacters() const = 0;

	virtual void Clear() = 0;
};