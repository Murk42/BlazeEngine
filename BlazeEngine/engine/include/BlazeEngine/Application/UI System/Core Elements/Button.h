#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/Application/UI System/UIElement.h"
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
		
		struct ButtonProperties
		{
			UIElementProperty<bool> clickable;
			UIElementProperty<bool> trigerable;

			UIElementProperty<UIEvent> pressed;
			UIElementProperty<UIEvent> released;
			UIElementProperty<UIEvent> entered;
			UIElementProperty<UIEvent> left;
		};

		class ButtonManager;

		class BLAZE_API Button : public UIElement
		{
			void DetachedFromManager() override;

			ButtonState state;
		public:
			bool triggerable = true;
			bool clickable = true;
			UIEvent left;
			UIEvent entered;
			UIEvent pressed;
			UIEvent released;

			Button();
			~Button();

			ButtonState GetState() const { return state; }
			
			void SetProperties(const ButtonProperties&);
			
			using ManagerType = ButtonManager;
			static constexpr const char* typeName = "Button";

			friend class ButtonManager;
		};

		class BLAZE_API ButtonManager : public UIElementManager<Button>
		{
		public:
			void Update(UIElement*) override;			
			void Serialize(UIElement*, BinaryOutputStream&) override;
			void Deserialize(UIElement*, BinaryInputStream&) override;
		};
	}
}