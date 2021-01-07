#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/GameSystem/Components/Transform.h"
#include "BlazeEngine/GameSystem/ECS/Component.h"
#include "BlazeEngine/DataStructures/Rect.h"

namespace Blaze
{
	enum class Align
	{
		Left,
		Right,
		Bottom,
		Top,
		BottomLeft,
		BottomRight,
		TopLeft,
		TopRight,
		Center,
	};

	class BLAZE_API Transform2D : public Component<Transform2D, Transform>, public Transform
	{
	public:
		Rectf rect;
		float rot;
		Vec2f pivot;
		Vec2f scale;

		Align rectAlign;
		Align viewportAlign;

		Transform2D();
		Transform2D(const Transform2D& t);
		Transform2D(Rectf rect, Align rectAlign = Align::Center, Align viewportAlign = Align::Center, Vec2f scale = Vec2f(1), float rot = 0, Vec2f pivot = Vec2f(0));

		void UpdateMatrix() override;
	};
}