#pragma once
#include "BlazeEngine/UI/Graphics/UIRenderUnit.h"

namespace Blaze::UI
{
	class RenderNode
	{
	public:
		virtual void PreRender(const UIRenderContext& renderContext) { }
		virtual UIRenderUnitBase* GetRenderUnit(uintMem index) = 0;
	private:
	};
}