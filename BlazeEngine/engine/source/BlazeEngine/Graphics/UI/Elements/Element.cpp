#include "BlazeEngine/Graphics/UI/Elements/Element.h"
#include "BlazeEngine/Graphics/UI/Elements/ElementManager.h"
#include "BlazeEngine/Graphics/UI/Layer.h"
#include "BlazeEngine/Math/Math.h"

#include "source/BlazeEngine/Internal/Engine.h"

namespace Blaze::UI
{
	extern LayerManager* mainLayerManager;

	Element::Element()
		: parent(nullptr), manager(nullptr)
	{		
	}	
	void Element::SetParent(Element& parent)
	{
		parent.children.emplace_back(this);
		this->parent = &parent;
	}
	void Element::UpdateTransform()
	{
		Vec2f offset;
		Vec2f parentAbsoluteSize;
		Vec2f parentAbsolutePos;

		transform.absoluteSize = transform.size * transform.scale;

		if (parent != nullptr)
		{
			parentAbsoluteSize = parent->transform.absoluteSize;
			parentAbsolutePos = parent->transform.absolutePos;
		}
		else
		{
			parentAbsoluteSize = engine->UI.viewportSize;
			parentAbsolutePos = Vec2i();
		}

		offset += parentAbsolutePos;

		switch (transform.parentAlign)
		{
		case Align::Top:		 offset += Vec2f(parentAbsoluteSize.x / 2, parentAbsoluteSize.y); break;
		case Align::TopRight:	 offset += Vec2f(parentAbsoluteSize.x, parentAbsoluteSize.y); break;
		case Align::Right:		 offset += Vec2f(parentAbsoluteSize.x, parentAbsoluteSize.y / 2); break;
		case Align::BottomRight: offset += Vec2f(parentAbsoluteSize.x, 0); break;
		case Align::Bottom:		 offset += Vec2f(parentAbsoluteSize.x / 2, 0); break;
		case Align::BottomLeft:	 offset += Vec2f(0, 0); break;
		case Align::Left:		 offset += Vec2f(0, parentAbsoluteSize.y / 2); break;
		case Align::TopLeft:	 offset += Vec2f(0, parentAbsoluteSize.y); break;
		case Align::Center:		 offset += Vec2f(parentAbsoluteSize.x / 2, parentAbsoluteSize.y / 2); break;
		}

		switch (transform.align)
		{
		case Align::Top:		 offset -= Vec2f(transform.absoluteSize.x / 2, transform.absoluteSize.y); break;
		case Align::TopRight:	 offset -= Vec2f(transform.absoluteSize.x, transform.absoluteSize.y); break;
		case Align::Right:		 offset -= Vec2f(transform.absoluteSize.x, transform.absoluteSize.y / 2); break;
		case Align::BottomRight: offset -= Vec2f(transform.absoluteSize.x, 0); break;
		case Align::Bottom:		 offset -= Vec2f(transform.absoluteSize.x / 2, 0); break;
		case Align::BottomLeft:	 offset -= Vec2f(0, 0); break;
		case Align::Left:		 offset -= Vec2f(0, transform.absoluteSize.y / 2); break;
		case Align::TopLeft:	 offset -= Vec2f(0, transform.absoluteSize.y); break;
		case Align::Center:		 offset -= Vec2f(transform.absoluteSize.x / 2, transform.absoluteSize.y / 2); break;
		}

		transform.absolutePos = offset + transform.pos;
		transform.mat = Math::TranslationMatrix<float>(Vec3f(transform.absolutePos.x, transform.absolutePos.y, transform.depth)) * Math::ScalingMatrix<float>(Vec3f(transform.absoluteSize.x, transform.absoluteSize.y, 1.0f));
		if (TransformUpdate)
			TransformUpdate();
	}
}