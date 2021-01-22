#include "BlazeEngine/DataStructures/Transform2D.h"
#include "BlazeEngine/Graphics/Renderer.h"

namespace Blaze
{
	Transform2D::Transform2D()
		: pos(0), size(0), parentAlign(Align::Center), parent(nullptr)
	{

	}
	Transform2D::Transform2D(Vec2f pos, Vec2f size, Align parentAlign, Transform2D* parent, float depth)
		: pos(pos), size(size), parentAlign(parentAlign), parent(parent), depth(depth)
	{
	}

	void Transform2D::Update()
	{
		if (parent != nullptr)
		{
			Vec2f offset;
			switch (parentAlign)
			{
			case Align::Top:		 offset = Vec2f((parent->size.x - size.x) / 2, parent->size.y - size.y); break;
			case Align::TopRight:	 offset = Vec2f(parent->size.x - size.x, parent->size.y - size.y); break;
			case Align::Right:		 offset = Vec2f(parent->size.x - size.x, (parent->size.y - size.y) / 2); break;
			case Align::BottomRight: offset = Vec2f(parent->size.x - size.x, 0); break;
			case Align::Bottom:		 offset = Vec2f((parent->size.x - size.x) / 2, 0); break;
			case Align::BottomLeft:	 offset = Vec2f(0, 0); break;
			case Align::Left:		 offset = Vec2f(0, (parent->size.y - size.y) / 2); break;
			case Align::TopLeft:	 offset = Vec2f(0, parent->size.y - size.y); break;
			case Align::Center:		 offset = Vec2f((parent->size.x - size.x) / 2, (parent->size.y - size.y) / 2); break;
			}

			absolutePos = parent->absolutePos + offset + pos;
			mat = Math::TranslationMatrix<float>(Vec4i(absolutePos.x, absolutePos.y, 0.0f, 1.0f));
		}
		else
		{		
			Vec2f viewportSize = Renderer::GetViewportSize();
			Vec2f offset;
			switch (parentAlign)
			{
			case Align::Top:		 offset = Vec2f((viewportSize.x - size.x) / 2, viewportSize.y - size.y); break;
			case Align::TopRight:	 offset = Vec2f(viewportSize.x - size.x, viewportSize.y - size.y); break;
			case Align::Right:		 offset = Vec2f(viewportSize.x - size.x, (viewportSize.y - size.y) / 2); break;
			case Align::BottomRight: offset = Vec2f(viewportSize.x - size.x, 0); break;
			case Align::Bottom:		 offset = Vec2f((viewportSize.x - size.x) / 2, 0); break;
			case Align::BottomLeft:	 offset = Vec2f(0, 0); break;
			case Align::Left:		 offset = Vec2f(0, (viewportSize.y - size.y) / 2); break;
			case Align::TopLeft:	 offset = Vec2f(0, viewportSize.y - size.y); break;
			case Align::Center:		 offset = Vec2f((viewportSize.x - size.x) / 2, (viewportSize.y - size.y) / 2); break;
			}

			absolutePos = offset + pos;
			mat = Math::TranslationMatrix<float>(Vec4i(absolutePos.x, absolutePos.y, 0.0f, 1.0f));
		}
	}
}