#pragma once
#include "BlazeEngine/Event/EventHandler.h"

namespace Blaze
{
	template<typename T>
	class LambdaEventHandler : public EventHandler<T>
	{
	public:
		LambdaEventHandler();
		LambdaEventHandler(bool supress, bool listening);
		LambdaEventHandler(const std::function<void(T)>& function);
		LambdaEventHandler(std::function<void(T)>&& function);
		~LambdaEventHandler();						

		void SetFunction(const std::function<void(T)>& function);
		void SetFunction(std::function<void(T)>&& function);

		LambdaEventHandler& operator=(const std::function<void(T)>& function);
		LambdaEventHandler& operator=(std::function<void(T)>&& function);
	private:
		std::function<void(T)> function;

		void OnEvent(T event);
	};
	template<typename T>
	inline LambdaEventHandler<T>::LambdaEventHandler()
	{
	}
	template<typename T>
	inline LambdaEventHandler<T>::LambdaEventHandler(bool supress, bool listening)
		: EventHandler(supress, listening)
	{
	}
	template<typename T>
	inline LambdaEventHandler<T>::LambdaEventHandler(const std::function<void(T)>& function)
		: function(function)
	{

	}
	template<typename T>
	inline LambdaEventHandler<T>::LambdaEventHandler(std::function<void(T)>&& function)
		: function(std::move(function))
	{

	}
	template<typename T>
	inline LambdaEventHandler<T>::~LambdaEventHandler()
	{		
	}
	template<typename T>
	inline void LambdaEventHandler<T>::SetFunction(const std::function<void(T)>& function)
	{
		this->function = function;
	}
	template<typename T>
	inline void LambdaEventHandler<T>::SetFunction(std::function<void(T)>&& function)
	{
		this->function = std::move(function);
	}
	template<typename T>
	inline LambdaEventHandler<T>& LambdaEventHandler<T>::operator=(const std::function<void(T)>& function)
	{
		SetFunction(function);
	}
	template<typename T>
	inline LambdaEventHandler<T>& LambdaEventHandler<T>::operator=(std::function<void(T)>&& function)
	{
		SetFunction(std::move(function));
	}
	template<typename T>
	inline void LambdaEventHandler<T>::OnEvent(T event)
	{
		if (function)
			function(event);
	}
}