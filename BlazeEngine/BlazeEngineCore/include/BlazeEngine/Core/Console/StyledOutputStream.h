#pragma once
#include "BlazeEngine/Core/Common/Stream.h"

namespace Blaze
{
	class StyledWriteStream : public WriteStream
	{
	public:
		virtual void ResetStyle() = 0;
		virtual uintMem WriteWithoutStyle(const void* ptr, uintMem byteCount) = 0;
	};
}