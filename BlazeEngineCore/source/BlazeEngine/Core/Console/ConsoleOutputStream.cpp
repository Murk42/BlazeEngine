#include "pch.h"
#include "BlazeEngine/Core/Console/ConsoleOutputStream.h"

#ifdef BLAZE_PLATFORM_WINDOWS
#include <Windows.h>
#else
#error
#endif

namespace Blaze
{
	ConsoleOutputStream::ConsoleOutputStream()
		: handle(GetStdHandle(STD_OUTPUT_HANDLE))
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		SetConsoleOutputCP(65001);
#endif
	}
	ConsoleOutputStream::~ConsoleOutputStream()
	{
	}
	uintMem ConsoleOutputStream::Write(const void* ptr, uintMem byteCount)
	{
		uintMem printBegin = 0;
		uintMem printEnd = 0;

		u8StringView string{ reinterpret_cast<const char8_t*>(ptr), byteCount };
		for (uintMem i = 0; i < byteCount; ++i)
		{
			if (string[i] == '<')
			{
#ifdef BLAZE_PLATFORM_WINDOWS
				WriteConsoleA(handle, (const char*)ptr + printBegin, printEnd - printBegin, NULL, NULL);
#endif

				uintMem j = string.Find(">", i);

				if (j == SIZE_MAX)
				{
					printBegin = 0;
					printEnd = 0;
					return byteCount;
				}

				ProcessStyleTag(string.SubString(i + 1, j));

				printBegin = j + 1;
				printEnd = printBegin;

				i = j;
			}
			else
				++printEnd;
		}

#ifdef BLAZE_PLATFORM_WINDOWS
			WriteConsoleA(handle, (const char*)ptr + printBegin, printEnd - printBegin, NULL, NULL);
#endif

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
	void ConsoleOutputStream::ResetStyle()
	{
#ifdef BLAZE_PLATFORM_WINDOWS
		SetConsoleTextAttribute(handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
	}
	bool GetAttributeFromString(u8StringView text, uint16& attribute)
	{
		if (text == "red")
			attribute = FOREGROUND_RED;
		else if (text == "green")
			attribute = FOREGROUND_GREEN;
		else if (text == "blue")
			attribute = FOREGROUND_BLUE;
		else if (text == "yellow")
			attribute = FOREGROUND_RED | FOREGROUND_GREEN;
		else if (text == "purple")
			attribute = FOREGROUND_RED | FOREGROUND_BLUE;
		else if (text == "cyan")
			attribute = FOREGROUND_GREEN | FOREGROUND_BLUE;
		else if (text == "white")
			attribute = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
		else if (text == "black")
			attribute = 0;
		else if (text == "RED")
			attribute = FOREGROUND_RED | FOREGROUND_INTENSITY;
		else if (text == "GREEN")
			attribute = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		else if (text == "BLUE")
			attribute = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		else if (text == "YELLOW")
			attribute = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
		else if (text == "PURPLE")
			attribute = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		else if (text == "CYAN")
			attribute = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		else if (text == "WHITE")
			attribute = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
		else if (text == "BLACK")
			attribute = FOREGROUND_INTENSITY;
		else
			return false;

		return true;
	}
	void ConsoleOutputStream::ProcessStyleTag(u8StringView tag)
	{
		if (tag.StartsWith("color"))
		{
			if (tag.Count() == 5)
			{

			}
			else if (tag[5] == '/')
			{
				attribute &= ~15;
				attribute |= 7;
#ifdef BLAZE_PLATFORM_WINDOWS
				SetConsoleTextAttribute(handle, attribute);
#endif
			}
			else if (tag[5] == '=')
			{
				auto color = tag.SubString(6, -1);

#ifdef BLAZE_PLATFORM_WINDOWS
				uint16 attribute;
				if (GetAttributeFromString(color, attribute))
				{
					this->attribute = (this->attribute & ~15) | (attribute & 15);
					SetConsoleTextAttribute(handle, this->attribute);
				}
#endif

			}
		}
		else if (tag.StartsWith("background"))
		{
			if (tag.Count() == 10)
			{

			}
			else if (tag[10] == '/')
			{
				attribute &= ~240;
#ifdef BLAZE_PLATFORM_WINDOWS
				SetConsoleTextAttribute(handle, attribute);
#endif
			}
			else if (tag[10] == '=')
			{
				auto color = tag.SubString(11, -1);

#ifdef BLAZE_PLATFORM_WINDOWS
				uint16 attribute;
				if (GetAttributeFromString(color, attribute))
				{
					this->attribute = (this->attribute & ~240) | ((attribute << 4) & 240);
					SetConsoleTextAttribute(handle, this->attribute);
				}
#endif
			}
		}
	}
}