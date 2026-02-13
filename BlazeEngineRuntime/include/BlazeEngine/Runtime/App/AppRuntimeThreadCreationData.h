#pragma once
#include "BlazeEngine/External/BlazeEngineCoreSTDInterface.h"
#include "BlazeEngine/Core/Container/Array.h"
#include "BlazeEngine/Core/Type/TypeTraits.h"
#include "BlazeEngine/Core/Common/Tuple.h"
#include "BlazeEngine/Core/Threading/Thread.h"
#include "BlazeEngine/Runtime/App/AppRuntimeThread.h"
#include <atomic>

namespace Blaze
{
	class Thread;

	class AppRuntimeThreadCreationData
	{
	public:
		AppRuntimeThreadCreationData();
		AppRuntimeThreadCreationData(AppRuntimeThreadCreationData&& other) noexcept = default;

		void Clear();

		template<IsDerivedFrom<AppRuntimeThread> T, typename ... Args> requires IsConstructibleFrom<T, Args...>
		void Initialize(Args&& ... args);

		bool InstantiateOnThread(Thread& thread);

		template<typename T, typename ... Args> requires IsConstructibleFrom<T, Args...>&& IsDerivedFrom<T, AppLayer>
		AppRuntimeThreadCreationData&& RegisterLayer(Args&& ... args);
		AppRuntimeThreadCreationData&& RegisterLayer(AppLayerCreationData&& layerCreationData);

		AppRuntimeThreadCreationData& operator=(AppRuntimeThreadCreationData&& other) noexcept = default;

		template<IsDerivedFrom<AppRuntimeThread> T, typename ... Args> requires IsConstructibleFrom<T, Args...>
		static AppRuntimeThreadCreationData New(Args&& ... args);
	private:
		struct ThreadArguments
		{
			AppRuntimeThreadCreationData& creationData;
			std::atomic_flag threadInitialized;
		};

		Thread::BasicThreadFunction threadFunction;
		Handle<VirtualTupleBase> runtimeThreadArguments;

		Array<AppLayerCreationData> layerCreationData;

		friend class App;
	};

	template<IsDerivedFrom<AppRuntimeThread> T, typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline void AppRuntimeThreadCreationData::Initialize(Args && ...args)
	{
		runtimeThreadArguments = Handle<VirtualTupleBase>::CreateDerived<VirtualTuple<Args...>>(std::forward<Args>(args)...);

		threadFunction = [](void* userData) -> int {
			AppRuntimeThreadCreationData& data = reinterpret_cast<ThreadArguments*>(userData)->creationData;
			Tuple<Args...> arguments = std::move(*static_cast<Tuple<Args...>*>(static_cast<VirtualTuple<Args...>*>(data.runtimeThreadArguments.Ptr())));
			Array<AppLayerCreationData> layerCreationData = std::move(data.layerCreationData);

			reinterpret_cast<ThreadArguments*>(userData)->threadInitialized.test_and_set();
			reinterpret_cast<ThreadArguments*>(userData)->threadInitialized.notify_one();

			T runtimeThreadDerived = make_from_tuple<T, Args...>(std::move(arguments));
			AppRuntimeThread& runtimeThread = runtimeThreadDerived;

			runtimeThread.queuedLayerCreationData.Append(std::move(layerCreationData));
			runtimeThread.Run();

			return 0;
			};
	}

	template<typename T, typename ...Args> requires IsConstructibleFrom<T, Args...> && IsDerivedFrom<T, AppLayer>
	inline AppRuntimeThreadCreationData&& AppRuntimeThreadCreationData::RegisterLayer(Args&& ... args)
	{
		AppLayerCreationData creationData;
		creationData.Initialize<T>(std::forward<Args>(args)...);
		RegisterLayer(std::move(creationData));
	}
	template<IsDerivedFrom<AppRuntimeThread> T, typename ...Args> requires IsConstructibleFrom<T, Args...>
	inline AppRuntimeThreadCreationData AppRuntimeThreadCreationData::New(Args && ...args)
	{
		AppRuntimeThreadCreationData creationData;
		creationData.Initialize<T>(std::forward<Args>(args)...);
		return creationData;
	}
}