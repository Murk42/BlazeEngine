#include "pch.h"
#include <Windows.h>
#include "BlazeEngineCore/Utilities/StringParsing.h"

namespace Blaze::Windows
{
	template<typename T>
	class Guard
	{
	public:
		T function;
		Guard(const T& function)
			: function(function)
		{
		}
		~Guard()
		{
			function();
		}
	};
	String GetErrorString(DWORD error, uint recursionIndex)
	{
		LPSTR errorString;
		auto res = FormatMessageA(
			FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_IGNORE_INSERTS, 
			NULL, error, 0, (LPSTR)&errorString, 0, NULL
			);		

		if (res == 0)
		{
			DWORD lastError = GetLastError();

			if (recursionIndex > 8)
			{				
				String value = GetErrorString(lastError, recursionIndex + 1);
				Debug::Logger::LogError("Windows API", "FormatMessageA returned \"" + value + "\"");
			}
			else
				Debug::Logger::LogError("Windows API", "FormatMessageA returned error code " + StringParsing::Convert((uint32)lastError) + ". Blaze::Windows::GetErrorString recursion limit reached");

			return "error code " + StringParsing::Convert((uint32)error);
		}

		if (errorString == nullptr)
		{
			Debug::Logger::LogError("Blaze Engine", "FormatMessageA passed a empyt string pointer");

			return "error code " + StringParsing::Convert((uint32)error);
		}

		String out = String(errorString, res - 2); //-1 is to ignore the '\r\n' character

		LocalFree(errorString);

		return out;
	}
}