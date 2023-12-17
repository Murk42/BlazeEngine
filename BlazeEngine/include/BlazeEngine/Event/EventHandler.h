#pragma once

namespace Blaze
{
	template<typename T>
	class EventDispatcher;

	template<typename T>
	class EventHandler
	{
	public:
		EventHandler();
		EventHandler(bool suppress, bool listening);
		EventHandler(EventHandler&&) noexcept;
		~EventHandler();

		void StartsuppressingEvents();
		void StopsuppressingEvents();
		void StartListeningForEvents();
		void StopListeningForEvents();

		inline bool Issuppressing() const { return suppress; };
		inline bool IsListening() const { return listening; }
		inline EventDispatcher<T>* GetDispatcher() const { return dispatcher; }

		void UnsubscribeFromDispatcher();

		virtual void OnEvent(T event) = 0;
	protected:
		virtual void DispatcherDestroyed();
	public:

		EventHandler& operator=(EventHandler&&) noexcept;

		template <typename T>
		friend class EventDispatcher;
	private:
		bool suppress;
		bool listening;
		EventDispatcher<T>* dispatcher;
	};	

	template<typename T>
	inline EventHandler<T>::EventHandler()
		: suppress(false), listening(true), dispatcher(nullptr)
	{
	}
	template<typename T>
	inline EventHandler<T>::EventHandler(bool suppress, bool listening)
		: suppress(suppress), listening(listening), dispatcher(nullptr)
	{

	}
	template<typename T>
	inline EventHandler<T>::EventHandler(EventHandler&& other) noexcept 		
	{
		suppress = other.suppress;
		listening = other.listening;
		dispatcher = other.dispatcher;

		if (dispatcher != nullptr)
		{
			dispatcher->RemoveHandler(other);
			dispatcher->AddHandler(*this);
		}

		other.suppress = false;
		other.listening = true;
		other.dispatcher = nullptr;
	}
	template<typename T>
	inline EventHandler<T>::~EventHandler()
	{		
		if (dispatcher != nullptr)
			dispatcher->RemoveHandler(*this);
	}
	template<typename T>
	inline void EventHandler<T>::StartsuppressingEvents()
	{
		suppress = true;
	}
	template<typename T>
	inline void EventHandler<T>::StopsuppressingEvents()
	{
		suppress = false;
	}
	template<typename T>
	inline void EventHandler<T>::StartListeningForEvents()
	{
		listening = true;
	}
	template<typename T>
	inline void EventHandler<T>::StopListeningForEvents()
	{
		listening = false;
	}
	template<typename T>
	inline void EventHandler<T>::UnsubscribeFromDispatcher()
	{
		if (dispatcher != nullptr)
		{
			dispatcher->RemoveHandler(*this);
			dispatcher = nullptr;
		}
	}

	template<typename T>
	inline void EventHandler<T>::DispatcherDestroyed()
	{
	}

	template<typename T>
	inline EventHandler<T>& EventHandler<T>::operator=(EventHandler&& other) noexcept
	{
		if (dispatcher != nullptr)
			dispatcher->RemoveHandler(other);

		suppress = other.suppress;
		listening = other.listening;
		dispatcher = other.dispatcher;

		if (dispatcher != nullptr)
		{
			dispatcher->RemoveHandler(other);
			dispatcher->AddHandler(*this);
		}

		other.suppress = false;
		other.listening = true;
		other.dispatcher = nullptr;

		return *this;
	}
}