#include "pch.h"
#include "BlazeEngine/Console/ConsoleOutputStream.h"

#include <iostream>

namespace Blaze
{
	uintMem ConsoleOutputStream::Write(const void* ptr, uintMem byteCount)
	{
		std::cout.write((const char*)ptr, byteCount);		
		return byteCount;
	}
	bool ConsoleOutputStream::MovePosition(intMem offset)
	{
		return false;
	}
	bool ConsoleOutputStream::SetPosition(uintMem offset)
	{
		return false;
	}
	bool ConsoleOutputStream::SetPositionFromEnd(intMem offset)
	{
		return false;
	}
	uintMem ConsoleOutputStream::GetPosition() const
	{
		return 0;
	}
	uintMem ConsoleOutputStream::GetSize() const
	{
		return 0;
	}
}