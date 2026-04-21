#pragma once
#include "BlazeEngine/UI/Graphics/RenderUnit.h"

namespace Blaze::UI
{
	class BLAZE_API RenderableNode
	{
	public:
		virtual bool PreRender(const Graphics::RenderContext& renderContext) { return false; }
		virtual RenderUnitBase* GetRenderUnit(uintMem index) = 0;
	private:
	};
}