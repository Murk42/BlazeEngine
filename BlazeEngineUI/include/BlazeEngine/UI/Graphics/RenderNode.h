#pragma once
#include "BlazeEngine/UI/Graphics/RenderUnit.h"

namespace Blaze::UI
{
	class RenderNode
	{
	public:
		virtual void PreRender(const RenderContext& renderContext) { }
		virtual RenderUnitBase* GetRenderUnit(uintMem index) = 0;
	private:
	};
}