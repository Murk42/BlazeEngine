#pragma once
#include "BlazeEngine/Core/EngineCore.h"
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

	class BLAZE_API Transform2D
	{
	public:
		Vec2f pos;
		float depth;
		Vec2f size;
		Mat4f mat;

		Vec2f absolutePos;

		Align parentAlign;
		Transform2D* parent;

		Transform2D();
		Transform2D(Vec2f pos, Vec2f size, Align parentAlign = Align::Center, Transform2D* parent = nullptr, float depth = 0);

		void Update();
	};
}