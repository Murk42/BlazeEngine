#include "BlazeEngine/Application/UI System/UIElement.h"

namespace Blaze::UI
{
	UIElement::UIElement()
		: depth(0), clickable(false), anchorAlignment(Align::Center), localAlignment(Align::Center), anchor(nullptr), updateState(0),
		rect(0, 0, 100, 100), active(true)
	{
	}

	UIElement::~UIElement()
	{
	}

	void UIElement::SetPos(Vec2f pos)
	{
		rect.pos = pos;
		updateState = 0;
	}

	void UIElement::SetSize(Vec2f size)
	{
		Vec2f old = rect.size;
		rect.size = size;
		updateState = 0;
		if (old != size)
			sizeChanged();
	}

	void UIElement::SetRect(Rectf rect)
	{
		Vec2f old = this->rect.size;
		this->rect = rect;
		updateState = 0;

		if (old != rect.size)
			sizeChanged();
	}

	void UIElement::SetDepth(float depth)
	{
		this->depth = depth;
	}
	void UIElement::SetClickableFlag(bool clickable)
	{
		this->clickable = clickable;
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
	void UIElement::SetActiveFlag(bool active)
	{
		this->active = active;
	}
}