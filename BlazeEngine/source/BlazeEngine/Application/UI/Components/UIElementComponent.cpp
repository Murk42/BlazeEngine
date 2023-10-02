#include "pch.h"
#include "BlazeEngine/Application/UI/Components/UIElementComponent.h"
#include "BlazeEngine/Application/UI/Components/UITransformComponent.h"
#include "BlazeEngine/Application/UI/UIComponent.h"

#include "UIElmenetCreationData.h"

namespace Blaze::UI::Components
{
	UIElementComponent::UIElementComponent() 
		: scene(nullptr),
		layerIt(), layerElementIt(),
		name()
	{	
		if (currentUIElementCreationData->scene != nullptr)
		{			
			scene = currentUIElementCreationData->scene;
			layerIt = currentUIElementCreationData->layerIt;
			layerElementIt = currentUIElementCreationData->layerElementIt;
			name = std::move(currentUIElementCreationData->name);

			currentUIElementCreationData->scene = nullptr;
			
			for (uint i = 2; i < GetEntity()->GetComponentCount(); ++i)
			{
				auto& tags = GetEntity()->GetComponentTypeData(i)->GetTypeTags();

				if (tags.Contains("UIComponent"))
					uiComponents.AddBack((UIComponent*)GetEntity()->GetComponent(i));				
			}			
		}
	}		
}