#pragma once

namespace Blaze
{	
	namespace Console
	{								
		BLAZE_API void Write(char text);
		BLAZE_API void Write(StringViewUTF8 text);
		BLAZE_API void WriteLine(StringViewUTF8 text);

		BLAZE_API StringUTF8 Read();
	}

	namespace Debug::Logger
	{
		BLAZE_API void AddOutputToConsole();
		BLAZE_API void RemoveOutputFromConsole();
	}
}