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
		EventHandler(bool supress, bool listening);
		EventHandler(EventHandler&&) noexcept;
		~EventHandler();

		void StartSupressingEvents();
		void StopSupressingEvents();
		void StartListeningForEvents();
		void StopListeningForEvents();

		inline bool IsSupressing() const { return supress; };
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
		bool supress;
		bool listening;
		EventDispatcher<T>* dispatcher;
	};	

	template<typename T>
	inline EventHandler<T>::EventHandler()
		: supress(false), listening(true), dispatcher(nullptr)
	{
	}
	template<typename T>
	inline EventHandler<T>::EventHandler(bool supress, bool listening)
		: supress(supress), listening(listening), dispatcher(nullptr)
	{

	}
	template<typename T>
	inline EventHandler<T>::EventHandler(EventHandler&& other) noexcept 		
	{
		supress = other.supress;
		listening = other.listening;
		dispatcher = other.dispatcher;

		if (dispatcher != nullptr)
		{
			dispatcher->RemoveHandler(other);
			dispatcher->AddHandler(*this);
		}

		other.supress = false;
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
	inline void EventHandler<T>::StartSupressingEvents()
	{
		supress = true;
	}
	template<typename T>
	inline void EventHandler<T>::StopSupressingEvents()
	{
		supress = false;
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

		supress = other.supress;
		listening = other.listening;
		dispatcher = other.dispatcher;

		if (dispatcher != nullptr)
		{
			dispatcher->RemoveHandler(other);
			dispatcher->AddHandler(*this);
		}

		other.supress = false;
		other.listening = true;
		other.dispatcher = nullptr;

		return *this;
	}
}