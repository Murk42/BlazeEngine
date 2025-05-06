#pragma once
#include "BlazeEngineCore/Event/EventHandler.h"

namespace Blaze
{
	template<typename T>
	class BLAZE_CORE_API LambdaEventHandler : public EventHandler<T>
	{
	public:
		using Function = std::function<void(const T&)>;

		LambdaEventHandler();
		LambdaEventHandler(const Function& function);
		LambdaEventHandler(Function&& function);
		~LambdaEventHandler();						

		void SetFunction(const Function& function);
		void SetFunction(Function&& function);

		void OnEvent(const T& event) override;

		inline const Function& GetFunction();

		LambdaEventHandler& operator=(const Function& function);
	private:
		Function function;
	};
	template<typename T>
	inline LambdaEventHandler<T>::LambdaEventHandler()
	{
	}
	template<typename T>
	inline LambdaEventHandler<T>::LambdaEventHandler(const Function& function)
		: function(function)
	{

	}
	template<typename T>
	inline LambdaEventHandler<T>::LambdaEventHandler(Function&& function)
		: function(std::move(function))
	{

	}
	template<typename T>
	inline LambdaEventHandler<T>::~LambdaEventHandler()
	{		
	}
	template<typename T>
	inline void LambdaEventHandler<T>::SetFunction(const Function& function)
	{
		this->function = function;
	}
	template<typename T>
	inline void LambdaEventHandler<T>::SetFunction(Function&& function)
	{
		this->function = std::move(function);
	}
	template<typename T>
	inline LambdaEventHandler<T>& LambdaEventHandler<T>::operator=(const Function& function)
	{
		SetFunction(function);
	}
	template<typename T>
	inline void LambdaEventHandler<T>::OnEvent(const T& event)
	{
		if (function)
			function(event);
	}
}