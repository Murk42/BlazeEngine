#pragma once
#include <concepts>
#include "BlazeEngine/Core/Debug/LoggerListener.h"
#include "BlazeEngine/Core/Debug/Result.h"

namespace Blaze::Graphics
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
			void NewLog(const Debug::Log& result) override
			{
				if (result.GetType() == Debug::LogType::Error)
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