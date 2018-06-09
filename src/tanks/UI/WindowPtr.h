#pragma once

namespace UI
{
	class UIWindow;

	namespace detail
	{
		struct Resident;
	}

	class WindowWeakPtr
	{
	public:
		explicit WindowWeakPtr(UIWindow *p);
        WindowWeakPtr();
		~WindowWeakPtr();

		UIWindow* operator->() const;
		UIWindow* Get() const;
		void Set(UIWindow *p);

	private:
		WindowWeakPtr(const WindowWeakPtr&) = delete;
		WindowWeakPtr& operator = (const WindowWeakPtr&) = delete;
		detail::Resident *_resident;
	};

}