#include "BlazeEngine/Application/UI System2/Components/UIElementComponent.h"
#include "BlazeEngine/Application/UI System2/UIScene.h"

namespace Blaze::UI2::Components
{
#define ACTIVE 0
#define ANCHOR_ACTIVE 1
#define SOLID 2
	
	UIElementComponent::UIElementComponent(ECS::ComponentConstructData data)
		: Component(data),
		updateState(0), flags(0b01111), clipRect(0, 0, NAN, NAN),
		name(), pos(), size(100, 100), anchor(nullptr), localAlignment(Align::Center), anchorAlignment(Align::Center),
		depth(0.0f), clipElement(nullptr), scene(nullptr)
	{		
	}

	UIElementComponent::~UIElementComponent()
	{		
	}

	Result UIElementComponent::ChangeLayer(StringView newLayer)
	{
		Result result;

		if (scene != nullptr)
		{			
			auto layerMapIt = scene->layerNames.find(newLayer);

			if (layerMapIt == scene->layerNames.end())
			{
				result += BLAZE_WARNING_RESULT("Blaze Engine", "Invalid layer name while chaning elements layer, the new layer name wasnt found inside the scene");
				return result;
			}

			if (layerIt == decltype(layerIt)())
				result += BLAZE_WARNING_RESULT("Blaze Engine", "Trying to change elements layer but the element wasnt assigned to a layer");
			else
			{
				auto& elements = layerIt->elements;
				elements.erase(layerElementIt);								
			}			
	
			layerIt = layerMapIt->second;
			layerElementIt = layerIt->elements.emplace(layerIt->elements.end(), this);			 
		}		

		layerChangedEvent.Call({ this });

		return result;
	}

	Result UIElementComponent::SetName(String newName)
	{				
		Result result;

		if (scene != nullptr)
		{
			String oldName = std::move(name);
			name = newName;
		
			auto old = scene->nameToElementMap.find(oldName);

			if (old != scene->nameToElementMap.end())
				scene->nameToElementMap.erase(old);
			else if (oldName.Size() > 0)
				result += BLAZE_WARNING_RESULT("Blaze Engine", "Element name not found in scene while renaming a element");

			scene->nameToElementMap.insert({ newName, GetEntity() });
		}
		else
			name = newName;		

		nameChangedEvent.Call({ this });

		return result;
	}
	void UIElementComponent::SetPos(Vec2f pos)
	{
		this->pos = pos;
		updateState = 0;
	}

	void UIElementComponent::SetSize(Vec2f size)
	{
		Vec2f old = this->size;
		this->size = size;
		updateState = 0;
		//if (old != size && manager != nullptr)
		//	manager->AddEventToQueue(sizeChanged);

		sizeChangedEvent.Call({ this });
	}

	void UIElementComponent::SetDepth(float depth)
	{
		this->depth = depth;

		depthChangedEvent.Call({ this });
	}
	void UIElementComponent::SetLocalAlignment(Align align)
	{
		this->localAlignment = align;

		localAlignmentChangedEvent.Call({ this });
	}
	void UIElementComponent::SetAnchorAlignment(Align align)
	{
		this->anchorAlignment = align;

		anchorAlignmentChangedEvent.Call({ this });
	}
	void UIElementComponent::SetAnchor(UIElementComponent* anchor)
	{
		this->anchor = anchor;

		anchorChangedEvent.Call({ this });
	}
	void UIElementComponent::SetClipElement(UIElementComponent* clipElement)
	{
		this->clipElement = clipElement;

		clipElementChangedEvent.Call({ this });
	}

	void UIElementComponent::SetSolidFlag(bool solid)
	{
		flags.set(SOLID, solid);

		solidFlagChangedEvent.Call({ this });
	}
	void UIElementComponent::SetActiveFlag(bool active)
	{
		flags.set(ACTIVE, active);

		activeFlagChangedEvent.Call({ this });
	}
	bool UIElementComponent::IsActive() const
	{
		return flags.test(ACTIVE) && flags.test(ANCHOR_ACTIVE);
	}	
	Rectf UIElementComponent::GetClipRect() const
	{
		return clipRect;
	}
	bool UIElementComponent::IsSolid() const
	{
		return flags.test(SOLID);
	}
}