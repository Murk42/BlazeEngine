#pragma once
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/Matrix.h"
#include "BlazeEngine/Math/Math.h"

namespace Blaze
{
	enum class Align
	{
		Top,
		TopRight,
		Right,
		BottomRight,
		Bottom,
		BottomLeft,
		Left,
		TopLeft,
		Center
	};

	struct Transform2D
	{
		Vec2f pos;
		float depth;
		Vec2f size;
		Mat4f mat;

		Vec2f absolutePos;

		Align parentAlign;
		Transform2D* parent;

		Transform2D()
			: pos(0), size(0), parentAlign(Align::Center), parent(nullptr)
		{

		}
		Transform2D(Vec2f pos, Vec2f size, Align parentAlign = Align::Center, Transform2D* parent = nullptr)
			: pos(pos), size(size), parentAlign(parentAlign), parent(parent)
		{
		}

		void Update()
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
				absolutePos = pos;
				mat = Math::TranslationMatrix<float>(Vec4i(absolutePos.x, absolutePos.y, 0.0f, 1.0f));
			}
		}
	};
}