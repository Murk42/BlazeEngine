#include "BlazeEngine/Graphics/OpenGL/OpenGLFence.h"
#include "GL/glew.h"

namespace Blaze::OpenGL
{
	Fence::Fence()
		: id(nullptr)
	{		
	}	
	Fence::~Fence()
	{
		if (id != nullptr)
		{
			glDeleteSync((GLsync)id);
			FlushOpenGLResult();
		}
	}
	Result Fence::SetFence()
	{
		if (id != nullptr)
		{
			glDeleteSync((GLsync)id);
			CHECK_OPENGL_RESULT();
		}

		id = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		CHECK_OPENGL_RESULT();

		return Result();
	}
	Result Fence::BlockServer()
	{	
		if (id == nullptr)
			return Result();

		glWaitSync((GLsync)id, 0, GL_TIMEOUT_IGNORED);
		CHECK_OPENGL_RESULT();

		return Result();
	}
	FenceReturnState Fence::BlockClient(double timeout)
	{
		if (id == nullptr)
			return FenceReturnState::FenceNotSet;
		auto ret = (FenceReturnState)glClientWaitSync((GLsync)id, GL_SYNC_FLUSH_COMMANDS_BIT, timeout * 1000000000.0);		
		FlushOpenGLResult();

		return ret;
	}
	bool Fence::IsSet()
	{
		return id != nullptr;
	}
	bool Fence::IsSignaled()
	{
		if (id == nullptr)
			return false;

		int value;		
		glGetSynciv((GLsync)id, GL_SYNC_STATUS, 1, 0, &value);
		FlushOpenGLResult();

		return (value == GL_SIGNALED ? true : false);
	}
}