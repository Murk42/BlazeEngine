#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"

namespace Blaze
{
	class Window;

	class BlazeEngineContext
	{
	public:
		struct InitializationParameters
		{

		};

		virtual ~BlazeEngineContext() { }

		Window* GetWindowFromHandle(void* handle) const;

		virtual void ProcessEvents() = 0;

		static bool InitializeEngine(const InitializationParameters& params);
		static void TerminateEngine();
		static BlazeEngineContext* GetEngineContext();
	};
}
