#pragma once

namespace Blaze::OpenGL
{
	enum FenceReturnState
	{
		AlreadySignaled = 0x911A,
		TimeoutExpired = 0x911B,
		ConditionSatisfied = 0x911C,
		Error = 0x911D,
		FenceNotSet,
	};

	class BLAZE_API Fence
	{
		void* id;
	public:
		Fence();
		~Fence();

		Result SetFence();

		Result BlockServer();
		//The timeout is in seconds
		FenceReturnState BlockClient(double timeout);

		bool IsSet();
		bool IsSignaled();
	};
}