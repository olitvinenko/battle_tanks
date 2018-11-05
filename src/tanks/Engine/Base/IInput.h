#pragma once

class Vector2;

enum class Key;
enum class MouseButton;

struct IInputListener
{
	virtual ~IInputListener() = default;

	virtual void OnMouseButtonDown(MouseButton button) { }
	virtual void OnMouseButtonUp(MouseButton button) { }

	virtual void OnMousePosition(float x, float y) { }
	virtual void OnMouseScrollOffset(float x, float y) { }

	virtual void OnCharacter(char character) { }

	virtual void OnKeyDown(Key key) { }
	virtual void OnKey(Key key) { }
	virtual void OnKeyUp(Key key) { }
};

struct IInput
{
	virtual ~IInput() = default;

	virtual void Read() = 0;

	virtual void AddListener(IInputListener *listener) = 0;
	virtual void RemoveListener(IInputListener *listener) = 0;

	virtual bool GetKey(Key key) const = 0;
	virtual bool GetMouseButton(MouseButton button) const = 0;
	virtual const Vector2& GetMousePosition() const = 0;

	virtual void Clear() = 0;
};