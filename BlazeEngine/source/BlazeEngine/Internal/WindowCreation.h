#pragma once
#include "BlazeEngine/Core/Window.h"
#include <future>
#include <queue>

namespace Blaze
{			
	template<typename Request, typename Result>
	class ThreadRequestHandler
	{
		std::mutex waitingMutex;
		
		Request request;
		Result result;

		std::atomic_flag activeFlag;		
	public:
		Result Request(const Request& request)
		{
			std::lock_guard<std::mutex> waiting_LG { waitingMutex };			
							
			this->request = request;

			activeFlag.test_and_set();							
			activeFlag.wait(true);			

			return std::move(result);
		}

		template<typename F>
		void CheckRequests(const F& func)
		{
			if (!activeFlag.test())
				return;
			
			result = func(request);

			activeFlag.clear();
			activeFlag.notify_one();
		}
	};

	template<typename Request>
	class ThreadRequestHandler<Request, void>
	{
		std::mutex queueMutex;

		Array<Request> requests;				
	public:
		void Request(const Request& request)
		{
			std::lock_guard<std::mutex> lg { queueMutex };

			requests.AddBack(request);			
		}

		template<typename F>
		void CheckRequests(const F& func)
		{
			std::lock_guard<std::mutex> lg { queueMutex };

			for (auto& request : requests)
				func(request);
			
			requests.Clear();
		}
	};



	struct WindowCreationData
	{		
		String name;
		Vec2i pos;
		Vec2i size;
		WindowHandleGraphicsaAPI graphicsAPI;		
	};
	struct WindowDestructionData
	{
		WindowHandle handle;
	};
	
	extern ThreadRequestHandler<WindowCreationData, WindowHandle> windowCreationQueue;
	extern ThreadRequestHandler<WindowDestructionData, void> windowDestructionQueue;
	
}