#pragma once

namespace Blaze
{
	void Terminate();

	struct SafeTerminate
	{
		~SafeTerminate()
		{
			Terminate();
		}
	};
	extern SafeTerminate safeTerminate;
}