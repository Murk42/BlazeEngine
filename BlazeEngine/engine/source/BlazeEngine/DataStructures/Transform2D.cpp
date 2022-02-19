#include "BlazeEngine/DataStructures/Transform2D.h"
#include "BlazeEngine/Graphics/Renderer.h"

namespace Blaze
{
	Transform2D::Transform2D()
		: pos(0), depth(0), size(0), scale(1.0f), align(Align::Center), parentAlign(Align::Center), parent(nullptr)
	{

	}	

	void Transform2D::Update()
	{
		Vec2f offset;
		Vec2f parentAbsoluteSize;
		Vec2f parentAbsolutePos;

		absoluteSize = size * scale;

		if (parent != nullptr)
		{
			parentAbsoluteSize = parent->absoluteSize;
			parentAbsolutePos = parent->absolutePos;
		}
		else
		{
			parentAbsoluteSize = Vec2f(Renderer::GetViewportSize());
			parentAbsolutePos = Vec2f();
		}
		
		offset += parentAbsolutePos;					

		switch (parentAlign)
		{
		case Align::Top:		 offset += Vec2f(parentAbsoluteSize.x / 2	, parentAbsoluteSize.y		); break;
		case Align::TopRight:	 offset += Vec2f(parentAbsoluteSize.x		, parentAbsoluteSize.y		); break;
		case Align::Right:		 offset += Vec2f(parentAbsoluteSize.x		, parentAbsoluteSize.y / 2	); break;
		case Align::BottomRight: offset += Vec2f(parentAbsoluteSize.x		, 0								); break;
		case Align::Bottom:		 offset += Vec2f(parentAbsoluteSize.x / 2	, 0								); break;
		case Align::BottomLeft:	 offset += Vec2f(0							, 0								); break;
		case Align::Left:		 offset += Vec2f(0							, parentAbsoluteSize.y / 2	); break;
		case Align::TopLeft:	 offset += Vec2f(0							, parentAbsoluteSize.y		); break;
		case Align::Center:		 offset += Vec2f(parentAbsoluteSize.x / 2	, parentAbsoluteSize.y / 2	); break;
		}
		
		switch (align)
		{
		case Align::Top:		 offset -= Vec2f(absoluteSize.x / 2, absoluteSize.y); break;
		case Align::TopRight:	 offset -= Vec2f(absoluteSize.x, absoluteSize.y); break;
		case Align::Right:		 offset -= Vec2f(absoluteSize.x, absoluteSize.y / 2); break;
		case Align::BottomRight: offset -= Vec2f(absoluteSize.x, 0); break;
		case Align::Bottom:		 offset -= Vec2f(absoluteSize.x / 2, 0); break;
		case Align::BottomLeft:	 offset -= Vec2f(0, 0); break;
		case Align::Left:		 offset -= Vec2f(0, absoluteSize.y / 2); break;
		case Align::TopLeft:	 offset -= Vec2f(0, absoluteSize.y); break;
		case Align::Center:		 offset -= Vec2f(absoluteSize.x / 2, absoluteSize.y / 2); break;
		}

		absolutePos = offset + pos;			
		mat = Math::TranslationMatrix<float>(Vec3f(absolutePos.x, absolutePos.y, depth)) * Math::ScalingMatrix<float>(Vec3f(absoluteSize.x, absoluteSize.y, 1.0f));
	}
}