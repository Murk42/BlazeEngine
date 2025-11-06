#pragma once
#include "BlazeEngine/Core/Common/Stream.h"

namespace Blaze
{
	class StyledOutputStream : public WriteStream
	{
	public:
		virtual void ResetStyle() = 0;
	};
}