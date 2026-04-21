#pragma once
#include "BlazeEngine/Runtime/App/AppLayer.h"
#include "BlazeEngine/UI/Common/System.h"

namespace Blaze
{
	class UIDebuggableLayer : public virtual AppLayer
	{
	public:
		virtual ~UIDebuggableLayer() {}

		virtual UI::System* GetUISystem() = 0;
	};
}