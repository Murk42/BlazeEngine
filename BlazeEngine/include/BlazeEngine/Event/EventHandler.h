#pragma once
#include "BlazeEngine/BlazeEngineDefines.h"

namespace Blaze
{
	template<typename T>
	class EventDispatcher;

	template<typename T>
	class BLAZE_API EventHandler
	{
	public:		
		using EventType = T;
		using DispatcherType = EventDispatcher<T>;
		EventHandler();
		EventHandler(bool suppress, bool listening);
		EventHandler(EventHandler&&) noexcept;
		~EventHandler();
			
		//There should be no dispatcher getter because the dispatcher can change unexpectedly, for
		//example when it is moved (using the move constructor or move assignment operator)		
		void UnsubscribeFromDispatcher();

		//The handler mustn't be destroyed in this function, but it can be unsubscribed from the dispatcher, or called recursively
		virtual void OnEvent(T event) = 0;			
		EventHandler& operator=(EventHandler&&) noexcept;

	protected:
		virtual void Unsubscribed();
	private:				
		DispatcherType* dispatcher;
		EventHandler* next;		
		std::recursive_mutex mutex;

		template <typename T>
		friend class EventDispatcher;
	};	
	
	template<typename T>
	inline EventHandler<T>::EventHandler()
		: dispatcher(nullptr), next(nullptr)
	{
	}
	template<typename T>
	inline EventHandler<T>::EventHandler(bool suppress, bool listening)
		: dispatcher(nullptr), next(nullptr)
	{

	}
	template<typename T>
	inline EventHandler<T>::EventHandler(EventHandler&& other) noexcept
		: dispatcher(nullptr), next(nullptr)
	{		
		std::unique_lock<std::recursive_mutex> lk{ other.mutex };

		if (other.dispatcher != nullptr)
		{
			auto dispatcher = other.dispatcher;
			dispatcher->RemoveHandler(other);
			dispatcher->AddHandler(*this);						
		}				
	}
	template<typename T>
	inline EventHandler<T>::~EventHandler()
	{		
		std::lock_guard<std::recursive_mutex> lk{ mutex };

		if (dispatcher != nullptr)
			dispatcher->RemoveHandler(*this);
	}		
	template<typename T>
	inline void EventHandler<T>::UnsubscribeFromDispatcher()
	{
		std::lock_guard<std::recursive_mutex> lk{ mutex };
		if (dispatcher != nullptr)
		{
			dispatcher->RemoveHandler(*this);
			dispatcher = nullptr;
		}
	}
	template<typename T>
	inline void EventHandler<T>::Unsubscribed()
	{
	}
	template<typename T>
	inline EventHandler<T>& EventHandler<T>::operator=(EventHandler&& other) noexcept
	{	
		{
			std::lock_guard<std::recursive_mutex> lk1{ mutex };
			if (dispatcher != nullptr)
				dispatcher->RemoveHandler(other);
		}

		std::lock_guard<std::recursive_mutex> lk2{ other.mutex };
		auto newDispatcher = other.dispatcher;
			
		if (newDispatcher != nullptr)
		{
			newDispatcher->RemoveHandler(other);
			newDispatcher->AddHandler(*this);
		}				

		return *this;
	}
}