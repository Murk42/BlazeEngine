#include "List.h"

namespace Blaze::UI
{
	void List::UpdateSize()
	{
		Vec2f size;

		for (auto& el : elements)
		{
			Vec2f elSize;
			uint textType = GetManager()->GetElementTypeRegistry().GetElementTypeIndex("Text");
			if (el.second == textType && 0)
			{
				Text* text = (Text*)el.first;
				Vec2f textSize = text->GetSize();
				float baseline = text->GetBaselineDistance();
				elSize = Vec2f(textSize.x, baseline);
				text->SetPos(Vec2f(0, -size.y - baseline));
			}
			else
			{
				el.first->SetPos(Vec2f(0, -size.y));
				elSize = el.first->GetSize();
			}

			size.x = std::max(size.x, elSize.x);
			size.y += elSize.y + 8;
		}

		Vec2f oldSize = UIElement::GetSize();

		if (customSize == -1)
			UIElement::SetSize(size);
		else
			UIElement::SetSize({ customSize, size.y });
	}

	void List::AddElement(UIElement* el, uint type)
	{
		if (el == nullptr)
			return;

		elements.emplace_back(el, type);

		el->sizeChanged += [&]() { UpdateSize(); };
		el->SetAnchor(this);
		
		uint textType = GetManager()->GetElementTypeRegistry().GetElementTypeIndex("Text");
		if (type == textType && 0)
		{
			el->SetLocalAlignment(Align::BottomLeft);
			el->SetAnchorAlignment(Align::TopLeft);
		}
		else
		{
			el->SetLocalAlignment(Align::TopLeft);
			el->SetAnchorAlignment(Align::TopLeft);
		}

		UpdateSize();
	}

	void List::SetRect(Rectf rect)
	{
		customSize = rect.w;
		UIElement::SetPos(rect.pos);
		UIElement::SetSize({ customSize, rect.h });
	}

	void List::SetSize(Vec2f size)
	{
		customSize = size.x;
		UIElement::SetSize({ customSize, size.y });
	}

	void List::SetActiveFlag(bool active)
	{
		for (auto el : elements)
			el.first->SetActiveFlag(active);

		UIElement::SetActiveFlag(active);
	}

	UIElementParsingData ListManager::GetElementParsingData()
	{
		UIElementParsingData data = UIBaseElementManager::GetElementParsingData();

		data.AddMember<List, std::vector<String>>("elements", 
			[&](UIScene& scene, List& list, const std::vector<String>& names) {
				for (int i = 0; i < names.size(); ++i)
				{
					auto el = scene.GetElement(names[i]);					
					list.AddElement(el.first, el.second);
				}
			}
		, nullptr);

		return data;
	}
}