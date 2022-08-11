#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Application/UI System/UIElement.h"
#include "BlazeEngine/Application/UI System/UIAlignment.h"
#include "BlazeEngine/Application/UI System/UIElementManager.h"
#include "BlazeEngine/DataStructures/Rect.h"
#include <functional>

namespace Blaze
{
	namespace UI
	{
		enum class ButtonState
		{
			None,
			Hovered,
			Down,
		};

		class ButtonManager;

		class BLAZE_API Button : public UIElement
		{
		public:
			bool active;
			ButtonState state;					

			std::function<void()> left;
			std::function<void()> entered;
			std::function<void()> pressed;
			std::function<void()> released;

			Button();					
			
			using ManagerType = ButtonManager;
			static constexpr const char* typeName = "Button";
		};

		class BLAZE_API ButtonManager : public UIElementManager<Button>
		{			
		public:
			void Update(size_t index, size_t end);			
		};
	}
}