#pragma once

namespace Blaze::Graphics::OpenGLWrapper
{
	extern Result OpenGLResult;

	bool FlushOpenGLResult();

#define CHECK_OPENGL_RESULT() if(OpenGLResult) return std::move(OpenGLResult);
}