#pragma once
#include <vector>
#include <functional>

namespace Blaze::UI
{
	class UIManager;

	using UIEventFunction = std::function<void()>;

	class BLAZE_API UIEvent
	{
		std::vector<UIEventFunction> functions;
	public:
		UIEvent();
		UIEvent(const UIEvent&);
		UIEvent(UIEvent&&) noexcept;
		UIEvent(UIEventFunction const&);
		UIEvent(std::initializer_list<UIEventFunction> const&);
		~UIEvent();

		void AddFunction(const UIEventFunction& func);
		UIEvent& operator+=(const UIEventFunction& func);
		UIEvent& operator+=(const UIEvent& event);		
		
		UIEvent& operator=(const UIEvent& event);
		UIEvent& operator=(UIEvent&& event) noexcept;

		friend class UIManager;
	};
}