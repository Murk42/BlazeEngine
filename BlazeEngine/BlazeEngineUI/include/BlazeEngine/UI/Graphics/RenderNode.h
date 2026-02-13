#pragma once
#include "BlazeEngine/UI/Graphics/RenderUnit.h"

namespace Blaze::UI
{
	class RenderNode
	{
	public:
		virtual bool PreRender(const RenderContext& renderContext) { return false; }
		virtual RenderUnitBase* GetRenderUnit(uintMem index) = 0;
	private:
	};
}