#include "BlazeEngine/Application/UI System2/UIElement.h"
#include "BlazeEngine/Application/UI System2/UIElementTransform.h"
#include "BlazeEngine/Graphics/Graphics.h"

#include "UIElmenetConstructData.h"

namespace Blaze::UI2
{
	UIElementConstructData uiElementConstructData;

	UIElement::UIElement() : 
		transform(uiElementConstructData.transform), 
		scene(uiElementConstructData.scene),
		layerIt(uiElementConstructData.layerIt),
		layerElementIt(uiElementConstructData.layerElementIt),
		name(uiElementConstructData.name)	
	{
		if (uiElementConstructData.state == 2)
		{
			uiElementConstructData.state = 3;
		}
		else if (uiElementConstructData.state == 0)
		{
			Logger::ProcessLog(BLAZE_WARNING_LOG("Blaze Engine",
				"Creating a UIElement outside of a UIScene. It may not function properly. Or there was an internal engine error."));
		}
		else
		{
			Logger::ProcessLog(BLAZE_WARNING_LOG("Blaze Engine",
				"Internal engine error. uiElementConstructData.state was changed inappropriately"));
		}
	}	
	void UIElementSystem::Update(ECS::Component* component)
	{
		auto* element = component->Cast<UIElement>();
		auto* transform = element->transform;

		Vec2f pos = transform->GetViewportPos();
		Vec2f size = transform->GetSize();

		Graphics::DrawBoxOutline2D(pos, pos + size, 0xffffffff, 2);
	}
}