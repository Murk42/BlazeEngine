#pragma once
#include "BlazeEngine/Runtime/BlazeEngineContext.h"
#include "BlazeEngine/Core/Container/Array.h"
#include "BlazeEngine/Core/Type/TypeTraits.h"
#include "BlazeEngine/Core/Common/Tuple.h"
#include "BlazeEngine/Runtime/App/AppRuntimeThread.h"
#include "BlazeEngine/External/BlazeEngineCoreSTDInterface.h"

namespace Blaze
{
	class AppRuntimeThreadCreationData
	{
	public:
		AppRuntimeThreadCreationData();
		AppRuntimeThreadCreationData(AppRuntimeThreadCreationData&& other) noexcept = default;

		void Clear();

		template<IsDerivedFrom<AppRuntimeThread> T, typename ... Args> requires IsConstructibleFrom<T, Args...>
		void Initialize(Args&& ... args);

		template<std::invocable<AppRuntimeThread&> F>
		void Run(const F& function);

		template<typename T, typename ... Args> requires IsConstructibleFrom<T, Args...>&& IsDerivedFrom<T, AppLayer>
		AppRuntimeThreadCreationData&& RegisterLayer(Args&& ... args);
		AppRuntimeThreadCreationData&& RegisterLayer(AppLayerCreationData&& layerCreationData);

		AppRuntimeThreadCreationData& operator=(AppRuntimeThreadCreationData&& other) noexcept = default;

		template<IsDerivedFrom<AppRuntimeThread> T, typename ... Args> requires IsConstructibleFrom<T, Args...>
		static AppRuntimeThreadCreationData New(Args&& ... args);
	private:
		using RunWithObjectFuntion = void(*)(AppRuntimeThread&, const void*);
		using CreateObjectFunction = void(AppRuntimeThreadCreationData::*)(RunWithObjectFuntion, const void*);

		CreateObjectFunction createObjectFunction;
		Handle<VirtualTupleBase> arguments;

		Array<AppLayerCreationData> layerCreationData;

		template<IsDerivedFrom<AppRuntimeThread> T, typename ...Args> requires IsConstructibleFrom<T, Args...>
		void SpecificCreateObjectFunction(RunWithObjectFuntion, const void*);

		friend class App;
	};

	template<IsDerivedFrom<AppRuntimeThread> T, typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline void AppRuntimeThreadCreationData::Initialize(Args && ...args)
	{
		arguments = Handle<VirtualTupleBase>::CreateDerived<VirtualTuple<Args...>>(std::forward<Args>(args)...);
		createObjectFunction = &AppRuntimeThreadCreationData::SpecificCreateObjectFunction<T, Args...>;
	}

	template<std::invocable<AppRuntimeThread&> F>
	inline void AppRuntimeThreadCreationData::Run(const F& function)
	{
		RunWithObjectFuntion relayFunction = [](AppRuntimeThread& thread, const void* data) { (*reinterpret_cast<const F*>(data))(thread); };
		(this->*createObjectFunction)(relayFunction, &function);
	}

	template<typename T, typename ...Args> requires IsConstructibleFrom<T, Args...> && IsDerivedFrom<T, AppLayer>
	inline AppRuntimeThreadCreationData&& AppRuntimeThreadCreationData::RegisterLayer(Args&& ... args)
	{
		AppLayerCreationData creationData;
		creationData.Initialize<T>(std::forward<Args>(args)...);
		RegisterLayer(std::move(creationData));

		return std::move(*this);
	}
	template<IsDerivedFrom<AppRuntimeThread> T, typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline AppRuntimeThreadCreationData AppRuntimeThreadCreationData::New(Args && ...args)
	{
		AppRuntimeThreadCreationData creationData;
		creationData.Initialize<T>(std::forward<Args>(args)...);
		return creationData;
	}
	template<IsDerivedFrom<AppRuntimeThread> T, typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline void AppRuntimeThreadCreationData::SpecificCreateObjectFunction(RunWithObjectFuntion function, const void* data)
	{
		T runtimeThreadDerived = make_from_tuple<T, Args...>(std::move(*static_cast<VirtualTuple<Args...>*>(arguments.Ptr())));

		for (auto& data : layerCreationData)
			runtimeThreadDerived.AddLayer(std::move(data));

		function(runtimeThreadDerived, data);
	}
}