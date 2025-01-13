#pragma once
#include <concepts>
#include "BlazeEngineCore/Debug/LoggerListener.h"
#include "BlazeEngineCore/Debug/Result.h"

namespace Blaze
{
	template<typename F> requires std::invocable<F>
	bool CheckOpenGLError(const F& function)
	{
		class ErrorChecker : public Debug::LoggerListener
		{
		public:			
			ErrorChecker()
				: signaled(false)
			{

			}
			void NewResult(const Result& result) override
			{
				if (result.HighestLogType() == Debug::LogType::Error)
					signaled = true;
			}

			inline bool Signaled() const { return signaled; }
		private:
			bool signaled;
		};
		ErrorChecker checker;

		function();

		return checker.Signaled();
		
	}

#define CHECK_OPENGL_ERROR(x) CheckOpenGLError([&](){ x; })
}