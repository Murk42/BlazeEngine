#pragma once
#include "BlazeEngine/Core/Math/Vector.h"

namespace Blaze::Graphics
{
	class RenderContext
	{
	public:
		RenderContext(Vec2u targetSize);

		Vec2u GetTargetSize() const { return targetSize; }
	private:
		Vec2u targetSize;
	};
}