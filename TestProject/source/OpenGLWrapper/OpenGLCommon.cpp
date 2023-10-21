#include "pch.h"
#include "OpenGLCommon.h"

namespace OpenGLWrapper
{
	Result OpenGLResult;

	bool FlushOpenGLResult()
	{
		bool wasFull = OpenGLResult.operator bool();
		OpenGLResult = ::Result();
		return wasFull;
	}

}