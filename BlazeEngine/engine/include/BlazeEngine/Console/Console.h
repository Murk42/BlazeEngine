#pragma once

namespace Blaze
{	
	namespace Console
	{								
		void BLAZE_API Write(char text);
		void BLAZE_API Write(StringView text);
		void BLAZE_API WriteLine(StringView text);		

		String BLAZE_API Read();
	}
}