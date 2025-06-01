#include "pch.h"

#include <Windows.h>
#include "BlazeEngineCore/Debug/Logger.h"

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
	String GetErrorString(uint32 error, uint recursionIndex)
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
				Debug::Logger::LogError("Windows API", "FormatMessageA returned \"{}\"", GetErrorString(lastError, recursionIndex + 1));
			else
				Debug::Logger::LogError("Windows API", "FormatMessageA returned error code {}. Blaze::Windows::GetErrorString recursion limit reached", lastError);

			return Format("error code {}", error);
		}

		if (errorString == nullptr)
		{
			BLAZE_ENGINE_CORE_ERROR("FormatMessageA passed a empyt string pointer");

			return Format("error code {}", error);
		}

		String out = String(errorString, res - 2); //-1 is to ignore the '\r\n' character

		LocalFree(errorString);

		return out;
	}
}