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
			glDeleteSync((GLsync)id);
	}
	void Fence::SetFence()
	{
		if (id != nullptr)
			glDeleteSync((GLsync)id);			

		id = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
	}
	void Fence::BlockServer()
	{	
		if (id == nullptr)
			return;
		glWaitSync((GLsync)id, 0, GL_TIMEOUT_IGNORED);
	}
	FenceReturnState Fence::BlockClient(double timeout)
	{
		if (id == nullptr)
			return FenceReturnState::FenceNotSet;
		return (FenceReturnState)glClientWaitSync((GLsync)id, GL_SYNC_FLUSH_COMMANDS_BIT, timeout * 1000000000.0);		
	}
	bool Fence::IsSet()
	{
		return id == nullptr;
	}
	bool Fence::IsSignaled()
	{
		if (id == nullptr)
			return false;

		int value;		
		glGetSynciv((GLsync)id, GL_SYNC_STATUS, 1, 0, &value);
		return (value == GL_SIGNALED ? true : false);
	}
}