#pragma once
#include "Window.h"
#include <memory>

namespace UI
{
	class ScrollView
		: public Window
		, private ScrollSink
	{
	public:
		ScrollView(LayoutManager &manager);

		void SetContent(std::shared_ptr<Window> content);
		void SetVerticalScrollEnabled(bool enabled) { _verticalScrollEnabled = enabled; }
		void SetHorizontalScrollEnabled(bool enabled) { _horizontalScrollEnabled = enabled; }

		// Window
		ScrollSink* GetScrollSink() override { return this; }
		RectFloat GetChildRect(TextureManager &texman, const LayoutContext &lc, const StateContext &sc, const Window &child) const override;
		Vector2 GetContentSize(TextureManager &texman, const StateContext &sc, float scale) const override;

	private:
		std::shared_ptr<Window> _content;
		Vector2 _offset = {};
		bool _verticalScrollEnabled = true;
		bool _horizontalScrollEnabled = false;

		// ScrollSink
		void OnScroll(TextureManager &texman, const UI::InputContext &ic, const LayoutContext &lc, const StateContext &sc, Vector2 pointerPosition, Vector2 offset) override;
	};

}// namespace UI
