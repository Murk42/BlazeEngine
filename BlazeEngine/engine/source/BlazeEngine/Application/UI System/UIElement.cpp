#include "BlazeEngine/Application/UI System/UIElement.h"
#include "BlazeEngine/Application/UI System/UIScene.h"

namespace Blaze::UI
{
#define ACTIVE 0
#define ANCHOR_ACTIVE 1
#define SOLID 2

	//Result UIElement::TieElement(UIElement* element, uint typeIndex)
	//{		
	//	if (manager == nullptr)	
	//		return BLAZE_ERROR_LOG("BlazeEngine", "No element type registry set");
	//
	//	element->flags.set(MANUAL, true);
	//
	//	element->manager = manager;
	//	element->layer = "";
	//	element->typeIndex = typeIndex;
	//
	//	manager->elements.emplace_back(element);
	//
	//	element->AttachedToManager();
	//	tiedElements.emplace_back(element);
	//}
	//Result UIElement::UntieElement(UIElement* element)
	//{
	//	element->DetachedFromManager();
	//
	//	if (element == manager->focusedElement)
	//		manager->focusedElement = nullptr;
	//	
	//	tiedElements.erase(std::find(tiedElements.begin(), tiedElements.end(), element));
	//
	//	manager->elements.erase(std::find(manager->elements.begin(), manager->elements.end(), element));
	//
	//	element->layer = "";
	//	element->manager = nullptr;
	//	element->flags.set(MANUAL, false);
	//}	
	Result UIElement::TieElement(UIElement* element)
	{		
		element->manager = manager;
		element->layer = "";
		element->typeIndex = -1;
		element->tiedParent = this;

		tiedElements.Resize(tiedElements.Count() + 1);
		tiedElements.Last() = element;

		return { };
	}
	Result UIElement::UntieElement(UIElement* element)
	{
		auto it = std::find(tiedElements.begin(), tiedElements.end(), element);
		size_t index = it - tiedElements.Ptr();

		if (it == tiedElements.end())
			return BLAZE_ERROR_RESULT("BlazeEngine", "Trying to untie a element from a element that it isnt tied to");
		
		Array<UIElement*> newArray(tiedElements.Count() - 1);
		for (int i = 0; i < index; ++i)
			newArray[i] = tiedElements[i];
		for (int i = index; i < newArray.Count(); ++i)
			newArray[i] = tiedElements[i + 1];
		tiedElements = std::move(newArray);
		
		element->manager = nullptr;
		element->tiedParent = nullptr;

		return { };
	}
	UIElement::UIElement()
		: updateState(0), layer(), manager(nullptr), scene(nullptr), flags(0b01111), typeIndex(-1), clipRect(0, 0, NAN, NAN),
		name(), pos(), size(100, 100), anchor(nullptr), localAlignment(Align::Center), anchorAlignment(Align::Center),
		depth(0.0f), clipElement(nullptr), tiedParent(nullptr)
	{
		SetProperties({ });
	}

	UIElement::~UIElement()
	{
		if (tiedParent != nullptr)
			tiedParent->UntieElement(this);
		else if (manager != nullptr)
			manager->RemoveElement(this);
	}

	Result UIElement::ChangeLayer(StringView newLayer)
	{
		if (manager == nullptr)
			return BLAZE_ERROR_RESULT("BlazeEngine", "Trying to change a layer of a element that doesnt belong to a manager");
		
		auto& elements = manager->layerNames[layer]->elements;

		auto it = std::find(elements.begin(), elements.end(), this);
		elements.erase(it);

		auto layerIT = manager->layerNames[newLayer];
		layer = layerIT->name;
		layerIT->elements.emplace_back(this);

		return { };
	}

	void UIElement::SetName(String newName)
	{
		if (scene != nullptr)
		{
			String oldName = std::move(name);
			name = newName;

			auto old = scene->nameToElementMap.find(oldName);
			if (old != scene->nameToElementMap.end())			
				scene->nameToElementMap.erase(old);			
			scene->nameToElementMap.insert({ newName, this });
		}
		else
			name = newName;
	}
	void UIElement::SetPos(Vec2f pos)
	{
		this->pos = pos;
		updateState = 0;
	}

	void UIElement::SetSize(Vec2f size)
	{
		Vec2f old = this->size;
		this->size = size;
		updateState = 0;
		if (old != size && manager != nullptr)
			manager->AddEventToQueue(sizeChanged);
	}	

	void UIElement::SetDepth(float depth)
	{
		this->depth = depth;
	}
	void UIElement::SetLocalAlignment(Align align)
	{
		this->localAlignment = align;
	}
	void UIElement::SetAnchorAlignment(Align align)
	{
		this->anchorAlignment = align;
	}
	void UIElement::SetAnchor(UIElement* anchor)
	{
		this->anchor = anchor;
	}	
	void UIElement::SetClipElement(UIElement* clipElement)
	{
		this->clipElement = clipElement;
	}

	void UIElement::SetSolidFlag(bool solid)
	{		
		flags.set(SOLID, solid);		
	}
	void UIElement::SetActiveFlag(bool active)
	{		
		flags.set(ACTIVE, active);
	}
	bool UIElement::IsActive() const 
	{ 
		return flags.test(ACTIVE) && flags.test(ANCHOR_ACTIVE);
	}
	void UIElement::SetProperties(const UIElementProperties& p)
	{
		if (p.name.edited) SetName(p.name);
		if (p.pos.edited) SetPos(p.pos);
		if (p.size.edited) SetSize(p.size);
		if (p.depth.edited) SetDepth(p.depth);
		if (p.localAlign.edited) SetLocalAlignment(p.localAlign);
		if (p.anchorAlign.edited) SetAnchorAlignment(p.anchorAlign);
		if (p.anchor.edited) SetAnchor(p.anchor);
		if (p.active.edited) SetActiveFlag(p.active);
		if (p.solid.edited) SetSolidFlag(p.solid);
		if (p.clipElement.edited) SetClipElement(p.clipElement);
	}
	Rectf UIElement::GetClipRect() const
	{
		return clipRect;
	}
	bool UIElement::IsSolid() const 
	{ 
		return flags.test(SOLID);
	}	
}