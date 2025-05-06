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
	void Fence::Clear()
	{		
		if (id != nullptr)
		{
			glDeleteSync((GLsync)id);
			id = nullptr;
		}
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
		{
			BLAZE_ENGINE_GRAPHICS_FATAL("Trying to block server with a fence that is not set");
			return;
		}			

		glWaitSync((GLsync)id, 0, GL_TIMEOUT_IGNORED);				
	}
	FenceReturnState Fence::BlockClient(double timeout)
	{
		if (id == nullptr)
			return FenceReturnState::FenceNotSet;

		GLenum ret = glClientWaitSync((GLsync)id, GL_SYNC_FLUSH_COMMANDS_BIT, timeout * 1000000000.0);				
		
		FenceReturnState returnState;
		switch (ret)
		{
		case GL_ALREADY_SIGNALED:
			returnState = FenceReturnState::AlreadySignaled;
			break;
		case GL_TIMEOUT_EXPIRED:
			returnState = FenceReturnState::TimeoutExpired;
			break;
		case GL_CONDITION_SATISFIED:
			returnState = FenceReturnState::ConditionSatisfied;
			break;
		case GL_WAIT_FAILED:
			returnState = FenceReturnState::Error;
			break;
		default:			
			BLAZE_ENGINE_GRAPHICS_FATAL("glClie ntWaitSync returned an invalid value");
			returnState = (FenceReturnState)-1;
			break;
		}

		return returnState;
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
	void Fence::Swap(Fence& other)
	{
		void* temp = other.id;
		other.id = id;
		id = temp;
	}
	void* Fence::ReleaseHandleOwnership()
	{
		void* id = this->id;
		this->id = nullptr;
		return id;
	}
}