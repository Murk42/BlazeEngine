#pragma once
#include "BlazeEngineCore/BlazeEngineCoreDefines.h"
#include "BlazeEngineCore/Common/Stream.h"

namespace Blaze
{
	class BLAZE_API ConsoleOutputStream : public WriteStream
	{
		WriteStream* linkStream;
	public:
		uintMem Write(const void* ptr, uintMem byteCount) override;
		bool MovePosition(intMem offset) override;
		bool SetPosition(uintMem offset) override;
		bool SetPositionFromEnd(intMem offset) override;
		uintMem GetPosition() const override;
		uintMem GetSize() const override;
	};
}