#include "pch.h"
#include "BlazeEngineGraphics/Core/OpenGL/OpenGLWrapper/OpenGLFence.h"

namespace Blaze::Graphics::OpenGLWrapper
{
	Fence::Fence()
		: id(nullptr)
	{		
	}
	Fence::Fence(void* id)
		: id(id)
	{
	}

	Fence::~Fence()
	{
		if (id != nullptr)
		{
			glDeleteSync((GLsync)id);
			
		}
	}
	void Fence::SetFence()
	{
		if (id != nullptr)
		{
			glDeleteSync((GLsync)id);			
		}

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

		GLenum ret = glClientWaitSync((GLsync)id, GL_SYNC_FLUSH_COMMANDS_BIT, timeout * 1000000000.0);		
		
		switch (ret)
		{
		case GL_ALREADY_SIGNALED:
			return FenceReturnState::AlreadySignaled;
			break;
		case GL_TIMEOUT_EXPIRED:
			return FenceReturnState::TimeoutExpired;
			break;
		case GL_CONDITION_SATISFIED:
			return FenceReturnState::ConditionSatisfied;
			break;
		case GL_WAIT_FAILED:
			return FenceReturnState::Error;
			break;
		default:
			return (FenceReturnState)-1;
			break;
		}		
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
		

		return (value == GL_SIGNALED ? true : false);
	}
}