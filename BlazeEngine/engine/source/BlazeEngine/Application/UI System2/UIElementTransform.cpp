#include "BlazeEngine/Application/UI System2/UIElementTransform.h"
#include "BlazeEngine/Application/UI System2/UIElement.h"
#include "UIElmenetConstructData.h"

namespace Blaze::UI2
{
	UIElementTransform::UIElementTransform()
		: anchor(nullptr), localAlignment(Align::Center), anchorAlignment(Align::Center), scene(nullptr), updateState(0)
	{
		scene = uiElementConstructData.scene;			

		if (uiElementConstructData.state == 1)
		{
			uiElementConstructData.state = 2;
		}
		else
		{
			Logger::ProcessLog(BLAZE_WARNING_LOG("Blaze Engine",
				"Internal engine error while constructing UIElementTransform. uiElementConstructData.state was changed inappropriately"));
		}
	}

	void UIElementTransform::SetPos(Vec2f pos)
	{
		this->pos = pos;
	}

	void UIElementTransform::SetSize(Vec2f size)
	{
		this->size = size;
	}

	void UIElementTransform::SetLocalAlignment(Align align)
	{ 
		this->localAlignment = align;
	}

	void UIElementTransform::SetAnchorAlignment(Align align)
	{
		this->anchorAlignment = align;
	}

	void UIElementTransform::SetAnchor(UIElementTransform* anchor)
	{
		this->anchor = anchor;
	}

	Rectf RectIntersection(Rectf a, Rectf b)
	{
		Vec2f a1 = a.pos;
		Vec2f a2 = a1 + a.size;

		Vec2f b1 = b.pos;
		Vec2f b2 = b1 + b.size;

		a1.x = std::clamp(b1.x, a1.x, a2.x);
		a1.y = std::clamp(b1.y, a1.y, a2.y);
		a2.x = std::clamp(b2.x, a1.x, a2.x);
		a2.y = std::clamp(b2.y, a1.y, a2.y);

		return { a1, a2 - a1 };
	}

	void UIElementTransformSystem::ResolveClipRect(UIElementTransform* transform)
	{
		//if (element->clipElement != nullptr)
		//	element->clipRect = { element->clipElement->GetViewportPos(), element->clipElement->GetSize() };
		//else
		//	element->clipRect = { Vec2f(), (Vec2f)viewport.size };
		//
		//element->clipRect = RectIntersection(element->clipRect, { Vec2f(), (Vec2f)viewport.size });
	}
	void UIElementTransformSystem::ResolveAlignedRect(UIElementTransform* transform)
	{
		Vec2f size;
		Vec2f offset;

		if (transform->anchor != nullptr)
		{
			size = transform->anchor->size;
			offset = transform->anchor->viewportPos;
		}
		else
		{			
			size = (Vec2f)transform->scene->GetViewport().size;
			offset = Vec2f();
		}

		switch (transform->anchorAlignment)
		{
		case Blaze::Align::Top:			offset += Vec2f(size.x / 2, size.y); break;
		case Blaze::Align::TopRight:	offset += Vec2f(size.x, size.y); break;
		case Blaze::Align::Right:		offset += Vec2f(size.x, size.y / 2); break;
		case Blaze::Align::BottomRight:	offset += Vec2f(size.x, 0); break;
		case Blaze::Align::Bottom:		offset += Vec2f(size.x / 2, 0); break;
		case Blaze::Align::BottomLeft:	offset += Vec2f(0, 0); break;
		case Blaze::Align::Left:		offset += Vec2f(0, size.y / 2); break;
		case Blaze::Align::TopLeft:		offset += Vec2f(0, size.y); break;
		case Blaze::Align::Center:		offset += Vec2f(size.x / 2, size.y / 2); break;
		}

		size = transform->size;

		switch (transform->localAlignment)
		{
		case Blaze::Align::Top:			offset -= Vec2f(size.x / 2, size.y); break;
		case Blaze::Align::TopRight:	offset -= Vec2f(size.x, size.y); break;
		case Blaze::Align::Right:		offset -= Vec2f(size.x, size.y / 2); break;
		case Blaze::Align::BottomRight:	offset -= Vec2f(size.x, 0); break;
		case Blaze::Align::Bottom:		offset -= Vec2f(size.x / 2, 0); break;
		case Blaze::Align::BottomLeft:	offset -= Vec2f(0, 0); break;
		case Blaze::Align::Left:		offset -= Vec2f(0, size.y / 2); break;
		case Blaze::Align::TopLeft:		offset -= Vec2f(0, size.y); break;
		case Blaze::Align::Center:		offset -= Vec2f(size.x / 2, size.y / 2); break;
		}

		transform->viewportPos = transform->pos + offset;
		transform->viewportPos.x = std::round(transform->viewportPos.x);
		transform->viewportPos.y = std::round(transform->viewportPos.y);
	}
	void UIElementTransformSystem::CleanTransform(UIElementTransform* transform)
	{
		if (transform->updateState == updateState)
			return;		

		if (transform->anchor != nullptr)
		{
			CleanTransform(transform->anchor);
			//element->flags.set(1, element->anchor->IsActive());
		}
		//else
			//element->flags.set(1, true);

		//if (element->clipElement != nullptr)
		//	CleanElement(element->clipElement);

		//Rectf oldClipRect = element->clipRect;
		Vec2f oldViewportPos = transform->viewportPos;

		ResolveClipRect(transform);
		ResolveAlignedRect(transform);

		//if (oldClipRect != element->clipRect)
		//	element->clipRectChangedEvent.Call({ element });
		//if (oldViewportPos != element->viewportPos)
		//	element->viewportPosChangedEvent.Call({ element });

		transform->updateState = updateState;

		//for (auto tied : element->tiedElements)
		//	CleanElement(tied);

	}
	void UIElementTransformSystem::Update(const ECS::ComponentContainer& components)
	{
		updateState++;

		for (auto component : components)
		{
			auto* transform = component->Cast<UIElementTransform>();			
			CleanTransform(transform);
		}		
	}
}