#pragma once
#include "BlazeEngine/Core/EngineCore.h"
#include "BlazeEngine/DataStructures/Vector.h"
#include "BlazeEngine/DataStructures/Matrix.h"

namespace Blaze::UI
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

	class BLAZE_API ElementTransform
	{
	public:
		Vec2f pos;
		float depth;
		Vec2f size;
		float scale;
		Align align;

		Vec2f absolutePos;
		Vec2f absoluteSize;

		Align parentAlign;		

		Mat4f mat;		

		ElementTransform();		
	};
}