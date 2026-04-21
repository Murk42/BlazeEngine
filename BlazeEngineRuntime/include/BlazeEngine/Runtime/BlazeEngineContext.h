#pragma once
#include "BlazeEngine/Core/BlazeEngineCoreDefines.h"
#include "BlazeEngine/Core/Time/TimingTree.h"

namespace Blaze
{
	class TimingTree;
	class Window;

	class BlazeEngineContext
	{
	public:
		struct InitializationParameters
		{

		};

		virtual ~BlazeEngineContext() { }

		Window* GetWindowFromHandle(void* handle) const;
		void SetInitializationTimingTree(TimingTree initializationTimingTree);

		inline const TimingTree& GetInitializationTimingTree() const { return initializationTimingTree; }

		virtual void PollEvents() = 0;
		virtual bool WaitForEvents(float seconds) = 0;

		virtual void NudgeMainThread() = 0;

		static bool InitializeEngine(const InitializationParameters& params, TimingTree& timingTree);
		static void TerminateEngine();
		static BlazeEngineContext* GetEngineContext();
	private:
		TimingTree initializationTimingTree;
	};
}
