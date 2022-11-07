#include "BlazeEngine/Application/UI System/Core Elements/List.h"
#include "BlazeEngine/Application/UI System/Core Elements/Text.h"

namespace Blaze::UI
{
	void List::InitializeElement(UIElement* el, uint type)
	{
		el->sizeChanged += [&]() { UpdateSize(); };
		el->SetAnchor(this);		

		uint textType = GetManager()->GetElementTypeRegistry().GetElementTypeIndex("Text");
		if (type == textType)
		{
			el->SetLocalAlignment(Align::BottomLeft);
			el->SetAnchorAlignment(Align::TopLeft);
		}
		else
		{
			el->SetLocalAlignment(Align::TopLeft);
			el->SetAnchorAlignment(Align::TopLeft);
		}

	}
	List::List()
		: elementOffset(0)
	{		
		UIElement::SetSolidFlag(false);
	}
	void List::UpdateSize()
	{
		Vec2f size;

		float lastSpacing = 0;
		for (int i = 0; i < elements.size(); ++i)
		{
			auto& el = elements[i];
			Vec2f elSize;
			Vec2f elPos;

			uint textType = GetManager()->GetElementTypeRegistry().GetElementTypeIndex("Text");

			if (el.typeIndex == textType)
			{
				Text* text = (Text*)el.ptr;

				Vec2f textSize = text->GetSize();
				float baseline = text->GetBaselineDistance();
				
				elSize = Vec2f(textSize.x, std::ceil(baseline * 0.6f));
				elPos = Vec2f(0, -size.y - elSize.y);
			}
			else
			{
				elSize = el.ptr->GetSize() + Vec2f(0, 0);
				elPos = Vec2f(0, -size.y);
			}
				
			if (lastSpacing < el.topSpacing)
				lastSpacing = el.topSpacing;

			el.ptr->SetPos(elPos + Vec2f(el.offset, -lastSpacing));

			size.x = std::max(size.x, el.offset + elSize.x);
			size.y += elSize.y + (i < elements.size() - 1 ? elementOffset : 0) + lastSpacing;

			lastSpacing = el.bottomSpacing;
		}

		size.y += lastSpacing;

		Vec2f oldSize = UIElement::GetSize();
		
		UIElement::SetSize(size);
	}

	void List::AddElement(ListElementData data, int location)
	{
		if (data.ptr == nullptr)
			return;

		if (location == -1)
			elements.emplace_back(data);
		else
			elements.insert(elements.begin() + location, data);

		InitializeElement(data.ptr, data.typeIndex);

		UpdateSize();
	}
	void List::RemoveElement(uint location)
	{
		if (location == -1)
			elements.pop_back();
		else
			elements.erase(elements.begin() + location);

		UpdateSize();
	}
	void List::SetElements(const std::vector<ListElementData>& elements)
	{
		this->elements = elements;

		for (auto el : elements)
			InitializeElement(el.ptr, el.typeIndex);

		UpdateSize();
	}

	void List::SetSize(Vec2f size)
	{		
		UIElement::SetSize({ 0, size.y });
	}

	void List::SetActiveFlag(bool active)
	{
		UIElement::SetActiveFlag(active);
	}

	void List::SetSolidFlag(bool solid)
	{
		for (auto el : elements)
			el.ptr->SetActiveFlag(solid);
	}

	void List::SetProperties(const ListProperties& p)
	{		
		SetElements(p.elements);
		SetElementOffset(p.elementsOffset);
	}
}