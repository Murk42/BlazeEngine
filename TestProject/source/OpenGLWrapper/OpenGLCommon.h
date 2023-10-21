#pragma once

namespace OpenGLWrapper
{
	extern Result OpenGLResult;

	bool FlushOpenGLResult();

#define CHECK_OPENGL_RESULT() if(OpenGLResult) return std::move(OpenGLResult);
}
#define OPENGL_WRAPPER_API