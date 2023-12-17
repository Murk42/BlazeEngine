#pragma once

namespace Blaze
{	
	namespace Console
	{								
		BLAZE_API void Write(char text);
		BLAZE_API void Write(StringView text);
		BLAZE_API void WriteLine(StringView text);

		BLAZE_API String Read();
	}

	namespace Debug::Logger
	{
		BLAZE_API void AddOutputToConsole();
		BLAZE_API void RemoveOutputFromConsole();
	}
}