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
			ButtonState state;

			UIEvent left;
			UIEvent entered;
			UIEvent pressed;
			UIEvent released;

			Button();
			~Button();
			
			using ManagerType = ButtonManager;
			static constexpr const char* typeName = "Button";
		};

		class BLAZE_API ButtonManager : public UIElementManager<Button>
		{			
		public:
			void Update(size_t index, size_t end);

			static UIElementParsingData GetElementParsingData();
		};
	}
}