#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Console/StyledOutputStream.h"

namespace Blaze
{
	class BLAZE_API ConsoleOutputStream : public StyledOutputStream
	{
	public:
		ConsoleOutputStream();
		~ConsoleOutputStream();

		uintMem Write(const void* ptr, uintMem byteCount) override;
		bool MovePosition(intMem offset) override;
		bool SetPosition(uintMem offset) override;
		bool SetPositionFromEnd(intMem offset) override;
		uintMem GetPosition() const override;
		uintMem GetSize() const override;

		void ResetStyle() override;
	private:
		void* handle;
		uint16 attribute;

		void ProcessStyleTag(u8StringView tag);
	};
}