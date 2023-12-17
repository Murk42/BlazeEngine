#pragma once
#include "BlazeEngine/Window/WindowSDL.h"

namespace Blaze
{			
	template<typename Request, typename Result>
	class ThreadRequestHandler
	{
		std::mutex waitingMutex;
		
		Request request;
		Result result;
						
		std::atomic_uint8_t activeFlag;				
	public:		
		Result Request(const Request& request)
		{			
			std::lock_guard<std::mutex> waiting_LG { waitingMutex };			
							
			this->request = request;

			activeFlag = 1;
			activeFlag.wait(1);										

			return std::move(result);
		}

		template<typename F>
		void CheckRequests(const F& func)
		{
			if (activeFlag.load() == 0)
				return;
			
			result = func(request);

			activeFlag = 0;
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



	struct WindowSDLCreationData
	{		
		String name;
		Vec2i pos;
		Vec2u size;
		WindowHandleGraphicsaAPI graphicsAPI;		
	};
	struct WindowSDLDestructionData
	{
		WindowSDL::WindowSDLHandle handle;
	};
	
	extern ThreadRequestHandler<WindowSDLCreationData, WindowSDL::WindowSDLHandle> windowSDLCreationQueue;
	extern ThreadRequestHandler<WindowSDLDestructionData, void> windowSDLDestructionQueue;
	
}