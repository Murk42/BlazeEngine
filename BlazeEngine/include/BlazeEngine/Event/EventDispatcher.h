#pragma once
#include "BlazeEngine/Event/EventHandler.h"

namespace Blaze
{
	template<typename T>
	class EventHandler;

	template<typename T>
	class BLAZE_API EventDispatcher
	{
	public:		
		using EventType = T;
		using HandlerType = EventHandler<T>;

		EventDispatcher();
		EventDispatcher(const EventDispatcher& other) = delete;
		EventDispatcher(EventDispatcher&& other) noexcept;
		~EventDispatcher();
		
		void AddHandler(HandlerType& handler);
		void RemoveHandler(HandlerType& handler);
				
		void Call(const T& event); 

		EventDispatcher& operator=(const EventDispatcher& other) = delete;
		EventDispatcher& operator=(EventDispatcher&& other) noexcept;
	private:				
		struct Lock
		{
			std::atomic_flag flag;			

			Lock()
				: flag()
			{
			}

			void lock()
			{				
				while (flag.test_and_set())
					flag.wait(true);
			}			
			void unlock()
			{
				flag.clear();
				flag.notify_all();
			}
		};
		
		HandlerType* head;
		Lock lock;
		//4 bytes unused for padding

		template<typename T>
		friend class EventHandler;
	};	
	template<typename T>
	inline EventDispatcher<T>::EventDispatcher()
		: head(nullptr)
	{		
	}
	template<typename T>
	inline EventDispatcher<T>::EventDispatcher(EventDispatcher<T>&& other) noexcept				
		: head(nullptr)
	{
		std::lock_guard<Lock> lk{ other.lock };

		HandlerType* it = head;

		while (it != nullptr)
		{
			std::lock_guard<std::recursive_mutex> handlerLk{ it->mutex };

			it->dispatcher = this;

			it = it->next;
		}

		other.head = nullptr;
	}
	template<typename T>
	inline EventDispatcher<T>::~EventDispatcher()
	{
		std::lock_guard<Lock> lk{ lock };

		auto it = head;		
		
		while (it != nullptr)
		{
			std::lock_guard<std::recursive_mutex> handlerLk{ it->mutex };

			it->dispatcher = nullptr;	
			it->next = nullptr;
			it->Unsubscribed();

			it = it->next;
		}

		head = nullptr;
	}
	template<typename T>
	inline void EventDispatcher<T>::AddHandler(HandlerType& handler)
	{			
		std::unique_lock<std::recursive_mutex> handlerLk{ handler.mutex };

		if (handler.dispatcher == this)
			return;
		
		if (handler.dispatcher != nullptr)
		{
			handlerLk.unlock();
			handler.dispatcher->RemoveHandler(handler);
			handlerLk.lock();
		}

		handler.dispatcher = this;		

		std::lock_guard<Lock> lk{ lock };		
		handler.next = head;
		head = &handler;
	}
	template<typename T>
	inline void EventDispatcher<T>::RemoveHandler(HandlerType& handler)
	{
		{
			std::lock_guard<std::recursive_mutex> handlerLk{ handler.mutex };
			if (handler.dispatcher != this)
				Debug::Logger::LogError("Blaze Engine", "Cannot remove handler from a dispatcher that it isn't subscribed to");
		}
		{
			HandlerType* it;
			{
				std::lock_guard<Lock> lk{ lock };

				if (head == nullptr)
					Debug::Logger::LogFatal("Blaze Engine", "Implementation error");

				it = head;

				if (it == &handler)
					head = head->next;
			}
			
			std::unique_lock<std::recursive_mutex> handlerLk{ it->mutex };
			if (it != &handler)
			{				
				while (it->next != &handler && it->next != nullptr)
				{
					it = it->next;					
					handlerLk = std::unique_lock<std::recursive_mutex>{ it->mutex };
				}

				if (it->next == nullptr)
					Debug::Logger::LogFatal("Blaze Engine", "Implementation error");

				it->next = it->next->next;

				handlerLk = std::unique_lock<std::recursive_mutex>{ handler.mutex };
			}

			handler.dispatcher = nullptr;
			handler.next = nullptr;
		}

		handler.Unsubscribed();
	}
	template<typename T>
	inline void EventDispatcher<T>::Call(const T& event)
	{					
		HandlerType* it = nullptr;

		std::unique_lock<std::recursive_mutex> handlerLk;
		{
			std::lock_guard<Lock> lk{ lock };
			if (head == nullptr)
				return;

			it = head;
		
			handlerLk = std::unique_lock<std::recursive_mutex>{ it->mutex };
		}
		while (true)
		{
			HandlerType* next = it->next;

			it->OnEvent(event);							

			if (next == nullptr)
				break;

			it = next;

			handlerLk = std::unique_lock<std::recursive_mutex>{ it->mutex };
		}
	}	
	template<typename T>
	inline EventDispatcher<T>& EventDispatcher<T>::operator=(EventDispatcher&& other) noexcept
	{
		head = other.head;

		HandlerType* it = head;
		while (it != nullptr)
		{
			it->dispatcher = this;			

			it = it->next;
		} 

		other.head = nullptr;
		return *this;
	}		
}